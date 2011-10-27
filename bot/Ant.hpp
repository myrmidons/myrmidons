#ifndef ANT_H
#define ANT_H

#include "State.hpp"
#include "Pos.hpp"
#include "Map.hpp"
#include <map>
#include <set>

class Food;
class Ant;

typedef std::set<size_t> IndexSet;
typedef std::set<Pos> LocSet, PosSet;
typedef std::set<Ant*> AntSet;
typedef std::vector<Ant> AntVec;
typedef std::vector<Pos> PosVec;

class Ant
{
public:
	Ant(Pos const& loc = Pos());
	Ant(Ant const& ant)
		: m_position(ant.m_position){

	}
	Ant& operator=(Ant const& ant) {
		m_position = ant.m_position;
		return *this;
	}

	Pos& pos();

private:
	Pos m_position;
};


struct AntIdentifier {


	size_t m_numAnts;
	AntVec m_antStorage;
	AntSet m_liveAnts;
	IndexSet m_deadIndices;

public:

	AntIdentifier();

	void update(State& state);

	inline size_t indexOf(Ant* ant) { return (ant - &m_antStorage[0])/sizeof(Ant*); }
	Map* m_map;
};



#endif // ANT_H
