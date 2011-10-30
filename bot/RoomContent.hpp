#ifndef ROOMCONTENTS_HPP
#define ROOMCONTENTS_HPP

#include "Pos.hpp"
#include "Util.hpp" // IdComp
#include "Ant.hpp"

// This class handles the contents of a room.
// Everything about a room except its connectivity is put into here.
// Implementations in RoomContent.cpp

class RoomContent {
public:
	explicit RoomContent(Room* room);

	/*
	size_t getNumFood();
	size_t getNumMyrmidons(); // The number of enemies in the room this turn.
	size_t getNumEnemies(); // The number of enemies in the room this turn.
	size_t getNumFallenMyrmidons(); // Give me the number of Myrmidons that has fallen this turn. (See you in valhalla!)
	size_t getNumFallenEnemies(); // Give me the number of enemies that have fallen this turn.
	*/

	const AntSet& ants() const { return m_pAnts; }
	const PosSet& food() const { return m_food; }

	// Is the entire room currently visible?
	bool visible() const { return m_visible; }

	// Nothing in it.
	bool empty() const { return m_empty; }

	//////////////////////////////////////////////////////////////////////////
	// Used by g_map to update room contents:

	// Called each turn:
	void resetDynamic();
	void addAnt(Ant* ant);
	void removeAnt(Ant* ant);
	void addEnemy(Pos const& pos, int team);
	void addFood(Pos const& pos);

	// Called when first discovered:
	void addHill(Pos const& pos, int team);

	void update(); // Called once a turn to update statistics abotu the room

	//////////////////////////////////////////////////////////////////////////

	Room* m_room; // Parent

	// Given by map:
	AntSet m_pAnts;
	PosSet m_enemies;
	PosSet m_food;
	PosSet m_enemyHills;
	PosSet m_myrmidonHills;

	// Collected info in update()
	bool m_empty;    // As far as we know
	bool m_visible;  // in entirety.
};

#endif // ROOMCONTENTS_HPP
