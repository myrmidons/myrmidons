#ifndef ANTSTAR_HPP
#define ANTSTAR_HPP
#include <vector>
#include <deque>
#include "Pos.hpp"


class Map;

struct StarAnt {
	StarAnt* parent;
	bool visited;
	bool closed;
	bool passable;
	float f;
	float g;
	float h;
	int id;
	int x;
	int y;

	StarAnt()
	{
		parent = NULL;
	}

	friend bool operator<(const StarAnt& a, const StarAnt& b) {
		return a.f < b.f;
	}
	friend bool operator==(const StarAnt& a, const StarAnt& b) {
		return a.x == b.x && a.y == b.y;
	}
};

typedef std::vector<std::vector<StarAnt> > StarGrid;
typedef std::deque<Pos> Path;

class AntStar
{
	StarGrid m_grid;
	std::set<StarAnt> m_openList;
public:
    AntStar();
	Path findPath(Map* map, Pos from, Pos to);
};



#endif // ANTSTAR_HPP
