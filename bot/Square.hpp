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
	bool discovered,isVisible, isWater, isHill, isFood;
    int ant, hillPlayer;
	Ant* pAnt;
    std::vector<int> deadAnts;
	Room* room;

	Square()
	{
		discovered = isVisible = isWater = isHill = isFood = false;
        ant = hillPlayer = -1;
		room = NULL;
	}

	// reset dynamic information
    void reset()
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
