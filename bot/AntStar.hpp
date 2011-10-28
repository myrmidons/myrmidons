#ifndef ANTSTAR_HPP
#define ANTSTAR_HPP
#include <vector>
#include <deque>
#include "Pos.hpp"


class Map;

struct StarAnt {
	StarAnt* parent; // referens till föräldranoden, används för att härleda slutvägen
	bool visited; // har grannnoden besökts?
	bool closed; // ej i så kallade öppnade listan för A*
	bool passable; // vatten/mark?
	double f; // f(n) = g(n) + h(n), på vägen mot målet bör denna variabel vara identisk hela vägen (iom att g blir större och f blir mindre)
	double g; // g(n) antal steg sedan start (enkel räknare)
	double h; // h(n) hierustisk beräkning för nod
	Pos p;

	StarAnt(int x, int y)
	{
		parent = NULL;
		p = Pos(x,y);
	}

	friend bool operator<(const StarAnt& a, const StarAnt& b) {
		return a.f < b.f;
	}
	friend bool operator==(const StarAnt& a, const StarAnt& b) {
		return a.p == b.p;
	}
};

typedef std::vector<std::vector<StarAnt> > StarGrid;
typedef std::deque<Pos> Path;

class AntStar
{
	StarGrid m_grid;
	std::set<StarAnt> m_openList;
	int getMovementCost();
	double heuristics(StarAnt* current, StarAnt* goal);
public:
    AntStar();
	Path findPath(Pos from, Pos to);
};



#endif // ANTSTAR_HPP
