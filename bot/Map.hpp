#ifndef MAP_HPP
#define MAP_HPP
#include "Pos.hpp"
#include <map>
#include <vector>
#include "Square.hpp"
#include "AntStar.hpp"

class Ant;

class Map
{
	std::vector<std::vector<Square> > grid;
	std::map<Ant*, Pos> antpos;
	std::map<Pos, Ant*> posant;
	//resets all non-water squares to land and clears the bots ant vector
	void reset();
public:
	Map();
	int m_cols, m_rows;

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

	Square& square(Pos const& pos) { return grid[pos[0]][pos[1]]; }
	bool isOccupied(const Pos& loc);

	Vec2 size() const { return Vec2(m_cols, m_rows); }

	double distance(const Pos &loc1, const Pos &loc2);
};

extern Map* g_map;

#endif // MAP_HPP
