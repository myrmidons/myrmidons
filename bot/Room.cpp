#include "Room.hpp"
#include "Util.hpp"

////////////////////////////////////////////

Rooms::Room(int ix, Pos seed) : m_roomix(ix) {
	m_list.push_back(seed);
	m_open.push_back(seed);
	m_contents = new RoomContents();
}

Rooms::~Room() {
	delete m_contents;
}

bool Rooms::tryExpandWith(Pos pos) {
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

void Rooms::add(Pos pos) {
	m_list.push_back(pos);
}

////////////////////////////////////////////
// Rooms

void Rooms::expandWith(const PosList& pos) {
	ITC(PostList, pit, pos)
		expandWith(*pit);
}


void Rooms::expandWith(const Pos& pos) {
	if (g_map->isWaterAt(pos))
		return; // The only type of square not in a map

	// See if pos is close to existing expanding room and if so, expand it.
	ITC(RoomPtrVec, rit, m_open) {
		Room* r = *rit;
		if (r->tryExpandWith(pos))
			return;
	}

	// No hits - make room.
	Room* r = new Room(pos);
	m_rooms.push_back(r);
	m_open.push_back(r);
}
