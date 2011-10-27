#ifndef SQUARE_HPP
#define SQUARE_HPP

#include <vector>

class Room;

/*
    struct for representing a square in the grid.
*/
struct Square
{
	bool seen,isVisible, isWater, isHill, isFood;
    int ant, hillPlayer;
    std::vector<int> deadAnts;
	Room* room;

	Square()
	{
		seen = isVisible = isWater = isHill = isFood = 0;
        ant = hillPlayer = -1;
	};

    //resets the information for the square except water information
    void reset()
    {
		room = NULL;
        isVisible = 0;
        isHill = 0;
        isFood = 0;
        ant = hillPlayer = -1;
        deadAnts.clear();
	}

	// Are we sure this is ground?
	bool isGround() const {
		return isVisible && !isWater;
	}
};

#endif //SQUARE_H_
