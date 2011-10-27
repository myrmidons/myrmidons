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

// Calculate m_interests that coincides with givens positions.
void Room::calcInterests(const PosSet& unassigned) {
	/* Primary interests are corner cases, positions with two neighbors into our open set.
	   Beyond these the only positions we are alowed to expand too
	   are positions that push out our bounding box.
	   This is the requirement for keeping us Manhattan Convex.
	*/

	map<Pos, int> neighs; // all neighbor cells

	ITC(PosSet, pit, m_open) {
		for (int i=0; i<4; ++i) { // All four directions
			Pos p = g_map.getNeighbor(*pit, i);
			if (unassigned.count(p)) {
				// This is a candidate!
				neighs[p]++;
			}
		}
	}

	m_interests.clear();

	ITC(map<Pos,int>, nit, neighs) {
		if (nit->second==2 || !m_bb.contains(nit->first)) {
			// TODO: limit with maxwidth
			// We may expand here
			Interest intr;
			intr.room = this;
			intr.pos = nit->first;
			intr.neighbors = nit->second;
			m_interests.push_back(intr);
		}
	}
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

bool isInRange(const BB& bb, const Pos& pos, const Vec2& mapSize, int maxRoomWidth) {
	Vec2 r =  bb.distance(pos, mapSize);
	return std::min(r.x(), r.y()) < maxRoomWidth;
}

bool areAnyInRange(Room* room, const PosList& pos, const Vec2& mapSize, int maxRoomWidth) {
	ITC(PosList, pit, pos)
		if (isInRange(room->getBB(), *pit, mapSize, maxRoomWidth))
			return true;
	return false;
}

void Rooms::expandWith(const PosSet& pos) {
	int maxRoomWidth = 10; // FIXME
	Vec2 mapSize = Vec2(200,200); // FIXME

	RoomList rooms; // Affected rooms
	ITC(RoomList, rit, m_open) // Go through open rooms
		if (areAnyInRange(*rit, pos, mapSize, maxRoomWidth))
			rooms.push_back(*rit);
}

Rooms* g_rooms = NULL;
