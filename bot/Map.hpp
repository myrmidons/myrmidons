#ifndef MAP_HPP
#define MAP_HPP
#include "Pos.hpp"
#include <map>
#include <vector>
#include "Square.hpp"
class Ant;
class Map
{
	std::vector<std::vector<Square> > grid;
	std::map<Ant*, Pos> antpos;
	std::map<Pos, Ant*> posant;

	int rows;
	int cols;

	//resets all non-water squares to land and clears the bots ant vector
	void reset();
public:
	Map();
	void initMap(int rows, int cols);
	void removeAnt(Ant* ant);
	void addAnt(Ant* ant);
	Ant* getAnt(Pos const& pos);
	//void updateVisionInformation();

	// Not sure if this is up to the Map-class in the end, but probably.
	void moveAnt(Pos const& from, Pos const& to); // Not used by the AntIdentifyer, just by Bot for testing purposes.

	// Take one (wrapped) step into one of four directions.
	Pos getLocation(const Pos &loc, int direction);

	Square& square(Pos const& pos) { return grid[pos[0]][pos[1]]; }
	bool isOccupied(const Pos& loc);

};

extern Map* g_map;

#endif // MAP_HPP