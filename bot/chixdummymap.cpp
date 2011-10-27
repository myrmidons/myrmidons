#include "chixdummymap.hpp"
#include "Ant.hpp"
#include "State.hpp"

void Map::removeAnt(Ant* ant) {
	if(!antpos.count(ant)) {
		// FAIL!
		g_state.bug << "FAIL in Map::removeAnt: Ant already removed! (or never added)" << std::endl;
	}
	else if(!posant.count(ant->pos())) {
		// FAIL!
		g_state.bug << "FAIL in Map::removeAnt: Ant postion already removed! (or never added)" << std::endl;
	}
	else {
		posant.erase(ant->pos());
		antpos.erase(ant);
	}
}

// Tell the map that an ant has spawned.
void Map::addAnt(Ant* ant) {
	if(antpos.count(ant)) {
		// FAIL!
		g_state.bug << "FAIL in Map::addAnt: Ant already added!" << std::endl;
	}
	else if(posant.count(ant->pos())) {
		// FAIL!
		g_state.bug << "FAIL in Map::addAnt: Ant position already added!" << std::endl;
	}
	else {
		antpos[ant] = ant->pos();
		posant[ant->pos()] = ant;
	}
}

// Get the ant that occupies a specific position.
Ant* Map::getAnt(Pos const& pos) {
	if(posant.count(pos)) {
		return posant[pos];
	}
//		g_state.bug << "In Map::getAnt: Ant does not exist!" << std::endl;
	return 0;
}

void Map::moveAnt(Pos const& from, Pos const& to) {
	g_state.bug << "Looking for ant at " << from << ": ";
	Ant* ant = getAnt(from);
	if(ant) {
		g_state.bug << "and moving it from " <<  ant->pos() << " to ";
		removeAnt(ant);
		ant->pos() = to;
		addAnt(ant);
		g_state.bug << ant->pos() << std::endl;
	}
	else {
		g_state.bug << " without finding it." << std::endl;
	}
}
