#include "Path.hpp"
#include "Map.hpp"
#include "Room.hpp"
#include "Util.hpp"
#include "Logger.hpp"
#include <algorithm>

////////////////////////////////////////////////////

Path::Path(int dist, Pos start, Pos end, const WPList& wps)
	: m_dist(dist), m_distLeft(dist), m_start(start), m_end(end), m_points(wps) {
}

Vec2 deltaAlong(Vec2 pos, int axis, Vec2 d) {
	pos[axis] += sign(d[axis]);
	return g_map->wrapPos( pos );
}

void testAndAdd(PosList& dest, Room* room, Pos pos) {
	Square& s = g_map->square(pos);
	if (s.isWater || s.room!=room)
		return;
	dest.push_back(pos);
}

// Assumes in same room
PosList prioritizeWalk(Room* room, Pos from, Pos to) {
	Vec2 d = g_map->difference(from, to);
	if (d==Vec2(0,0))
		return PosList(1, to); // We have arrived

	int prioAxis = (Abs(d[0]) > Abs(d[1]) ? 0 : 1);
	PosList ret;
	testAndAdd(ret, room, deltaAlong(from, prioAxis, d));

	if (d[1-prioAxis] != 0)
		testAndAdd(ret, room, deltaAlong(from, 1-prioAxis, d));

	return ret;
}

PosList Path::getNextStep(Pos pos) {
	STAMP("Path::getNextStep");

	ASSERT(this->isValid());

	if (pos==m_end) {
		LOG_DEBUG("getNextStep has arrived");
		m_distLeft = 0;
		return PosList(1, pos); // We have arrived
	}

	Room* room = g_map->roomAt(pos);

	if (room == g_map->roomAt(m_end)) {
		// We're in last room
		LOG_DEBUG("In goal room");
		m_distLeft = g_map->manhattanDist(pos, m_end);
		return prioritizeWalk(room, pos, m_end);
	}

	// See where along path we are:
	int wpIx = -1;
	for (int i=0; i<(int)m_points.size(); ++i) {
		const WayPoint& wp = m_points[i];
		if (wp.room == room) {
			// This is where we're at
			wpIx = i;
			break;
		}
	}

	if (wpIx == -1) {
		LOG_DEBUG("Path::getNextStep failed: not on path");
		m_distLeft = -1;
		return PosList();
	}

	ASSERT(wpIx != (int)m_points.size()-1); // Can't be last - we would be at end then.

	const WayPoint& nextWP = m_points[wpIx+1];
	Room* nextRoom = nextWP.room;

	int distLeftInNextRoom = this->length() - nextWP.dist;

	PosList ret;

	// See if we neighbor any cell in nextRoom already:
	for (int dir=0; dir<4; ++dir) {
		Pos nc = g_map->getLocation(pos, dir);
		if (g_map->roomAt(nc) == nextRoom)
			ret.push_back(nc); // We can go here
	}

	if (!ret.empty()) {
		m_distLeft = distLeftInNextRoom + g_map->manhattanDist(ret[0], nextWP.pos);
		return ret; // We can go to neighbor room right now, so let's.
	}

	// Find good path to neighbor room.
	Pos targetCell = room->closestPosNearNeighbor(pos, nextRoom);

	m_distLeft = g_map->manhattanDist(pos, targetCell) + 1 + distLeftInNextRoom;

	return prioritizeWalk(room, pos, targetCell);
}
