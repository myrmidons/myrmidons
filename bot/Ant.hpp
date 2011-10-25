#ifndef ANT_H
#define ANT_H

#include "State.hpp"
#include "Pos.hpp"
#include <map>
#include <set>

class Food;
class Ant;

// This is a dummy for the future map-class to be created by goff and emilk.
struct Map;

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

// This is a dummy for the future map-class to be created by goff and emilk.
struct Map {
	std::map<Ant*, Pos> antpos;
	std::map<Pos, Ant*> posant;

	// Tell the map that a certain ant is dead.
	// the map don't own the ant, and should be prepared for
	// getting the same pointer as a newly spawned ant.
	void removeAnt(Ant* ant) {
		if(antpos.count(ant)) {
			posant.erase(antpos[ant]);
			antpos.erase(ant);
		}
		else {
			// FAIL!
		}
	}

	// Tell the map that an ant has spawned.
	void addAnt(Ant* ant) {
		if(antpos.count(ant)) {
			// FAIL!
		}
		else {
			antpos[ant] = ant->pos();
			posant[ant->pos()] = ant;
		}
	}

	// Get the ant that occupies a specific position.
	Ant* getAnt(Pos const& pos) {
		if(posant.count(pos)) {
			return posant[pos];
		}
		return 0;
	}
};

#endif // ANT_H
