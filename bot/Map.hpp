#ifndef MAP_HPP
#define MAP_HPP
#include "Pos.hpp"
#include <map>
#include <vector>
#include "Square.hpp"
#include "AntStar.hpp"
#include "Assert.hpp"

class Ant;
class RoomContents;

class Map
{
public:
	Map();
	void initMap(Vec2 const& dim);

	// Friendly ants
	void hill(Pos const& pos);
	void removeAnt(Ant* ant);
	void addAnt(Ant* ant);
	Ant* getAnt(Pos const& pos);

	// Terrain
	void water(Pos const& pos);

	// Food
	void food(Pos const& pos);

	// Enemy ants
	void enemyHill(Pos const& pos, int team);
	void enemyAnt(Pos const& pos, int team);

	void updateVisionInformation();

	// Not sure if this is up to the Map-class in the end, but probably.
	void moveAnt(Pos const& from, Pos const& to); // Not used by the AntIdentifyer, just by Bot for testing purposes.

	// Take one (wrapped) step into one of four directions.
	Pos getLocation(const Pos &loc, int direction);

	PosPath getOptimalPathTo(const Pos &from, const Pos &to);

	Square& square(Pos const& pos);
	Room* roomAt(const Pos& pos);
	RoomContents* roomContentAt(const Pos& pos);

	bool isOccupied(const Pos& loc);

	const Vec2& size() const { return m_size; }

	void newTurn(int turn);

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

private:
	std::vector<std::vector<Square> > m_grid; // x/y
	std::map<Ant*, Pos> antpos;
	std::map<Pos, Ant*> posant;
	Vec2 m_size;

	//resets all non-water squares to land and clears the bots ant vector
	void reset();

};

extern Map* g_map;

#endif // MAP_HPP
