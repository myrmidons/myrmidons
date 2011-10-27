#ifndef CHIXDUMMYMAP_HPP
#define CHIXDUMMYMAP_HPP

#ifndef MAP_HPP

#include <map>

class Ant;
class Pos;
#include "State.hpp"

// This is a dummy for the future map-class to be created by goff and emilk.
class Map {
	std::map<Ant*, Pos> antpos;
	std::map<Pos, Ant*> posant;

public:

	Map() {
		g_state.bug << "Creating dummy map." << std::endl;
	}

	// Tell the map that a certain ant is dead.
	// the map don't own the ant, and should be prepared for
	// getting the same pointer back later as a newly spawned ant.
	void removeAnt(Ant* ant); // Used by Identifier

	// Tell the map that an ant has spawned.
	void addAnt(Ant* ant); // Used by Identifier

	// Get the ant that occupies a specific position.
	Ant* getAnt(Pos const& pos); // Used by Identifier

	// Tell the map there is an enemy from a certain team at this position.
	//void enemy(Pos const& pos, int team); // Used by Identifier

	// Tell the map that there is a dead enemy at this position.
	// * May be called multiple times with the same position in one turn
	// if the enemy ants are stupid enough to collide with each other.
	// (In that case they will belong to the same team, since ants of different teams
	// will battle it out befor colliding.)
	//void deadEnemy(Pos const& pos, int team); // Used by Identifier

	// Tell the map where food have spawned.
	//void addFood(Pos const& pos); // Used by Identifier

	// Tell the map that food have disapeared.
	//
	// Should the identifier try to track if it have been eaten by an ant?
	// i.e. also provide something like 'void eatenFood(Pos const& pos, int team)'?
	// In that case this would only be called when food have fallen into the FoW.
	//
	// DISCUSS.
	//void lostFood(Pos const& pos);  // Used by Identifier

	// Tell the map that new water have been discovered.
	//void water(Pos const& pos);

	// Not sure if this is up to the Map-class in the end, but probably.
	void moveAnt(Pos const& from, Pos const& to); // Not used by the AntIdentifyer, just by Bot for testing purposes.
};
#endif

#endif // CHIXDUMMYMAP_HPP
