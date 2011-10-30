#ifndef SQUARE_HPP
#define SQUARE_HPP

#include <vector>

class Room;
class RoomContent;
class Ant;

const int NO_TEAM  = -1;
const int OUR_TEAM = 0;

/* One grid cell on the map.
It's hip to be...              */
struct Square
{
	// Static (once discovered, does not change).
	bool discovered; // Been seen at least once.
	bool isWater, isHill;
	Room* room;

	// Dynamic (may change):
	bool m_isVisible; // Being seen right now
	int m_lastVisible; // Which turn was we last visible?
	bool isFood;

	Ant* pAnt;
	int antTeam;  // The ant at this position is team...
	int hillTeam; // the hill at this position is team...
	std::vector<int> deadAnts;
	Ant* destinyAnt; // Ant heading here (may still be a long way away though).
	bool hillAlive; // if false, the hill here has been killed.

	Square() : m_lastVisible(-1)
	{
		discovered = isWater = isHill = isFood = false;
		pAnt = destinyAnt = NULL;
		room = NULL;
		antTeam = hillTeam = NO_TEAM;
		resetDynamics();
		hillAlive = true;
	}

	// reset dynamic information
	void resetDynamics()
	{
		m_isVisible = false;
		isFood = false;
		antTeam = NO_TEAM; // Ants can move. Tracker will set each turn.
		deadAnts.clear();
	}

	// Are we sure this is ground?
	bool isGround() const {
		return discovered && !isWater;
	}

	bool visible() const { return m_isVisible; }

	// For easy access
	RoomContent* roomContent();

	void markAsVisible();
};

#endif //SQUARE_H_
