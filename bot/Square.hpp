#ifndef SQUARE_HPP
#define SQUARE_HPP

#include <vector>

class Room;
class Ant;

/*
    struct for representing a square in the grid.
*/
struct Square
{
	bool discovered; // Been seen at least once.
	bool isVisible; // Being seen right now
	bool isWater, isHill, isFood;

	int ant, hillPlayer; // What is this? team?
	Ant* pAnt;
	std::vector<int> deadAnts;
	Room* room;
	Ant* destinyAnt; // Ant heading here.

	Square()
	{
		discovered = isVisible = isWater = isHill = isFood = false;
		ant = hillPlayer = -1;
		destinyAnt = NULL;
		room = NULL;
	}

	// reset dynamic information
	void resetDynamics()
	{
		isVisible = 0;
		isHill = 0;
		isFood = 0;
		//    ant = hillPlayer = -1;
		deadAnts.clear();
	}

	// Are we sure this is ground?
		bool isGround() const {
		return discovered && !isWater;
	}
};

#endif //SQUARE_H_
