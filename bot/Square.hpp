#ifndef SQUARE_HPP
#define SQUARE_HPP

#include <vector>

class Room;
class Ant;

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

	int ant, hillPlayer; // What is this? team?
	Ant* pAnt;
	std::vector<int> deadAnts;
	Ant* destinyAnt; // Ant heading here (may still be a long way away though).

	Square() : m_lastVisible(-1)
	{
		discovered = isWater = isHill = isFood = false;
		ant = hillPlayer = -1;
		destinyAnt = NULL;
		room = NULL;
		resetDynamics();
	}

	// reset dynamic information
	void resetDynamics()
	{
		m_isVisible = false;
		isFood = false;
	//    ant = hillPlayer = -1;
		deadAnts.clear();
	}

	// Are we sure this is ground?
	bool isGround() const {
		return discovered && !isWater;
	}

	bool visible() const { return m_isVisible; }

	void markAsVisible();
};

#endif //SQUARE_H_
