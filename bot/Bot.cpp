#include "Bot.hpp"
#include "Ant.hpp"
#include "Map.hpp"
#include "Logger.hpp"
#include "Tracker.hpp"
#include "Room.hpp"
#include "RoomContents.hpp"
#include "Util.hpp"
#include "Coordinator.hpp"
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

int Bot::closestLocation(const Pos& loc, const vector<Pos>& location) {
	int result = -1;
	float minDist = 10000000;
	for(size_t i = 0; i < location.size(); ++i) {
		float dist = g_map->euclidDist(location[i], loc);
		if(dist < minDist) {
			minDist = dist;
			result = (int)i;
		}
	}

	return result;
}

void lookForFood(Ant* ant) {
	// Try find food in room:
	Vec2 pos = ant->pos();
	RoomContents* rc = g_map->roomContentAt(pos);
	const PosSet& food = rc->m_food;

	if (!food.empty()) {
		// Go to random food:
		LOG_DEBUG("Going to food");
		PosSet::const_iterator it = food.begin();
		advance(it, rand()%food.size());
		ant->goToFoodAt(*it);
	}
}

//makes the bots moves for the turn
void Bot::makeMoves()
{
	LOG_DEBUG("Bot::makeMoves");

	// Update current ant states
	AntSet& ants = g_tracker->getAnts();
	IT(AntSet, it, ants)
		(*it)->updateState();

	// Distribute food to close ants
	const PosSet& food = g_tracker->getFood();
	ITC(PosSet, pit, food) {
		// Find closest ant to this
		Pos foodPos = *pit;
		if (g_map->square(foodPos).destinyAnt)
			continue; // Someone is already heading for this food.

		LOG_DEBUG("Looking for ant close to food...");

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
	}


	// Find crowded rooms, send ants out of them...
	ITC(RoomList, rit, g_rooms->rooms()) {
		Room* room = *rit;
		RoomContents* rc = room->contents();
		const AntSet& roomAnts = rc->ants();

		const RoomSet& neighRooms = room->neighborRooms();

		if (!roomAnts.empty() && !neighRooms.empty()) {
			/* TODO: base on the number of food expected to be found, related to:
			foodExp = room->area() * room->timeSinceLastVisit();
			 */
			RoomList cands;
			ITC(RoomSet, rit, neighRooms) {
				if ((*rit)->contents()->ants().size() <= roomAnts.size()) {
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

	ITC(AntSet, it, ants) {
		Ant* ant = *it;
		if (ant->state() == Ant::STATE_NONE)
			lookForFood(ant);
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
