#include "Ant.hpp"
#include "Map.hpp"
#include "Room.hpp"
#include "State.hpp".hpp"

Ant::Ant(const Pos &loc)
	: m_state(STATE_NONE), m_position(loc) {
}

Ant::Ant(Ant const& ant)
	: m_position(ant.m_position){

}

Ant& Ant::operator=(Ant const& ant) {
	m_position = ant.m_position;
	return *this;
}

Pos& Ant::pos() {
	return m_position;
}

bool Ant::goTo(Pos dest) {
	m_path = Path::findPath(this->pos(), dest);
	if (m_path.isValid()) {
		// Win
		m_state = STATE_GOING_TO_ROOM;
		return true;
	} else {
		// Fail
		m_state = STATE_NONE;
		return false;
	}
}

bool Ant::goToFoodAt(Pos dest) {
	LOG_DEBUG("Ant::goToFoodAt");
	if (goTo(dest)) {
		// Win
		m_state = STATE_GOING_TO_FOOD;
		return true;
	}
	return false;
}

bool Ant::goToRoom(Room* room) {
	if (goTo(room->centerPos())) {
		// Win
		m_state = STATE_GOING_TO_ROOM;
		return true;
	}
	return false;
}

void Ant::calcDesire() {
	if (!m_path.isValid())
		m_state = STATE_NONE;

	if (m_state==STATE_GOING_TO_FOOD) {
		if (!g_map->square(m_path.dest()).isFood) {
			m_state = STATE_NONE;
		}
	}

	if (m_state==STATE_GOING_TO_ROOM) {
		if (g_map->roomAt(pos()) && g_map->roomAt(m_path.dest()))
			m_state = STATE_NONE;
	}

	if (m_state == STATE_NONE) {
		m_desire.clear();
		// TODO: priorities.
		m_desire.push_back( g_map->getLocation(pos(), rand()%4) );
	} else {
		m_desire = m_path.getNextStep(pos());
	}
}

