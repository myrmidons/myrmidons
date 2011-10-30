#ifndef MAP_HPP
#define MAP_HPP
#include "Pos.hpp"
#include <map>
#include <vector>
#include "Square.hpp"
#include "AntStar.hpp"
#include "Assert.hpp"
#include "Ant.hpp"

class RoomContent;

class Map
{
public:
	Map();
	void initMap(Vec2 const& dim);

	void addHill(Pos const& pos, int team);

	// Friendly ants
	void removeAnt(Ant* ant);
	void addAnt(Ant* ant);
	Ant* getAntAt(Pos const& pos);

	// Enemy ants
	void addEnemyAnt(EnemyAnt const& ant);

	// Terrain
	void addWater(Pos const& pos);

	// Food
	void addFood(Pos const& pos);

	//resets all non-water squares to land
	void resetDynamics();

	void updateVisionInformation(const PosList& myAnts);


	// Take one (wrapped) step into one of four directions.
	Pos getLocation(const Pos &loc, int direction);

	//PosPath getOptimalPathTo(const Pos &from, const Pos &to);

	Square& square(Pos const& pos);
	Room* roomAt(const Pos& pos);
	RoomContent* roomContentAt(const Pos& pos);

	bool isOccupied(const Pos& loc);

	const Vec2& size() const { return m_size; }

	void assertInMap(const Pos& pos) const {
		ASSERT(0<=pos.x() && pos.x()<m_size.x());
		ASSERT(0<=pos.y() && pos.y()<m_size.y());
	}

	// usefull for distances etc:
	int manhattanDist(Pos a, Pos b) const;
	float euclidDist(const Pos& a, const Pos& b) const;
	int euclidDistSq(const Pos& a, const Pos& b) const;

	// Wrapped.
	Vec2 difference(Pos a, Pos b) const;

	Pos wrapPos(Pos a) const;

private:
	std::vector<std::vector<Square> > m_grid; // x/y
	Vec2 m_size;

};

extern Map* g_map;

#endif // MAP_HPP
