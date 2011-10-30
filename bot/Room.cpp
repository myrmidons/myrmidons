#include "Room.hpp"
#include "RoomContent.hpp"
#include "Util.hpp"
#include "Map.hpp"
#include "State.hpp"
#include "Logger.hpp"
#include <limits>
#include <cmath>

#ifdef DEBUG
#	include <QImage>
#	include <QPainter>
#	include <sstream>
#	include <cstdio>
#endif

Rooms* g_rooms = NULL;

//#define ROOM_SPAM(x) LOG_DEBUG("Room: " << x);
#define ROOM_SPAM(x)

///////////////////////////////////////////////////////////////////////

bool operator<(const Interest& a, const Interest& b) {
	//ROOM_SPAM("Interest operator <");

	// Expand into where we have many neighbors first
	if (a.neighbors > b.neighbors) return true;
	if (a.neighbors < b.neighbors) return false;

	// Prioritize things close to center. Will keep squareness and roundness, and priotize small rooms.
	if (a.centerDistSq != b.centerDistSq) return a.centerDistSq < b.centerDistSq;
/*
	// Try to keep squareness:
	if (a.prio > b.prio) return true;
	if (a.prio < b.prio) return false;
*/

	// Exapand smallest room first
	if (a.area < b.area) return true;
	if (a.area > b.area) return false;

	return a.room->id < b.room->id; // tie-breaker.
}

///////////////////////////////////////////////////////////////////////

Room::Room(Pos seed) : id(getID<Room>()), m_center(seed), m_dirty(true) {
	m_bb.m_min = m_bb.m_max = seed;
	m_content = new RoomContent(this);
	add(seed);
}

Room::~Room() {
	delete m_content;
}

Pos Room::centerPos() const {
	return m_center;
}

void Room::makeClean() const {
	if (!m_dirty) return;

	m_neighbors.clear();
	m_neighborInfos.clear();

	ITC(PosSet, pit, m_cells) {
		for (int dir=0; dir<4; ++dir) {
			Pos cell = g_map->getLocation(*pit, dir);
			if (Room* r = g_map->square(cell).room) {
				if (r != this) {
					m_neighbors.insert(r);
					m_neighborInfos[r].cells.insert(*pit);
				}
			}
		}
	}

	m_dirty = false; // No longer
}

const RoomSet& Room::neighborRooms() const {
	makeClean();
	return m_neighbors;
}

const Room::NeighborInfo* Room::neighborInfo(Room* room) const {
	makeClean();
	ASSERT(m_neighborInfos.count(room));
	return &m_neighborInfos[room];
}

Pos Room::closestPosNearNeighbor(Pos from, Room* neighbor, int* outDist) const {
	ASSERT(this != neighbor);

	const NeighborInfo* ni = neighborInfo(neighbor);

	// Find best path from "from" to a cell in neighbor room
	const int MaxInt = std::numeric_limits<int>::max();
	int shortest = MaxInt;
	Pos closest;

	ITC(PosSet, pit, ni->cells) {
		int dist = g_map->manhattanDist(from, *pit);
		if (dist < shortest) {
			shortest = dist;
			closest = *pit;
		}
	}

	ASSERT(shortest != MaxInt);

	if (outDist)
		*outDist = shortest;

	return closest;
}

Pos Room::closestPosInNeighbor(Pos from, Room* neighbor, int* outDist) const {
	int dist;
	Pos closest = closestPosNearNeighbor(from, neighbor, &dist);

	// Take one step into other room:
	for (int dir=0; dir<4; ++dir) {
		Pos nc = g_map->getLocation(closest, dir);
		if (g_map->roomAt(nc) == neighbor) {
			// This will do
			if (outDist)
				*outDist = dist + 1; // +1 to take that extra step
			return nc;
		}
	}

	// What the hell - this shouldnt happen
	LOG_ERROR("closestPosInNeighbor failed goin from " << from << ", with closest at " << closest);
	LOG_ERROR("Room is " << this << ", neighbor is " << neighbor);

	ASSERT(false && "Room connections broken");

	if (outDist)
		*outDist = -1;

	return from; // Fail. return w/e.
}

bool Room::isClosable(Pos pos) const {
	for (int i=0; i<4; ++i) {
		Pos n = g_map->getLocation(pos, i);
		const Square& s = g_map->square(n);
		if (s.room==NULL && (!s.isWater || !s.discovered))
			return false; // We could expand here!
	}
	ROOM_SPAM("Closing cell " << pos << " in room " << this);
	return true;
}

// A border cell has at least one neighbor not in room. could be water.
bool isBorderCell(Pos pos, Room* r) {
	for (int dir=0; dir<4; ++dir)
		if (g_map->roomAt(g_map->getLocation(pos, dir)) != r)
			return true;
	return false;
}

