#ifndef ROOMCONTENTS_HPP
#define ROOMCONTENTS_HPP

#include "Pos.hpp"

class Ant;
typedef std::set<Ant*> AntSet;

// This class handles the contents of a room.
// Everything about a room except its connectivity is put into here.
// Implementations in RoomContent.cpp

class RoomContents {
public:
	// Called each turn?
	void addMyrmidon(Ant* ant);
	void removeMyrmidon(Ant* ant);

	size_t getNumFood();
	size_t getNumMyrmidons(); // The number of enemies in the room this turn.
	size_t getNumEnemies(); // The number of enemies in the room this turn.
	size_t getNumFallenMyrmidons(); // Give me the number of Myrmidons that has fallen this turn. (See you in valhalla!)
	size_t getNumFallenEnemies(); // Give me the number of enemies that have fallen this turn.

	void resetDynamic();

	// Tell the room that it contains an enemy hill and take appropriate action.
	// STUB
	void enemyHillDiscovered(Pos const& pos, int team);

	// Tell the room that it contains a myrmidon hill and take appropriate action.
	// STUB
	void myrmidonHillDiscovered(Pos const& pos);

	void insertFoodAt(Pos const& pos);
	void insertEnemyAt(Pos const& pos, int team);

	const AntSet& ants() const { return m_pAnts; }

	AntSet m_pAnts;
	PosSet m_enemies;
	PosSet m_food;
	PosSet m_enemyHills;
	PosSet m_myrmidonHills;
};

#endif // ROOMCONTENTS_HPP
