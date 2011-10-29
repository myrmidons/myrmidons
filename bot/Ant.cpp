#include "Ant.hpp"
#include "Map.hpp"
#include "Room.hpp"
#include "State.hpp"
#include "Util.hpp"

Ant::Ant(const Pos &loc)
	: id(getID<Ant>()), m_state(STATE_NONE), m_position(loc) {
}

Ant::~Ant() {
	stop(); // Clear Square.destinyAnt
}

Pos& Ant::pos() {
	return m_position;
}

bool Ant::goTo(Pos dest) {
	m_path = Path::findPath(this->pos(), dest);
	if (m_path.isValid()) {
		// Win
		m_state = STATE_GOING_TO_ROOM;
		g_map->square(dest).destinyAnt = this;
		return true;
	} else {
		// Fail
		LOG_DEBUG("Ant::goTo failed");
		stop();
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
	stop();

	if (room == g_map->roomAt(pos()))
		LOG_DEBUG("Asked to go to room it is in");

	LOG_DEBUG("Ant::goToRoom " << room);
	if (goTo(room->centerPos())) {
		LOG_DEBUG("Going to room");
		m_state = STATE_GOING_TO_ROOM;
		return true;
	}
	return false;
}

void Ant::stop() {
	if (m_path.isValid()) {
		Square& s = g_map->square(m_path.dest());
		if (s.destinyAnt == this)
			s.destinyAnt = NULL;
	}
	m_path=Path();
	m_state = STATE_NONE;
}

void Ant::updateState() {
	LOG_DEBUG("Ant::updateState");

	if (!m_path.isValid()) {
		LOG_DEBUG("Invalid path, stopping");
		stop();
	}

	if (m_state==STATE_GOING_TO_FOOD) {
		Square& s = g_map->square(m_path.dest());
		if (s.isVisible && !s.isFood) {
			LOG_DEBUG("FOOD GONE!");
			stop();
		}
	}

	if (m_state==STATE_GOING_TO_ROOM) {
		if (g_map->roomAt(pos()) == g_map->roomAt(m_path.dest())) {
			LOG_DEBUG("Arrived to room");
			m_state = STATE_NONE;
		}
	}
}

void Ant::calcDesire() {
	LOG_DEBUG("Ant::calcDesire");

	m_desire.clear();

	if (m_state == STATE_NONE) {
		// TODO: priorities.
		//m_desire.push_back( g_map->getLocation(pos(), rand()%4) );
	} else {
		m_desire = m_path.getNextStep(pos());

		if (m_desire.size()==0 || (m_desire.size()==1 && m_desire[0]==pos())) {
			LOG_DEBUG("Resetting state - at target?");
			stop();
		}
	}
}
