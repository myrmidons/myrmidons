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

// Rooms volenteer interest in unassigned cells.
// This interest is kept track of using this:
struct Interest
{
	Pos pos;
	Room* room;
	/* 1 or 2: how many neighbors the cell has to the room
		(3,4 impossible by constraint ofmanhattan-concavity). */
	int neighbors;

	// Less = priority. i.e.: "should a be assigned before b"?
	friend bool operator<(const Interest& a, const Interest& b) {
		if (a.neighbors > b.neighbors) return true;
		if (b.neighbors > a.neighbors) return false;

		return a.room < b.room; // Unrelated tie-breaker.
	}
};

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

void Rooms::expandWith(const PosList& pos) {
	int maxRoomWidth = 10; // FIXME
	Vec2 mapSize = Vec2(200,200); // FIXME

	RoomList rooms; // Affected rooms
	ITC(RoomList, rit, m_open) // Go through open rooms
		if (areAnyInRange(*rit, pos, mapSize, maxRoomWidth))
			rooms.push_back(*rit);
}

Rooms* g_rooms = NULL;
