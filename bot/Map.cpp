#include "Map.hpp"
#include "Ant.hpp"
#include "State.hpp"
#include "Identifier.hpp"
#include "Room.hpp"
#include <cmath>

Map* g_map = NULL;

Map::Map()
{

}

void Map::initMap(int rows, int cols) {
	m_cols = cols;
	m_rows = rows;
	grid = std::vector<std::vector<Square> >(m_rows, std::vector<Square>(m_cols, Square()));
}

void Map::removeAnt(Ant* ant) {
	if(!antpos.count(ant)) {
		// FAIL!
		g_state->bug << "FAIL in Map::removeAnt: Ant already removed! (or never added)" << std::endl;
	}
	else if(!posant.count(ant->pos())) {
		// FAIL!
		g_state->bug << "FAIL in Map::removeAnt: Ant postion already removed! (or never added)" << std::endl;
	}
	else {
		posant.erase(ant->pos());
		antpos.erase(ant);
	}
}

// Tell the map that an ant has spawned.
void Map::addAnt(Ant* ant) {
	if(antpos.count(ant)) {
		// FAIL!
		g_state->bug << "FAIL in Map::addAnt: Ant already added!" << std::endl;
	}
	else if(posant.count(ant->pos())) {
		// FAIL!
		g_state->bug << "FAIL in Map::addAnt: Ant position already added!" << std::endl;
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
//		g_state->bug << "In Map::getAnt: Ant does not exist!" << std::endl;
	return 0;
}

void Map::moveAnt(Pos const& from, Pos const& to) {
	g_state->bug << "Looking for ant at " << from << ": ";
	Ant* ant = getAnt(from);
	if(ant) {
		g_state->bug << "and moving it from " <<  ant->pos() << " to ";
		removeAnt(ant);
		ant->pos() = to;
		addAnt(ant);
		g_state->bug << ant->pos() << std::endl;
	}
	else {
		g_state->bug << " without finding it." << std::endl;
	}
}

void Map::water(const Pos &pos) {
	grid[pos[0]][pos[1]].isWater = 1;
}

//returns the new location from moving in a given direction with the edges wrapped
Pos Map::getLocation(const Pos &loc, int direction)
{
	return Pos( (loc[0] + DIRECTIONS[direction][0] + m_rows) % m_rows,
				(loc[1] + DIRECTIONS[direction][1] + m_cols) % m_cols );
};
/*
	This function will update update the lastSeen value for any squares currently
	visible by one of your live ants.

	BE VERY CAREFUL IF YOU ARE GOING TO TRY AND MAKE THIS FUNCTION MORE EFFICIENT,
	THE OBVIOUS WAY OF TRYING TO IMPROVE IT BREAKS USING THE EUCLIDEAN METRIC, FOR
	A CORRECT MORE EFFICIENT IMPLEMENTATION, TAKE A LOOK AT THE GET_VISION FUNCTION
	IN ANTS.PY ON THE CONTESTS GITHUB PAGE.
*/


void Map::updateVisionInformation() {
	std::queue<Pos> locQueue;
	Pos sLoc, cLoc, nLoc;

	PosSet discoveries;

	int rows = g_state->rows;
	int cols = g_state->cols;

	AntSet const& ants = g_state->identifier->getLiveAnts();
	for(AntSet::const_iterator it = ants.begin(); it != ants.end(); ++it) {
		sLoc = (*it)->pos();
		locQueue.push(sLoc);

		std::vector<std::vector<bool> > visited(rows, std::vector<bool>(cols, 0));
		grid[sLoc[0]][sLoc[1]].isVisible = 1;
		visited[sLoc[0]][sLoc[1]] = 1;

		while(!locQueue.empty()) {
			cLoc = locQueue.front();
			locQueue.pop();

			for(int d=0; d<TDIRECTIONS; d++) {
				nLoc = getLocation(cLoc, d);

				if(!visited[nLoc[0]][nLoc[1]] && distance(sLoc, nLoc) <= g_state->viewradius) {
					grid[nLoc[0]][nLoc[1]].isVisible = 1;
					if(!grid[nLoc[0]][nLoc[1]].discovered) {
						grid[nLoc[0]][nLoc[1]].discovered = true;
						discoveries.insert(nLoc);

					}
					locQueue.push(nLoc);
				}
				visited[nLoc[0]][nLoc[1]] = 1;
			}
		}
	}

	g_rooms->expandWith(discoveries);
}

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
	for(int row=0; row<m_rows; row++)
		for(int col=0; col<m_cols; col++)
			if(!grid[row][col].isWater)
				grid[row][col].reset();
}

//returns the euclidean distance between two locations with the edges wrapped
 double Map::distance(const Pos &loc1, const Pos &loc2)
{
	int d1 = std::abs(loc1[0]-loc2[0]),
		d2 = std::abs(loc1[1]-loc2[1]),
		dr = std::min(d1, m_rows-d1),
		dc = std::min(d2, m_cols-d2);
	return sqrt(dr*dr + dc*dc);
}