// break if we get anything more than "currentBest"
int bcRad2(int currentBest, Pos pos, const PosList& cells) {
	int r2=0;
	ITC(PosList, pit, cells) {
		r2 = std::max(r2, g_map->euclidDistSq(pos, *pit));
		if (r2 > currentBest)
			return std::numeric_limits<int>::max();
	}
	return r2;
}

void Room::add(Pos pos) {
	ROOM_SPAM("Room::add " << pos);

	m_cells.insert(pos);
	m_open.insert(pos);
	Square& s = g_map->square(pos);
	ASSERT(s.discovered);
	ASSERT(!s.isWater);
	ASSERT(s.room==NULL);
	s.room = this;

	// Expand boundingbox
	m_bb.expandTo(pos, g_map->size());

	ROOM_SPAM("bb is now from " << m_bb.m_min << " to " << m_bb.m_max);

	// Check to see if we can close anything in m_open
	PosSet closeThese;
	ITC(PosSet, pit, m_open)
		if (isClosable(*pit))
			closeThese.insert(*pit);

	ITC(PosSet, pit, closeThese)
		m_open.erase(*pit);

	///////////////////////////////////////////

	// Recalculate room center. Start by calculating bounding points.
	PosList border;
	ITC(PosSet, pit, m_cells)
		if (isBorderCell(*pit, this))
			border.push_back(*pit);

	int minRad2 = std::numeric_limits<int>::max();
	ITC(PosSet, pit, m_cells) {
		int r2 = bcRad2(minRad2, *pit, border);
		if (r2 < minRad2) {
			m_center = *pit;
			minRad2 = r2;
		}
	}

	///////////////////////////////////////////

	// We are now unsure of room-connection - dirty up affected rooms:
	this->m_dirty = true;
	for (int i=0; i<4; ++i)
		if (Room* r = g_map->square(g_map->getLocation(pos, i)).room)
			r->m_dirty = true;

	ROOM_SPAM("Room::add DONE");
}

bool Room::isFinished() const {
	ROOM_SPAM("Room::isFinished (this = " << this << ")");

	if (m_open.empty()) {
		ROOM_SPAM("Room::isFinished: m_open.empty()");
		return true;
	}

	if (getArea() >= g_rooms->maxRoomArea()) {
		ROOM_SPAM("Room::isFinished: maxRoomArea");
		return true;
	}

	/*
	Vec2 bbSize = m_bb.size(g_map->size());
	int maxWidth = g_rooms->maxRoomWidth();

	if (bbSize.x() >= maxWidth) return true;
	if (bbSize.y() >= maxWidth) return true;

	we stop this by not subscribing interest in cells outside a too long axis
	*/

	return false;
}


// Calculate m_interests that coincides with given positions.
void Room::calcInterests(const PosSet& unassigned) {
	ROOM_SPAM("Room::calcInterests (this = " << this << ")");

	//ROOM_SPAM("clearing m_interestPos...");
	m_interestPos.clear();

	//ROOM_SPAM("clearing m_interests...");
	m_interests.clear();

	//ROOM_SPAM("cleared");

	if (isFinished()) {
		ROOM_SPAM("Room::calcInterests - bailing out - we're finished!");
		return; // We have no interests - we are content.
	}

	typedef std::map<Pos, int> NeighMap;
	NeighMap neighs; // all neighbor cells

	ROOM_SPAM("Finding neighbors to room...");

	ITC(PosSet, pit, m_open) {
		ASSERT(g_map->square(*pit).isGround());
		for (int i=0; i<4; ++i) { // All four directions
			Pos p = g_map->getLocation(*pit, i);
			if (unassigned.count(p) && g_map->square(p).isGround()) { // We cant have water in rooms
				// This is a candidate!
				neighs[p]++;
			}
		}
	}

	ROOM_SPAM("Culling " << neighs.size() << " neighbors...");

	Vec2 mapSize = g_map->size();

	ITC(NeighMap, nit, neighs) {
		ASSERT(1 <= nit->second && nit->second <= 2);

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
			intr.area = this->getArea();

			if (intr.neighbors==2) {
				intr.prio=0;
			} else {
				Vec2 bbSize = m_bb.size(g_map->size());
				// We're expanding bb - check on wich side.
				Vec2 d = m_bb.distance(nit->first, g_map->size());
				ASSERT(d.x()==0 || d.y()==0);
				ASSERT(d.x()==1 || d.y()==1);
				int axis = (d.x() > d.y() ? 0 : 1);

				if (bbSize[axis] >= g_rooms->maxRoomWidth()) {
					ROOM_SPAM("Ignoring neighbor at " << intr.pos << " expanding bb too much");
					continue; // We may not expand this way!
				}

				intr.prio = bbSize[1-axis] - bbSize[axis]; // larger on other axis is good
			}

			//intr.centerDistSq = wrappedDistanceSqr(m_bb.centerF(mapSize), Vec2f(intr.pos), mapSize);
			intr.centerDistSq = g_map->euclidDistSq(m_center, intr.pos);

			if (intr.centerDistSq > g_rooms->maxRoomRadiusSq()) {
				ROOM_SPAM("Ignoring neighbor at " << intr.pos << " going outside max radius");
				continue; // We may not expand this way!
			}

			ROOM_SPAM("Adding neighbor cell at " << nit->first << " to interests...");

			m_interests.insert(intr);
			m_interestPos.insert(intr.pos);
		}
	}

	ROOM_SPAM("Room::calcInterests DONE");
}

