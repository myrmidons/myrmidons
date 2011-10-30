#include "Square.hpp"
#include "Tracker.hpp"
#include "Room.hpp"

void Square::markAsVisible() {
	m_isVisible = true;
	m_lastVisible = g_tracker->turn();
}

RoomContent* Square::roomContent() {
	ASSERT(room);
	return room->content();
}
