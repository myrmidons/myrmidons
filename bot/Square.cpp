#include "Square.hpp"
#include "Tracker.hpp"

void Square::markAsVisible() {
	m_isVisible = true;
	m_lastVisible = g_tracker->turn();
}
