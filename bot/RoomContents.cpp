#include "Room.hpp"
#include "Assert.hpp"

void RoomContents::addMyrmidon(Ant* ant) {
	ASSERT(m_pAnts.find(ant) == m_pAnts.end());
	m_pAnts.insert(ant);
}

void RoomContents::removeMyrmidon(Ant* ant) {
	ASSERT(m_pAnts.find(ant) != m_pAnts.end());
	m_pAnts.erase(ant);
}

void RoomContents::enemyHillDiscovered(Pos const& pos, int team) {
	// STUB
	// Begin drafting process, we have an invation to undertake!
}

void RoomContents::myrmidonHillDiscovered(Pos const& pos) {
	// STUB
	// Keep this under close guard, let no enemy near it!
}

void RoomContents::resetDynamic() {
	// Reset non-persistent stuff, like enemy ants a.s.o.
	// STUB
	m_food.clear();
}

void RoomContents::foodAt(Pos const& pos) {
	m_food.insert(pos);
}

int RoomContents::getNumMyrmidons() {
	return m_pAnts.size();
} // The number of enemies in the room this turn.

//int getNumEnemies(); // The number of enemies in the room this turn.
//int getNumFallenMyrmidons(); // Give me the number of Myrmidons that has fallen this turn. (See you in valhalla!)
//int getNumFallenEnemies();
