#ifndef MAP_HPP
#define MAP_HPP
#include "Pos.hpp"
#include <map>
#include <vector>
#include "Square.hpp"
#include "AntStar.hpp"
#include "Assert.hpp"

class Ant;

class Map
{
public:
	Map();
	void initMap(int rows, int cols);

	void removeAnt(Ant* ant);
	void addAnt(Ant* ant);
	Ant* getAnt(Pos const& pos);
	void updateVisionInformation();

	// Not sure if this is up to the Map-class in the end, but probably.
	void moveAnt(Pos const& from, Pos const& to); // Not used by the AntIdentifyer, just by Bot for testing purposes.

	// Take one (wrapped) step into one of four directions.
	Pos getLocation(const Pos &loc, int direction);

	Path getOptimalPathTo(const Pos &from, const Pos &to);

	Square& square(Pos const& pos);

	bool isOccupied(const Pos& loc);

	const Vec2& size() const { return m_size; }

	double distance(const Pos &loc1, const Pos &loc2);

	void assertInMap(const Pos& pos) const {
		ASSERT(0<=pos.x() && pos.x()<m_size.x());
		ASSERT(0<=pos.y() && pos.y()<m_size.y());
	}

private:
	std::vector<std::vector<Square> > m_grid; // x/y
	std::map<Ant*, Pos> antpos;
	std::map<Pos, Ant*> posant;
	Vec2 m_size;

	//resets all non-water squares to land and clears the bots ant vector
	//void reset();
};

extern Map* g_map;

#endif // MAP_HPP
