#ifndef PATH_HPP
#define PATH_HPP

#include "Pos.hpp"

class Room;

struct WayPoint {
	WayPoint(Room* r, Pos p, int d) : room(r), pos(p), dist(d) { }

	Room* room;
	Pos pos;    // In room
	int dist;   // From start of path to this waypoint.
};

typedef std::vector<WayPoint> WPList;

// Create paths with PathFinder
class Path
{
public:
	// Invalid path
	Path() : m_dist(-1), m_distLeft(-1) {}

	bool isValid() const { return m_dist >= 0; }

	/* We are at pos, along this path.
	   Where should we go next to get towards our goal?
	   This will return a list of suggested neighbor positions
	   to pos in order of preference.
	   Returns just "pos" on finish.
	   Return empty list if 'pos' is not on path.
	   */
	PosList getNextStep(Pos pos);

	const Pos& start() const { return m_start; }
	const Pos& dest() const { return m_end; }
	// Ant-steps from start to dest
	int length() const { return m_dist; }

	// updated by "getNextStep".
	int distanceLeft() const { return m_distLeft; }

	const WPList& wayPoints() const { return m_points; }

private:
	explicit Path(int dist, Pos start, Pos end, const WPList& rooms);
	friend class PathFinder;

	int m_dist;
	int m_distLeft;
	Pos m_start, m_end;
	WPList m_points;
};

typedef std::vector<Path> PathList;

struct PathLengthComp {
	bool operator()(const Path& a, const Path& b) const {
		return a.length() < b.length();
	}
};

#endif // PATH_HPP
