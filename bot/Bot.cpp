#include "Bot.hpp"
#include "Ant.hpp"
#include "Map.hpp"
#include "Logger.hpp"
#include "Tracker.hpp"
#include "Room.hpp"
#include "RoomContent.hpp"
#include "Util.hpp"
#include "Coordinator.hpp"
#include "PathFinder.hpp"
#include <algorithm>
#include <iostream>
#include <limits>

#ifdef DEBUG
#	include "DebugWindow.hpp"
#endif


using namespace std;

//constructor
Bot::Bot(IODevice& io_device)
	: io(io_device)
	, state(*g_state)
	, firstTurn(true)
{
	setupRandomDirections();
}

void Bot::setupRandomDirections() {
	DirVec dirs(4);
	dirs[0] = 0;
	dirs[1] = 1;
	dirs[2] = 2;
	dirs[3] = 3;

	do {
		dirVecs.push_back(dirs);
	} while(std::next_permutation(dirs.begin(), dirs.end()));
}

DirVec const& Bot::randomDirVec() const {
	return dirVecs[rand()%dirVecs.size()];
}

//plays a single game of Ants.
void Bot::playGame() {
	while(playOneTurn())
		;
}

bool Bot::playOneTurn()
{
	if (!(io.input() >> state))
		return false;
	if (firstTurn)
	{
		state.setup();
		firstTurn = false;
	}
	else
	{
		makeMoves();
	}
	endTurn();
	return true;
}

void lookForFood(Ant* ant) {
	if (ant->state() != Ant::STATE_NONE && ant->state() != Ant::STATE_GOING_TO_ROOM)
		return; // Dont abort it!

	PathList paths;
	int maxDist = 30; // FIXME: search radius.
	if (PathFinder::findPaths(&paths, ant->pos(), FoodGoal(), 1, maxDist)) {
		Pos dest = paths[0].dest();
		LOG_DEBUG("Sending " << *ant << " to food at " << dest);
		// FIXME: reuse path directly.
		ant->goToFoodAt(dest);
	}

	/*
	// Try find food in room:
	Vec2 pos = ant->pos();
	RoomContent* rc = g_map->roomContentAt(pos);
	const PosSet& food = rc->m_food;

	if (!food.empty()) {
		// Go to random food:
		LOG_DEBUG("Going to food");
		PosSet::const_iterator it = food.begin();
		advance(it, rand()%food.size());
		ant->goToFoodAt(*it);
	}
	*/
}

// Finds an ant that is either idle, or going to a food further away than this food
class HungryAntFinder : public Goal {
public:
	virtual bool findGoalsInRoom(PosList& outPos, Room* room, const Pos& pos, int dist) const {
		const AntSet& ants = room->content()->ants();
		ITC(AntSet, ait, ants) {
			Ant* ant = *ait;

			if (ant->state()==Ant::STATE_NONE || ant->state()==Ant::STATE_GOING_TO_ROOM) {
				outPos.push_back(ant->pos());
				continue;
			}

			if (ant->state()==Ant::STATE_GOING_TO_FOOD) {
				// Is this food coser?
				int foodDist = dist + g_map->manhattanDist(pos, ant->pos());
				if (foodDist < ant->path().distanceLeft()) {
					outPos.push_back(ant->pos());
					continue;
				}
			}
		}

		return !outPos.empty();
	}
};

