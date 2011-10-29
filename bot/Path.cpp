#include "Path.hpp"
#include "Map.hpp"
#include "Room.hpp"
#include "Util.hpp"
#include "Logger.hpp"
#include <algorithm>

struct SearchNode {
	SearchNode(SearchNode* par, Pos p, int d) : parent(par), pos(p), room(g_map->square(p).room), dist(d) {}

	SearchNode* parent; // We got here from where?
	Pos pos;
	Room* room;
	int dist; // So far
};

struct SearchNodeComp {
	bool operator()(const SearchNode* a, const SearchNode* b) const {
		//if (a->dist != b->dist)
		//	return a->dist < b->dist
		return a->dist < b->dist;
	}
};

typedef std::multiset<SearchNode*, SearchNodeComp> SearchNodeQueue;

Path Path::findPath(Pos start, Pos end)
{
	LOG_DEBUG("Path::findPath");

	Room* startRoom = g_map->square(start).room;
	Room* endRoom   = g_map->square(end).room;
	ASSERT(startRoom && endRoom);

	if (startRoom==endRoom) {
		LOG_DEBUG("Path finding within same room");
		return Path(g_map->manhattanDist(start, end), start, end, WPList());
	}

	SearchNodeQueue q;
	std::set<SearchNode*> allSearchNodes; // For freeing

	SearchNode* startNode = new SearchNode(NULL, start, 0);

	// Do breath first search from startRoom to endRoom. TODO: A*
	q.insert(startNode);
	allSearchNodes.insert(startNode);

	RoomSet closedRooms;

	Path retPath;

	while (!q.empty()) {
		// Get closest
		SearchNode* p = *q.begin();
		q.erase(q.begin());
		closedRooms.insert(p->room);

		if (p->room != endRoom) {
			// I still haven't found what I've been looking for.
			// Check neighbor rooms:
			const RoomSet& neighs = p->room->neighborRooms();
			ITC(RoomSet, rit, neighs) {
				Room* r = *rit;
				if (!closedRooms.count(r)) {
					int dist=0;
					Pos pos = p->room->closestPosInNeighbor(p->pos, r, &dist);
					ASSERT(dist>0);
					SearchNode* newNode = new SearchNode(p, pos, p->dist + dist);
					q.insert(newNode);
					allSearchNodes.insert(newNode);
				}
			}
		} else {
			// Done! Reconstruct path...
			LOG_DEBUG("Found path, building...");

			int sumDist = p->dist + g_map->manhattanDist(p->pos, end);
			WPList wps;
			while (p) {
				wps.push_back(WayPoint(p->room, p->pos));
				p = p->parent;
			}
			std::reverse(wps.begin(), wps.end());

			retPath = Path(sumDist, start, end, wps);
			break; // we're done here.
		}
	}

	ITC(std::set<SearchNode*>, wpit, allSearchNodes)
		delete *wpit;

	LOG_DEBUG("Path::findPath returning");

	return retPath; // invalid if fail
}

////////////////////////////////////////////////////

Path::Path(int dist, Pos start, Pos end, const WPList& wps)
	: m_dist(dist), m_start(start), m_end(end), m_points(wps) {
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

PosList Path::getNextStep(Pos pos) const {
	STAMP("Path::getNextStep");

	ASSERT(this->isValid());

	if (pos==m_end) {
		LOG_DEBUG("getNextStep has arrived");
		return PosList(1, pos); // We have arrived
	}

	Room* room = g_map->roomAt(pos);

	if (room == g_map->roomAt(m_end)) {
		// We're in last room
		LOG_DEBUG("In goal room");
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
		return PosList();
	}

	ASSERT(wpIx != (int)m_points.size()-1); // Can't be last - we would be at end then.

	Room* nextRoom = m_points[wpIx+1].room;

	PosList ret;

	// See if we neighbor any cell in nextRoom already:
	for (int dir=0; dir<4; ++dir) {
		Pos nc = g_map->getLocation(pos, dir);
		if (g_map->roomAt(nc) == nextRoom)
			ret.push_back(nc); // We can go here
	}

	if (!ret.empty())
		return ret; // We can go to neighbor room right now, so let's.

	// Find good path to neighbor room.
	Pos targetCell = room->closestPosNearNeighbor(pos, nextRoom);
	return prioritizeWalk(room, pos, targetCell);
}
