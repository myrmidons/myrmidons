#include "Ant.hpp"
#include "Map.hpp"
#include "Room.hpp"
#include "State.hpp"
#include "Logger.hpp"
#include "PathFinder.hpp"
#include "Util.hpp"

#define LOG_ANT(a, x) LOG_DEBUG(*a << " " << x)

Ant::Ant(const Pos &loc)
	: id(getID<Ant>()), m_state(STATE_NONE), m_position(loc) {
}

Ant::~Ant() {
	stop(); // Clear Square.destinyAnt
}

Pos Ant::pos() const {
	return m_position;
}

Pos Ant::expectedPos() {
	return m_expectedPosition;
}

void Ant::setExpectedPos(Pos p) {
	m_expectedPosition = p;
}
void Ant::setPos(Pos p) {
	m_position = p;
}

void Ant::wantToGoTo(Pos dest) {
        m_desire = m_path.getNextStep(dest);
}

bool Ant::goTo(Pos dest) {
	Path newPath = PathFinder::findPath(this->pos(), dest);
	if (newPath.isValid()) {
		// Win - there is a path
		m_path = newPath;
		LOG_ANT(this, "goTo " << dest);
		m_state = STATE_GOING_TO_POS;

		Square& s = g_map->square(dest);

		if (s.destinyAnt) {
			// Some-one else was header here.
			if (s.destinyAnt->state() == STATE_GOING_TO_FOOD)
				s.destinyAnt->stop(); // It should do something else.
		}

		s.destinyAnt = this;
		return true;
	} else {
		// Fail
		LOG_ANT(this, "goTo failed");
		//stop();
		return false;
	}
}

bool Ant::goToHillAt(Pos dest) {
	LOG_ANT(this, "goToHillAt " << dest);
	if (goTo(dest)) {
		// Win
		m_state = STATE_GOING_TO_HILL;
		return true;
	}
	return false;
}

bool Ant::goToFoodAt(Pos dest) {
	LOG_ANT(this, "goToFoodAt " << dest);
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
		LOG_ANT(this, "Asked to go to room it is in");

	LOG_ANT(this, "goToRoom " << room);
	if (goTo(room->centerPos())) {
		LOG_ANT(this, "Going to room");
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
	LOG_ANT(this, "Ant::updateState");

	if (!m_path.isValid()) {
		LOG_ANT(this, "Invalid path, stopping");
		stop();
	}

	if (m_state&STATE_GOING_TO_POS) {
		if (pos() == m_path.dest()) {
			LOG_DEBUG("At destination");
			stop();
		}
	}

	if (m_state==STATE_GOING_TO_HILL && !g_map->square(m_path.dest()).hillAlive)
		stop(); // Hill dead.

	if (m_state==STATE_GOING_TO_FOOD) {
		Square& s = g_map->square(m_path.dest());
		if (s.visible() && !s.isFood) {
			LOG_DEBUG("FOOD GONE!");
			stop();
		}
	}

	if (m_state==STATE_GOING_TO_ROOM) {
		if (g_map->roomAt(pos()) == g_map->roomAt(m_path.dest())) {
			LOG_ANT(this, "Arrived to room");
			m_state = STATE_NONE;
		}
	}
}

void Ant::calcDesire() {
	LOG_ANT(this, "calcDesire");

	m_desire.clear();

	if (m_state == STATE_NONE) {
		// TODO: priorities.
		//m_desire.push_back( g_map->getLocation(pos(), rand()%4) );
	} else {
		m_desire = m_path.getNextStep(pos());

		if (m_desire.size()==0 || (m_desire.size()==1 && m_desire[0]==pos())) {
			LOG_ANT(this, "Resetting state - at target?");
			stop();
		}
	}
}
