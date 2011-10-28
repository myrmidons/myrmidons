#include "Map.hpp"
#include "Ant.hpp"
#include "State.hpp"
#include "Tracker.hpp"
#include "Room.hpp"
#include <cmath>

Map* g_map = NULL;

Map::Map() {

}

void Map::initMap(Vec2 const& dim) {

	m_size = dim;
	g_state->bug << "Map size: " << m_size << std::endl;
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

		Square& sq = square(ant->pos());
		sq.ant = -1;
		sq.pAnt = 0;
		sq.room->contents()->removeMyrmidon(ant);
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

		Square& sq = square(ant->pos());
		sq.ant = 0;
		sq.pAnt = ant;
		sq.room->contents()->addMyrmidon(ant);

	}
}

// Get the ant that occupies a specific position.
Ant* Map::getAnt(Pos const& pos) {
	//return square(pos).pAnt;
	if(posant.count(pos)) {
		return posant[pos];
	}
	return 0;
}

void Map::moveAnt(Pos const& from, Pos const& to) {
	g_tracker->log << "Looking for ant at " << from << ": ";
	Ant* ant = getAnt(from);

	if(ant) {
		g_tracker->log << "and moving it from " <<  ant->pos() << " to ";
		removeAnt(ant);
		ant->pos() = to;
		addAnt(ant);
		g_tracker->log << ant->pos() << std::endl;

		square(to).ant = square(from).ant;
		square(from).ant = -1;

	/*	Square& sqFrom = square(from);
		sqFrom.ant = -1;
		sqFrom.pAnt = 0;
		sqFrom.room->contents()->removeMyrmidon(ant);

		Square& sqTo = square(to);
		sqTo.ant = 0;
		sqTo.pAnt = ant;
		sqTo.room->contents()->addMyrmidon(ant);
		*/
	}
	else {
		g_tracker->log << " without finding it." << std::endl;
	}
}

void Map::enemyHill(Pos const& pos, int team) {
	square(pos).room->contents()->enemyHillDiscovered(pos, team);
}

void Map::hill(Pos const& pos) {
	square(pos).room->contents()->myrmidonHillDiscovered(pos);
}

void Map::water(const Pos &pos) {
	square(pos).isWater = true;
}

void Map::food(Pos const& pos) {
	square(pos).isFood = true;
	square(pos).room->contents()->foodAt(pos);
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

	PosSet discoveries;

	AntSet const& ants = g_tracker->getLiveAnts();
	for(AntSet::const_iterator it = ants.begin(); it != ants.end(); ++it) {

		square(sLoc).ant = 0;
		sLoc = (*it)->pos();
		locQueue.push(sLoc);

		std::vector<std::vector<bool> > visited(size().x(), std::vector<bool>(size().y(), 0));
		m_grid[sLoc[0]][sLoc[1]].isVisible = 1;
		visited[sLoc[0]][sLoc[1]] = 1;

		if (!m_grid[sLoc[0]][sLoc[1]].discovered) {
			m_grid[sLoc[0]][sLoc[1]].discovered = true;
			discoveries.insert(sLoc);
		}

		while(!locQueue.empty()) {
			cLoc = locQueue.front();
			locQueue.pop();

			for(int d=0; d<TDIRECTIONS; d++) {
				nLoc = getLocation(cLoc, d);

				if(!visited[nLoc[0]][nLoc[1]] && distance(sLoc, nLoc) <= g_state->viewradius) {
					m_grid[nLoc[0]][nLoc[1]].isVisible = 1;
					if(!m_grid[nLoc[0]][nLoc[1]].discovered) {
						m_grid[nLoc[0]][nLoc[1]].discovered = true;
						discoveries.insert(nLoc);

					}

					locQueue.push(nLoc);
				}
				visited[nLoc[0]][nLoc[1]] = 1;
			}
		}
	}

	g_state->bug << discoveries.size() << " new discoveries." << std::endl;

	g_rooms->expandWith(discoveries);
}

bool Map::isOccupied(const Pos& loc) {
	return square(loc).ant != -1;
}


void Map::newTurn(int turn) {
	reset();
}


//resets all non-water squares to land and clears the bots ant vector
void Map::reset()
{
	for (int x=0; x<size().x(); ++x)
		for (int y=0; y<size().y(); ++y)
			if(!m_grid[x][y].isWater)
				m_grid[x][y].reset();
}


//returns the euclidean distance between two locations with the edges wrapped
double Map::distance(const Pos &loc1, const Pos &loc2)
{
	int d0 = std::abs(loc1[0]-loc2[0]);
	int d1 = std::abs(loc1[1]-loc2[1]);
	int dx = std::min(d0, m_size.x()-d0);
	int dy = std::min(d1, m_size.y()-d1);
	return sqrt(dx*dx + dy*dy);
}
