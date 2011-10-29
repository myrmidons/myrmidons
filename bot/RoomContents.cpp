#include "RoomContents.hpp"
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
	m_enemyHills.insert(pos);
}

void RoomContents::myrmidonHillDiscovered(Pos const& pos) {
	m_myrmidonHills.insert(pos);
}

void RoomContents::resetDynamic() {
	m_food.clear();
	m_enemies.clear();
}

void RoomContents::insertFoodAt(Pos const& pos) {
	m_food.insert(pos);
}

void RoomContents::insertEnemyAt(Pos const& pos, int team) {
	m_enemies.insert(pos);
}

size_t RoomContents::getNumMyrmidons() {
	return m_pAnts.size();
}

size_t RoomContents::getNumEnemies() {
	return m_enemies.size();
}

size_t RoomContents::getNumFood() {
	return m_food.size();
}

//int getNumFallenMyrmidons(); // Give me the number of Myrmidons that has fallen this turn. (See you in valhalla!)
//int getNumFallenEnemies();
