#include "Map.hpp"
#include "Ant.hpp"
#include "State.hpp"
#include "Identifier.hpp"
#include <cmath>
Map* g_map = NULL;

Map::Map()
{

}

void Map::initMap(int rows, int cols) {
	m_size = Vec2(cols, rows);
	m_grid = std::vector<std::vector<Square> >(m_size.x(),
											   std::vector<Square>(m_size.y(), Square()));
}

Path Map::getOptimalPathTo(const Pos &from, const Pos &to) {
	AntStar star = AntStar();
	return star.findPath(from, to);
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


//returns the new location from moving in a given direction with the edges wrapped
Pos Map::getLocation(const Pos &loc, int dir)
{
	ASSERT(0<=dir && dir<4);
	return Pos( (loc[0] + DIRECTIONS[dir][0] + m_size[0]) % m_size[0],
				(loc[1] + DIRECTIONS[dir][1] + m_size[1]) % m_size[1] );
}

Square& Map::square(Pos const& pos) {
	assertInMap(pos);
	return m_grid[pos[0]][pos[1]];
}
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

	int rows = g_state->rows;
	int cols = g_state->cols;

	AntSet const& ants = g_state->identifier->getLiveAnts();
	for(AntSet::const_iterator it = ants.begin(); it != ants.end(); ++it) {
		sLoc = (*it)->pos();
		locQueue.push(sLoc);

		std::vector<std::vector<bool> > visited(cols, std::vector<bool>(rows, 0));
		m_grid[sLoc[0]][sLoc[1]].isVisible = 1;
		visited[sLoc[0]][sLoc[1]] = 1;

		while(!locQueue.empty()) {
			cLoc = locQueue.front();
			locQueue.pop();

			for(int d=0; d<TDIRECTIONS; d++) {
				nLoc = getLocation(cLoc, d);

				if(!visited[nLoc[0]][nLoc[1]] && distance(sLoc, nLoc) <= g_state->viewradius) {
					m_grid[nLoc[0]][nLoc[1]].isVisible = 1;
					m_grid[nLoc[0]][nLoc[1]].seen = true;
					locQueue.push(nLoc);
				}
				visited[nLoc[0]][nLoc[1]] = 1;
			}
		}
	}
}

bool Map::isOccupied(const Pos& loc) {
	return square(loc).ant != -1;
}

/*
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
*/

//returns the euclidean distance between two locations with the edges wrapped
 double Map::distance(const Pos &loc1, const Pos &loc2)
{
	int d0 = std::abs(loc1[0]-loc2[0]),
		d1 = std::abs(loc1[1]-loc2[1]),
		dx = std::min(d0, m_size.x()-d0),
		dy = std::min(d1, m_size.y()-d1);
	return sqrt(dx*dx + dy*dy);
}
