#include "RoomContent.hpp"
#include "Assert.hpp"
#include "Square.hpp"
#include "Room.hpp"
#include "Map.hpp"

RoomContent::RoomContent(Room* room) : m_room(room) {}

size_t RoomContent::getNumMyrmidons() {
	return m_pAnts.size();
}

size_t RoomContent::getNumEnemies() {
	return m_enemies.size();
}

size_t RoomContent::getNumFood() {
	return m_food.size();
}

//int getNumFallenMyrmidons(); // Give me the number of Myrmidons that has fallen this turn. (See you in valhalla!)
//int getNumFallenEnemies();


//////////////////////////////////////////////////////////////////////////

void RoomContent::resetDynamic() {
	m_food.clear();
	m_enemies.clear();
}

void RoomContent::addAnt(Ant* ant) {
	ASSERT(m_pAnts.find(ant) == m_pAnts.end());
	m_pAnts.insert(ant);
}

void RoomContent::removeAnt(Ant* ant) {
	ASSERT(m_pAnts.find(ant) != m_pAnts.end());
	m_pAnts.erase(ant);
}

void RoomContent::addEnemy(Pos const& pos, int team) {
	m_enemies.insert(pos);
}

void RoomContent::addFood(Pos const& pos) {
	m_food.insert(pos);
}

void RoomContent::addHill(Pos const& pos, int team) {
	if (team==OUR_TEAM)
		m_myrmidonHills.insert(pos);
	else
		m_enemyHills.insert(pos);
}


void RoomContent::update() {
	m_empty = (m_pAnts.empty() && m_enemies.empty() && m_myrmidonHills.empty() && m_enemyHills.empty() && m_food.empty());

	m_visible = true;
	ITC(PosSet, pit, m_room->positions()) {
		Square& s = g_map->square(*pit);
		if (!s.visible())
			m_visible = false;
	}
}
