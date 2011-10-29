#include "Map.hpp"
#include "Ant.hpp"
#include "State.hpp"
#include "Tracker.hpp"
#include "Room.hpp"
#include "RoomContents.hpp"
#include "Util.hpp"
#include "Logger.hpp"
#include <cmath>
#include <queue>

Map* g_map = NULL;

Map::Map() {

}

void Map::initMap(Vec2 const& dim) {

	m_size = dim;
	LOG_DEBUG("Map size: " << m_size);
	m_grid = std::vector<std::vector<Square> >(m_size.x(),
											   std::vector<Square>(m_size.y(), Square()));
}

PosPath Map::getOptimalPathTo(const Pos &from, const Pos &to) {
	AntStar star = AntStar();
	return star.findPath(from, to);
}

void Map::removeAnt(Ant* ant) {
	LOG_DEBUG("removeAnt(" << *ant << ") from pos " << ant->pos());

	Square& sq = square(ant->pos());
	sq.antTeam = NO_TEAM;
	ASSERT(sq.pAnt == ant);
	sq.pAnt = 0;
	sq.room->contents()->removeMyrmidon(ant);
}

// Tell the map that an ant has spawned.
void Map::addAnt(Ant* ant) {
	LOG_DEBUG("addAnt(" << *ant << ") to pos " << ant->pos());

	Square& sq = square(ant->pos());
	ASSERT(sq.pAnt == NULL);
	sq.antTeam = 0;
	sq.pAnt = ant;
	sq.room->contents()->addMyrmidon(ant);
}

// Get the ant that occupies a specific position.
Ant* Map::getAntAt(Pos const& pos) {
	return square(pos).pAnt;
}

void Map::moveAnt(Pos const& from, Pos const& to) {
	LOG_TRACKER("Map::moveAnt from: " << from << ", to: " << to);
	Ant* ant = getAntAt(from);

	if (ant) {
		removeAnt(ant);
		ant->pos() = to;
		addAnt(ant);
		LOG_TRACKER(ant->pos());
	}
	else {
		LOG_ERROR("Map::moveAnt failed to find ant!");
	}
}

void Map::enemyHill(Pos const& pos, int team) {
	square(pos).room->contents()->enemyHillDiscovered(pos, team);
}

void Map::addHill(Pos const& pos) {
	square(pos).room->contents()->myrmidonHillDiscovered(pos);
}

void Map::addWater(const Pos &pos) {
	square(pos).isWater = true;
}

void Map::addFood(Pos const& pos) {
	//STAMP("Begin");
	square(pos).isFood = true;
	ASSERT(square(pos).discovered);
	ASSERT(square(pos).room);
	ASSERT(square(pos).room->contents());
	square(pos).room->contents()->insertFoodAt(pos);
	//STAMP("End");
}

void Map::addEnemyHill(EnemyHill const& hill) {
	square(hill.pos).hillTeam = hill.team;
}

void Map::addEnemyAnt(EnemyAnt const& ant) {
	Square& s = square(ant.pos);
	ASSERT(s.antTeam<0);
	s.antTeam = ant.team;
}

//returns the new location from moving in a given direction with the edges wrapped
Pos Map::getLocation(const Pos &loc, int dir)
{
	ASSERT(0<=dir && dir<=4); // 4==STAY is ok.
	return Pos( (loc[0] + DIRECTIONS[dir][0] + m_size[0]) % m_size[0],
				(loc[1] + DIRECTIONS[dir][1] + m_size[1]) % m_size[1] );
}

Square& Map::square(Pos const& pos) {
	assertInMap(pos);
	return m_grid[pos[0]][pos[1]];
}


Room* Map::roomAt(const Pos& pos) {
	return square(pos).room;
}

RoomContents* Map::roomContentAt(const Pos& pos) {
	Room* r = roomAt(pos);
	ASSERT(r);
	return r->contents();
}

/*
	This function will update update the lastSeen value for any squares currently
	visible by one of your live ants.

	BE VERY CAREFUL IF YOU ARE GOING TO TRY AND MAKE THIS FUNCTION MORE EFFICIENT,
	THE OBVIOUS WAY OF TRYING TO IMPROVE IT BREAKS USING THE EUCLIDEAN METRIC, FOR
	A CORRECT MORE EFFICIENT IMPLEMENTATION, TAKE A LOOK AT THE GET_VISION FUNCTION
	IN ANTS.PY ON THE CONTESTS GITHUB PAGE.
*/
void Map::updateVisionInformation(const PosList& antsPos) {
	std::queue<Pos> locQueue;
	Pos sLoc, cLoc, nLoc;

	PosSet discoveries;

	LOG_DEBUG("updateVisionInformation has " << antsPos.size() << " ants");

	ITC(PosList, pit, antsPos) {
		sLoc = *pit;
		locQueue.push(sLoc);

		std::vector<std::vector<bool> > visited(size().x(), std::vector<bool>(size().y(), 0));
		m_grid[sLoc[0]][sLoc[1]].markAsVisible();
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

				if (!visited[nLoc[0]][nLoc[1]] && euclidDist(sLoc, nLoc) <= g_state->viewradius) {
					m_grid[nLoc[0]][nLoc[1]].markAsVisible();
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

	LOG_DEBUG(discoveries.size() << " new discoveries.");

	g_rooms->expandWith(discoveries);
}

bool Map::isOccupied(const Pos& loc) {
	return square(loc).antTeam >= 0;
}

//resets all non-water squares to land
void Map::resetDynamics()
{
	STAMP("Map::resetDynamics");
	for (int x=0; x<size().x(); ++x)
		for (int y=0; y<size().y(); ++y)
			if (!m_grid[x][y].isWater)
				m_grid[x][y].resetDynamics();
}

//returns the euclidean distance between two locations with the edges wrapped
float Map::euclidDist(const Pos& a, const Pos& b) const {
	return sqrtf( (float)euclidDistSq(a, b) );
}

int Map::euclidDistSq(const Pos& a, const Pos& b) const {
	return
			sqr(wrappedAbsDist(a[0], b[0], m_size[0])) +
			sqr(wrappedAbsDist(a[1], b[1], m_size[1]));
}

int Map::manhattanDist(Pos a, Pos b) const {
	return wrappedAbsDist(a[0], b[0], m_size[0]) + wrappedAbsDist(a[1], b[1], m_size[1]);
}

// Wrapped.
Vec2 Map::difference(Pos a, Pos b) const {
	return Vec2(
				wrappedSignedDist(a[0], b[0], m_size[0]),
				wrappedSignedDist(a[1], b[1], m_size[1])
				);
}

Pos Map::wrapPos(Pos a) const {
	for (int i=0; i<2; ++i)
		a[i] = wrap(a[i], m_size[i]);
	return a;
}
