#include "Ant.hpp"
#include "Map.hpp"
#include "Room.hpp"
#include "State.hpp"

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
		LOG_DEBUG("Failed to go to path");
		m_state = STATE_NONE;
		return false;
	}
}

bool Ant::goToFoodAt(Pos dest) {
	LOG_DEBUG("Ant::goToFoodAt " << dest);
	if (goTo(dest)) {
		// Win
		m_state = STATE_GOING_TO_FOOD;
		return true;
	}
	return false;
}

bool Ant::goToRoom(Room* room) {
	if (room == g_map->roomAt(pos()))
		LOG_DEBUG("Asked to go to room it is in");

	LOG_DEBUG("Ant::goToRoom " << room);
	if (goTo(room->centerPos())) {
		// Win
		m_state = STATE_GOING_TO_ROOM;
		return true;
	}
	return false;
}

void Ant::calcDesire() {
	LOG_DEBUG("Ant::calcDesire");

	if (!m_path.isValid()) {
		LOG_DEBUG("Invalid path, going to STATE_NONE");
		m_state = STATE_NONE;
	}

	if (m_state==STATE_GOING_TO_FOOD) {
		if (!g_map->square(m_path.dest()).isFood) {
			LOG_DEBUG("FOOD GONE!");
			m_state = STATE_NONE;
		}
	}

	if (m_state==STATE_GOING_TO_ROOM) {
		if (g_map->roomAt(pos()) && g_map->roomAt(m_path.dest())) {
			LOG_DEBUG("Arrived to room");
			m_state = STATE_NONE;
		}
	}

	if (m_state == STATE_NONE) {
		m_desire.clear();
		// TODO: priorities.
		m_desire.push_back( g_map->getLocation(pos(), rand()%4) );
	} else {
		m_desire = m_path.getNextStep(pos());
	}
}

