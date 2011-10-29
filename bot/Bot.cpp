#include "Bot.hpp"
#include "Ant.hpp"
#include "Map.hpp"
#include "Tracker.hpp"
#include "Room.hpp"
#include "RoomContents.hpp"
#include "Util.hpp"

#include <algorithm>
#include <iostream>

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

int Bot::rankMove(Pos const& currentLoc, int dir) {
	// The new location to try to get to.
	Pos newLoc = g_map->getLocation(currentLoc, dir);

	int rank = 0;


	if(!safeLocation(newLoc) || g_map->isOccupied(newLoc))
		return -1000; // Absolutley not, it would be suicide!

/*
	for(int j = 0; j < (int)state.enemyAnts.size(); ++j) {
		double d = g_map->distance(newLoc,state.enemyAnts[j]);
		if(int(d*d + 0.5) < 13)
			rank -= 20;
	}

	int foodLocationIndex = closestLocation(currentLoc, state.food);
	if(foodLocationIndex >= 0) {
			Pos foodLoc = state.food[foodLocationIndex];
			if(g_map->distance(newLoc, foodLoc) >= g_map->distance(currentLoc, foodLoc)) {
				//rank -= 10; // this move would take us further away from the nearest food item.
			}
			else {
				rank += 10; // This move would take us closer to the nearest food item.
			}
	}*/


	return rank;
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
	const AntSet& ants = g_tracker->getLiveAnts();
	ITC(AntSet, it, ants)
		(*it)->updateState();

	// Distribute food to close ants
	const PosList& food = g_tracker->getAllFood();
	ITC(PosList, pit, food) {
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
			LOG_DEBUG("sending and to food.");
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

	STAMP_;

	ITC(AntSet, it, ants) {
		LOG_DEBUG("Deciding ant move...");

		Ant* ant = *it;
		Pos pos = ant->pos();

		if (ant->state() == Ant::STATE_NONE)
			lookForFood(ant);

		ant->calcDesire();
		PosList desire = ant->getDesire();

		if (desire.empty() || ant->state() == Ant::STATE_NONE) {
			// Random walk
			DirVec const& dirs = randomDirVec();

			int bestMove = 0, bestRank = -10000000;
			for (int i = 0; i<TDIRECTIONS; i++) {
				int d = dirs[i];
				int rank = rankMove(pos, d);
				if(rank > bestRank) {
					bestRank = rank;
					bestMove = d;
				}
			}
			if (bestRank > -100) {
				LOG_DEBUG("Random move");
				// This will not be needed. Just for testing the identifyer as things stand at the moment.
				g_map->moveAnt(pos, g_map->getLocation(pos, bestMove));
				state.makeMove(pos, bestMove); // Needed because the map is still just a dummy.
			}
		} else {
			// Follow desire.
			Pos dest = desire.front();
			LOG_DEBUG("Desire to go from " << pos << " to " << dest);
			Vec2 d = g_map->difference(pos, dest);
			int dir = -1;
			if (d.x()<0)      dir = WEST;
			else if (d.x()>0) dir = EAST;
			else if (d.y()<0) dir = NORTH;
			else if (d.y()>0) dir = SOUTH;
			if (dir != -1) {
				LOG_DEBUG("Following desire");
				g_map->moveAnt(pos, dest);
				state.makeMove(pos, dir);
			}
		}
	}

#ifdef DEBUG
	DebugWindow::instance()->redraw();
#endif

    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
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
