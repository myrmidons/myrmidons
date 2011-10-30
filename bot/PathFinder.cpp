#include "PathFinder.hpp"
#include "Map.hpp"
#include "Logger.hpp"
#include "Room.hpp"

PosGoal::PosGoal(Pos pos) : m_pos(pos), m_room(g_map->roomAt(pos)) {
}

////////////////////////////////////////////////////////////////////////////////

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

int PathFinder::findPaths(PathList* outPaths, Pos start, const Goal& goal, int maxAnswers, int maxDist)
{
	LOG_DEBUG("PathFinder::findPath");
	if (outPaths)
		outPaths->reserve(std::min(maxAnswers, 100));

	Room* startRoom = g_map->square(start).room;
	ASSERT(startRoom);

	SearchNodeQueue q;
	std::set<SearchNode*> allSearchNodes; // For freeing

	SearchNode* startNode = new SearchNode(NULL, start, 0);

	// Do breath first search from startRoom to endRoom. TODO: A*
	q.insert(startNode);
	allSearchNodes.insert(startNode);

	RoomSet closedRooms;

	int foundPaths=0;

	while (!q.empty()) {
		// Get closest
		SearchNode* const p = *q.begin();
		q.erase(q.begin());

		// Check room:
		PosList goalPos;
		if (goal.findGoalsInRoom(p->room, goalPos)) {
			LOG_DEBUG("Goal found in room...");

			if (goalPos.empty()) {
				// Any goal in the room will do - add the one we're at:
				goalPos.push_back(p->pos);
			}

			/*
			int keep = std::max((int)goalPos.size(), foundPaths-maxAnswers);
			if (goalPos.size() > keep) {
				// TODO: keep the closest ones
			}
			*/

			if (outPaths) {
				LOG_DEBUG("Building paths to room");
				// Build paths:

				// All goals in this room use same way-points.
				WPList wps;
				{
					SearchNode* node = p;
					while (node) {
						wps.push_back(WayPoint(node->room, node->pos));
						node = node->parent;
					}
					std::reverse(wps.begin(), wps.end());
				}

				ITC(PosList, pit, goalPos) {
					Pos end = *pit;
					int sumDist = p->dist + g_map->manhattanDist(p->pos, end);
					outPaths->push_back( Path(sumDist, start, end, wps) );
				}
			}

			foundPaths += goalPos.size();

			if (foundPaths >= maxAnswers) {
				LOG_DEBUG("maxAnswers=" << maxAnswers << " hit, breaking  (foundPaths=" << foundPaths << ")");
				break; // Done!
			}
		}

		closedRooms.insert(p->room);

		if (p->dist > maxDist) {
			LOG_DEBUG("PathFinder::findPath hit maxDist of " << maxDist << " after searching " << closedRooms.size() << " rooms ");
			break;
		}

		// I still haven't found what I've been looking for.
		// Check neighbor rooms:
		const RoomSet& neighs = p->room->neighborRooms();
		ITC(RoomSet, rit, neighs) {
			Room* r = *rit;

			//if (r->worthless()) continue; // Early out

			if (!closedRooms.count(r)) {
				int dist=0;
				Pos pos = p->room->closestPosInNeighbor(p->pos, r, &dist);
				ASSERT(dist>0);
				SearchNode* newNode = new SearchNode(p, pos, p->dist + dist);
				q.insert(newNode);
				allSearchNodes.insert(newNode);
			}
		}
	}

	ITC(std::set<SearchNode*>, wpit, allSearchNodes)
		delete *wpit;

	LOG_DEBUG("Path::findPath returning " << foundPaths << " path(s)");

	return foundPaths;
}

////////////////////////////////////////////////////////////////////////

Path PathFinder::findPath(Pos start, Pos end) {
	PathList paths;
	PosGoal goal(end);
	findPaths(&paths, start, goal, 1);
	if (paths.empty()) {
		return Path(); // Fail
	} else {
		return paths.front();
	}
}
