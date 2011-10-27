#include "Room.hpp"
#include "Util.hpp"
#include "Map.hpp"

#ifdef DEBUG
#	include <QImage>
#endif

Rooms* g_rooms = NULL;

///////////////////////////////////////////////////////////////////////

bool operator<(const Interest& a, const Interest& b) {
	// Expand into where we have many neighbors first
	if (a.neighbors > b.neighbors) return true;
	if (a.neighbors < b.neighbors) return false;

	// Try to keep squareness:
	if (a.prio > b.prio) return true;
	if (a.prio < b.prio) return false;

	// Exapand smallest room first
	if (a.room->getArea() < b.room->getArea()) return true;
	if (a.room->getArea() > b.room->getArea()) return false;

	return a.room < b.room; // tie-breaker.
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
	if (getArea() >= g_rooms->maxRoomArea())
		return true;
	Vec2 bbSize = m_bb.size(g_map->size());
	int maxWidth = g_rooms->maxRoomWidth();
	if (bbSize.x() >= maxWidth) return true;
	if (bbSize.y() >= maxWidth) return true;
	return false;
}


// Calculate m_interests that coincides with given positions.
void Room::calcInterests(const PosSet& unassigned) {
	m_interests.clear();
	m_interestPos.clear();

	if (isFinished())
		return; // We have no interests - we are content.

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

		/* Primary interests are corner cases, positions with two neighbors into our open set.
		   Beyond these the only positions we are alowed to expand too
		   are positions that push out our bounding box.
		   This is the requirement for keeping us Manhattan Convex.
		*/

		if (nit->second==2 || !m_bb.contains(nit->first)) {
			// We may expand here
			Interest intr;
			intr.room = this;
			intr.pos = nit->first;
			intr.neighbors = nit->second;

			if (intr.neighbors==2) {
				intr.prio=0;
			} else {
				Vec2 bbSize = m_bb.size(g_map->size());
				// We're expanding bb - check on wich side.
				Vec2 d = m_bb.distance(nit->first, g_map->size());
				assert(d.x()==0 || d.y()==0);
				assert(d.x()==1 || d.y()==1);
				int axis = (d.x() > d.y() ? 0 : 1);

				if (bbSize[axis] >= g_rooms->maxRoomWidth())
					continue; // We may not expand this way!

				intr.prio = bbSize[1-axis] - bbSize[axis]; // larger on other axis is good
			}

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

int Rooms::maxRoomWidth() const {
	return 10; // TODO: base on g-state view distance.
}

void Rooms::expandWith(const PosSet& posArg) {
	PosSet unassigned = posArg; // Copy so we can take away one at the time.

	int maxRoomWidth = this->maxRoomWidth();
	Vec2 mapSize = g_map->size();

	RoomSet rooms; // Rooms in range of new positions (optimization)
	ITC(RoomSet, rit, m_open) // Go through open rooms
		if (areAnyInRange(*rit, unassigned, mapSize, maxRoomWidth))
			rooms.insert(*rit);

	InterestSet interests;

	// Query interest in specific positions
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
			rooms.insert(room);

			room->calcInterests(unassigned);
			interests.insert(room->m_interests.begin(), room->m_interests.end());
		}

		if (room->isFinished()) {
			m_open.erase(room);
			rooms.erase(room); // No longer intersting for us
		}
	}
}

#ifdef DEBUG
QRgb randomColor(Room* r) {
	srand(reinterpret_cast<long>(r));
	return qRgb(rand()%255, rand()%255, rand()%255);
}

// Dump a png of the room colorings.
void Rooms::dumpImage() const {
	Vec2 size = g_map->size();
	//int Mult = 1; // Pixels per grid cell.
	QImage img(size.x(), size.y(), QImage::Format_ARGB32);
	img.fill(0);

	std::map<Room*, QRgb> colorMap;
	ITC(RoomList, rit, m_rooms)
		colorMap[*rit] = randomColor(*rit);

	for (int y=0; y<size.y(); ++y) {
		for (int x=0; x<size.x(); ++x) {
			Square& s = g_map->square(Pos(x,y));
			if (s.room) {
				img.setPixel(x, y, colorMap[s.room]);
			}
		}
	}

	img.save("rooms.png");
}
#endif
