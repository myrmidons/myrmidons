#include "Room.hpp"
#include "Util.hpp"

///////////////////////////////////////////////////////////////////////

Room::Room(int ix, Pos seed) : m_roomIx(ix) {
	m_cells.push_back(seed);
	m_open.push_back(seed);
	m_contents = new RoomContents();
}

Room::~Room() {
	delete m_contents;
}

bool Room::tryExpandWith(Pos pos) {
	/*
	// OPTIMIZE: bb early out
	// Check to see it is next to us.
	ITC(PosList, pit, m_open) {
		if (isNeighbors(pos, *pit)) {
			add(pos);
			return true;
		}
	}
	*/
	return false;
}

void Room::add(Pos pos) {
	m_cells.push_back(pos);
}

///////////////////////////////////////////////////////////////////////
// Rooms

void Rooms::expandWith(const PosList& pos) {
	ITC(PosList, pit, pos)
		expandWith(*pit);
}


void Rooms::expandWith(const Pos& pos) {
	//if (g_map->isWaterAt(pos))
	//	return; // The only type of square not in a map

	// See if pos is close to existing expanding room and if so, expand it.
	ITC(RoomPtrVec, rit, m_open) {
		Room* r = *rit;
		if (r->tryExpandWith(pos))
			return;
	}

	// No hits - make room.
	Room* r = new Room(m_rooms.size(), pos);
	m_rooms.push_back(r);
	m_open.push_back(r);
}
