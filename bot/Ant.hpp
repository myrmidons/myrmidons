#ifndef ANT_HPP
#define ANT_HPP

#include "Pos.hpp"
#include <map>
#include <vector>

class Ant {
	Pos m_position;

public:
	Ant(Pos const& loc = Pos());
	Ant(Ant const& ant);
	Ant& operator=(Ant const& ant);

	Pos& pos();
};

typedef std::set<Ant*> AntSet;
typedef std::vector<Ant> AntVec;

#endif // ANT_HPP
