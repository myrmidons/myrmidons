#include "Map.hpp"
#include "Ant.hpp"

Map* g_map = NULL;

Map::Map()
{

}

void Map::initMap(int numOfRows, int numOfCols) {
	rows = numOfRows;
	cols = numOfCols;
	grid = std::vector<std::vector<Square> >(rows, std::vector<Square>(cols, Square()));
}

// Tell the map that a certain ant is dead.
// the map don't own the ant, and should be prepared for
// getting the same pointer as a newly spawned ant.
void Map::removeAnt(Ant* ant) {
	if(antpos.count(ant)) {
		posant.erase(antpos[ant]);
		antpos.erase(ant);
	}
	else {
		// FAIL!
	}
}

// Tell the map that an ant has spawned.
void Map::addAnt(Ant* ant) {
	if(antpos.count(ant)) {
		// FAIL!
	}
	else {
		antpos[ant] = ant->pos();
		posant[ant->pos()] = ant;
	}
}

// Get the ant that occupies a specific position.
Ant* Map::getAnt(Pos const& pos) {
	if(posant.count(pos)) {
		return posant[pos];
	}
	return 0;
}
//returns the new location from moving in a given direction with the edges wrapped
Pos Map::getLocation(const Pos &loc, int direction)
{
	return Pos( (loc[0] + DIRECTIONS[direction][0] + rows) % rows,
					 (loc[1] + DIRECTIONS[direction][1] + cols) % cols );
};
/*
	This function will update update the lastSeen value for any squares currently
	visible by one of your live ants.

	BE VERY CAREFUL IF YOU ARE GOING TO TRY AND MAKE THIS FUNCTION MORE EFFICIENT,
	THE OBVIOUS WAY OF TRYING TO IMPROVE IT BREAKS USING THE EUCLIDEAN METRIC, FOR
	A CORRECT MORE EFFICIENT IMPLEMENTATION, TAKE A LOOK AT THE GET_VISION FUNCTION
	IN ANTS.PY ON THE CONTESTS GITHUB PAGE.
*/ /*
void Map::updateVisionInformation()
{
	std::queue<Pos> locQueue;
	Pos sLoc, cLoc, nLoc;

	for(int a=0; a<(int) myAnts.size(); a++)
	{
		sLoc = myAnts[a];
		locQueue.push(sLoc);

		std::vector<std::vector<bool> > visited(rows, std::vector<bool>(cols, 0));
		grid[sLoc[0]][sLoc[1]].isVisible = 1;
		visited[sLoc[0]][sLoc[1]] = 1;

		while(!locQueue.empty())
		{
			cLoc = locQueue.front();
			locQueue.pop();

			for(int d=0; d<TDIRECTIONS; d++)
			{
				nLoc = getLocation(cLoc, d);

				if(!visited[nLoc[0]][nLoc[1]] && distance(sLoc, nLoc) <= viewradius)
				{
					grid[nLoc[0]][nLoc[1]].isVisible = 1;
					locQueue.push(nLoc);
				}
				visited[nLoc[0]][nLoc[1]] = 1;
			}
		}
	}
}; */

bool Map::isOccupied(const Pos& loc) {
	return grid[loc[0]][loc[1]].ant != -1;
}

//resets all non-water squares to land and clears the bots ant vector
void Map::reset()
{
	//myAnts.clear();
	//enemyAnts.clear();
	//myHills.clear();
	//enemyHills.clear();
	//food.clear();
	//deadAnts.clear();
	for(int row=0; row<rows; row++)
		for(int col=0; col<cols; col++)
			if(!grid[row][col].isWater)
				grid[row][col].reset();
};
