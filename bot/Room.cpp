#include "Room.hpp"
#include "Util.hpp"
#include "Map.hpp"

///////////////////////////////////////////////////////////////////////

bool operator<(const Interest& a, const Interest& b) {
	// Expand into where we have many neighbors first
	if (a.neighbors > b.neighbors) return true;
	if (a.neighbors < b.neighbors) return false;

	// Exapand smallest room first
	if (a.room->getArea() < b.room->getArea()) return true;
	if (a.room->getArea() > b.room->getArea()) return false;

	return a.room < b.room; // Unrelated tie-breaker.
}

///////////////////////////////////////////////////////////////////////

Room::Room(Pos seed) {
	m_cells.insert(seed);
	m_open.insert(seed);
	m_bb.m_min = m_bb.m_max = seed;
	m_contents = new RoomContents();
	Square& s = g_map->square(seed);
	assert(s.room==NULL);
	s.room = this;
}

Room::~Room() {
	delete m_contents;
}

bool Room::isClosable(Pos pos) const {
	for (int i=0; i<4; ++i) {
		Pos n = g_map->getLocation(pos, i);
		const Square& s = g_map->square(n);
		if (s.room==NULL && s.isGround())
			return false; // We could expand here!
	}
	return true;
}

void Room::add(Pos pos) {
	m_cells.insert(pos);
	m_open.insert(pos);
	Square& s = g_map->square(pos);
	assert(s.room==NULL);
	s.room = this;

	// Expand boundingbox
	m_bb.expandTo(pos, g_map->size());

	// Check to see if we can close anything in m_open
	PosSet closeThese;
	ITC(PosSet, pit, m_open)
		if (isClosable(*pit))
			closeThese.insert(*pit);
	m_open.erase(closeThese.begin(), closeThese.end());
}

bool Room::isFinished() const {
	return getArea() >= g_rooms->maxRoomArea();
}


// Calculate m_interests that coincides with givens positions.
void Room::calcInterests(const PosSet& unassigned) {
	m_interests.clear();
	m_interestPos.clear();

	if (isFinished())
		return; // We have no interests - we are content.

	/* Primary interests are corner cases, positions with two neighbors into our open set.
	   Beyond these the only positions we are alowed to expand too
	   are positions that push out our bounding box.
	   This is the requirement for keeping us Manhattan Convex.
	*/

	typedef std::map<Pos, int> NeighMap;
	NeighMap neighs; // all neighbor cells

	ITC(PosSet, pit, m_open) {
		for (int i=0; i<4; ++i) { // All four directions
			Pos p = g_map->getLocation(*pit, i);
			if (unassigned.count(p)) {
				// This is a candidate!
				neighs[p]++;
			}
		}
	}

	ITC(NeighMap, nit, neighs) {
		assert(1 <= nit->second && nit->second <= 2);

		if (nit->second==2 || !m_bb.contains(nit->first)) {
			// TODO: limit with maxWidth
			// We may expand here
			Interest intr;
			intr.room = this;
			intr.pos = nit->first;
			intr.neighbors = nit->second;

			m_interests.insert(intr);
			m_interestPos.insert(intr.pos);
		}
	}
}

///////////////////////////////////////////////////////////////////////
// Rooms

bool isInRange(const BB& bb, const Pos& pos, const Vec2& mapSize, int maxRoomWidth) {
	Vec2 r =  bb.distance(pos, mapSize);
	return std::min(r.x(), r.y()) <= maxRoomWidth;
}

bool areAnyInRange(Room* room, const PosSet& pos, const Vec2& mapSize, int maxRoomWidth) {
	ITC(PosSet, pit, pos)
		if (isInRange(room->getBB(), *pit, mapSize, maxRoomWidth))
			return true;
	return false;
}

int Rooms::maxRoomArea() const {
	return 100; // FIXME
}

void Rooms::expandWith(const PosSet& posArg) {
	PosSet unassigned = posArg; // Copy so we can take away one at the time.

	int maxRoomWidth = 10; // FIXME
	Vec2 mapSize = g_map->size();

	RoomSet rooms; // Affected rooms
	ITC(RoomSet, rit, m_open) // Go through open rooms
		if (areAnyInRange(*rit, unassigned, mapSize, maxRoomWidth))
			rooms.insert(*rit);

	InterestSet interests;

	// Find intrested parties:
	ITC(RoomSet, rit, rooms) {
		Room* r = *rit;
		r->calcInterests(unassigned);
		interests.insert(r->m_interests.begin(), r->m_interests.end());
	}

	while (!unassigned.empty()) {
		Room* room; // To which we assign a position.
		if (!interests.empty()) {
			// Assign room with best fit:
			Interest intr = *interests.begin();
			room = intr.room;
			room->add(intr.pos);
			unassigned.erase(intr.pos); // No more!

			// This cell may have been the interest of may rooms.
			// Make sure we re-evaluates the interests of affected rooms:
			ITC(RoomSet, rit, rooms) {
				Room* ar = *rit;
				if (ar->m_interestPos.count(intr.pos)) {
					interests.erase(ar->m_interests.begin(), ar->m_interests.end());
					ar->calcInterests(unassigned);
					interests.insert(ar->m_interests.begin(), ar->m_interests.end());
				}
			}
		} else {
			// No interest taken - create a new room!
			// Which do we select? Any!
			Pos pos = *unassigned.end();
			unassigned.erase(unassigned.end());

			room = new Room(pos);
			m_rooms.push_back(room);
			m_open.insert(room);

			room->calcInterests(unassigned);
			interests.insert(room->m_interests.begin(), room->m_interests.end());
		}

		if (room->isFinished())
			m_open.erase(room);
	}
}

Rooms* g_rooms = NULL;