//makes the bots moves for the turn
void Bot::makeMoves()
{
	LOG_DEBUG("Bot::makeMoves");

	// Update current ant states
	AntSet& ants = g_tracker->getAnts();
	IT(AntSet, it, ants)
		(*it)->updateState();

	// Send free ants on food mission
	ITC(AntSet, it, ants)
		lookForFood(*it);

	// Distribute food to close ants
	const PosSet& food = g_tracker->getFood();
	ITC(PosSet, pit, food) {
		// Find closest ant to this
		Pos foodPos = *pit;
		if (g_map->square(foodPos).destinyAnt)
			continue; // Someone is already heading for this food.

		LOG_DEBUG("Looking for ant close to food...");

		PathList paths;
		HungryAntFinder goal;
		int maxDist = 25; // FIXME: Search radius.
		if (PathFinder::findPaths(&paths, foodPos, goal, 1, maxDist)) {
			LOG_DEBUG("sending ant to food.");
			Path path = paths[0];
			Ant* ant = g_map->getAntAt(path.dest());
			/*
			  // TODO: (quicker)
			path.reverse();
			ant->goToFood(path);
			/*/
			ant->goToFoodAt(foodPos);
			/**/
		}

		/*
		Ant* closest=NULL;
		int dist = std::numeric_limits<int>::max();

		ITC(AntSet, it, ants) {
			Ant* a = *it;
			if (a->state()!=Ant::STATE_GOING_TO_FOOD) {
				int d = g_map->manhattanDist(a->pos(), foodPos);
				if (d < dist) {
					dist = d;
					closest = a;
				}
			}
		}

		if (closest) {
			LOG_DEBUG("sending ant to food.");
			closest->goToFoodAt(foodPos);
		}
		*/
	}


	// Find crowded rooms, send ants out of them...
	ITC(RoomList, rit, g_rooms->rooms()) {
		Room* room = *rit;
		RoomContent* rc = room->content();
		const AntSet& roomAnts = rc->ants();

		const RoomSet& neighRooms = room->neighborRooms();

		if (!roomAnts.empty() && !neighRooms.empty()) {
			/* TODO: base on the number of food expected to be found, related to:
			foodExp = room->area() * room->timeSinceLastVisit();
			 */
			RoomList cands;
			ITC(RoomSet, rit, neighRooms) {
				if ((*rit)->content()->ants().size() <= roomAnts.size()) {
					// less crowded
					if ((*rit)->getArea() > 4) // Don't bother with small rooms
						cands.push_back(*rit);
				}
			}

			if (cands.empty())
				continue;

			if (roomAnts.size()==1)
				lookForFood(*roomAnts.begin());

			ITC(AntSet, ait, roomAnts) {
				Ant* a = *ait;
				if (a->state() == Ant::STATE_NONE) {
					//lookForFood(a);
					if (a->state() == Ant::STATE_NONE)
						a->goToRoom(cands[rand() % cands.size()]);
				}
			}
		}
	}

	// Kill enemy hills:
	ITC(AntSet, it, ants) {
		Ant* ant = *it;
		RoomContent* rc = g_map->roomContentAt(ant->pos());
		if (!rc->m_enemyHills.empty()) {
			// There is an enemy hill in this room - storm it berserk style!
			LOG_DEBUG("STORMING ANT HILL!");
			ant->goTo(*rc->m_enemyHills.begin());
		}
	}

	////////////////////////////////////////////////////////////
	// TEST CODE! Push all ants onto one food:
	/*if (food.size()>0) {
		LOG_DEBUG("TEST:   SENDING ALL ANTS TO ONE FOOD!!!!");
		ITC(PosSet, fit, food) {
			Pos p = *fit;
			bool anyWin=false;
			ITC(AntSet, ait, ants) {
				if ((*ait)->goToFoodAt(p))
					anyWin = true;
			}
			if (anyWin)
				break;
		}
	}
	*/
	////////////////////////////////////////////////////////////


	g_coordinator->moveAntsAfterDesire(ants); // all ants desires are calculated, so they should all have a plan/goal/mission/objective

#ifdef DEBUG
	// Draw map with desires and plans.
	DebugWindow::instance()->redraw();
#endif

	LOG_DEBUG("time taken: " << state.timer.getTime() << "ms" << endl);
}

//finishes the turn
void Bot::endTurn() {
    state.turn++;

	io.output() << "go" << endl;
}

bool Bot::safeLocation(const Pos &loc) {
	Square& sq = g_map->square(loc);
	return !(sq.isWater || sq.isFood);
}
