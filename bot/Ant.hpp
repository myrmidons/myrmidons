#ifndef ANT_HPP
#define ANT_HPP

#include "Pos.hpp"

class Ant {
	Pos m_position;

public:
	Ant(Pos const& loc = Pos());
	Ant(Ant const& ant);
	Ant& operator=(Ant const& ant);

	Pos& pos();
};

#endif // ANT_HPP