///////////////////////////////////////////////////////////////////////
// Rooms

bool isInRange(const BB& bb, const Pos& pos, const Vec2& mapSize, int maxRoomWidth) {
	Vec2 r =  bb.distance(pos, mapSize);
	return std::max(r.x(), r.y()) <= maxRoomWidth;
}

bool areAnyInRange(Room* room, const PosSet& pos, const Vec2& mapSize, int maxRoomWidth) {
	ITC(PosSet, pit, pos)
		if (isInRange(room->getBB(), *pit, mapSize, maxRoomWidth))
			return true;
	return false;
}

Rooms::Rooms() {
}

int Rooms::maxRoomArea() const {
	return maxRoomWidth()*maxRoomWidth(); // FIXME
}

int Rooms::maxRoomWidth() const {
	//return 10; // TODO: base on g-state view distance.
	return (int)std::ceil(2 * maxRoomRadius());
}

float Rooms::maxRoomRadius() const {
	return g_state->viewradius; // FIXME
}

float Rooms::maxRoomRadiusSq() const {
	return sqr(maxRoomRadius()); // FIXME
}

void Rooms::expandWith(const PosSet& posArg) {
	if (posArg.empty())
		return;

#ifdef DEBUG
	ITC(PosSet, pit, posArg) {
		Square& s = g_map->square(*pit);
		ASSERT(s.room==NULL);
		ASSERT(s.discovered);
		ASSERT(s.visible());
	}
#endif

	ROOM_SPAM("-" << std::endl << "Rooms::expandWith");

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
		ROOM_SPAM(unassigned.size() << " unassigned left");

		Room* room; // To which we assign a position.
		if (!interests.empty()) {
			ROOM_SPAM("A");

			// Assign room with best fit:
			Interest intr = *interests.begin();
			room = intr.room;

			ASSERT(rooms.count(room)); // How could we have gooten interest in this if not true?

			ROOM_SPAM("Assigning position " << intr.pos << " to room " << room);

			/*
			// IMPORTANT: do this before add, since add chages size of room, which interest uses.
			ROOM_SPAM("Erasing interests...");
			ITC(InterestSet, iit, room->m_interests)
				interests.erase(*iit);
				*/

			room->add(intr.pos);
			unassigned.erase(intr.pos); // No more!

			ROOM_SPAM("Updating interests...");

			// This cell may have been the interest of my rooms.
			// Make sure we re-evaluates the interests of affected rooms:
			ITC(RoomSet, rit, rooms) {
				Room* ar = *rit;
				if (ar->m_interestPos.count(intr.pos)) {
					ROOM_SPAM("Erasing old interests...");
					ITC(InterestSet, iit, ar->m_interests)
						interests.erase(*iit);

					ROOM_SPAM("Calculating new interests...");
					ar->calcInterests(unassigned);

					ROOM_SPAM("Adding new interests...");
					interests.insert(ar->m_interests.begin(), ar->m_interests.end());
				}
			}

			ROOM_SPAM("Interests updated (" << interests.size() << " left)");
		} else {
			ROOM_SPAM("B");

			// No interest taken - create a new room!
			// FIXME: what is the best choice here?
#if 1
			// Leftmost, topmost
			PosSet::iterator it = unassigned.begin();
#elif 0
			// Rightmost, bottommost
			PosSet::iterator it = unassigned.end();
			--it;
#else
			// Random
			PosSet::iterator it = unassigned.begin();
			std::advance(it, rand() % unassigned.size());
#endif
			Pos pos = *it;
			unassigned.erase(it);

			if (!g_map->square(pos).isGround())
				continue; // Discovered water? who cares!

			room = new Room(pos);
			m_rooms.push_back(room);
			m_open.insert(room);
			rooms.insert(room);

			ROOM_SPAM("Creating room " << room << " at " << pos);

			room->calcInterests(unassigned);
			ROOM_SPAM("New room has " << room->m_interests.size() << " interests");
			interests.insert(room->m_interests.begin(), room->m_interests.end());
		}

		if (room->isFinished()) {
			ROOM_SPAM("Removing finished room " << room);
			m_open.erase(room);
			rooms.erase(room); // No longer intersting for us
		}
	}

	// TODO: cull finished rooms from m_open

	ROOM_SPAM("Rooms::expandWith DONE" << std::endl << "-");
}

void Rooms::resetDynamics() {
	IT(RoomList, it, m_rooms) {
		(*it)->content()->resetDynamic();
	}
}

void Rooms::update() {
	IT(RoomList, it, m_rooms) {
		(*it)->content()->update();
	}
}
