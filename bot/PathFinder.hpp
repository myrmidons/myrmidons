#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include "Path.hpp"

class Goal {
public:
	/* If room has any goals, put them in outPos.
	   If several goal points are returned and the Path finder
	   is set to only return one goal path, the closest one
	   to the start will be picked.

	   "pos" is the position in the room closest to the start,
	   and "dist" the distane from the start to that position.
	   So the total distance from the start to a point in the room is
	   dist + g_maps->manhattanDistance(pos, point)

	   if true is returned and no outPos is empty,
	   the entire room is assumed to be a goal, and the closest cell
	   in the room to the start will be picked.
	   i.e., returning true and not filling outPos, is equivalent
	   to filling it with all positions in a room (or its border).

	   if false is returned, no goals are in the room, and outPos is expected to be empty.
	*/
	virtual bool findGoalsInRoom(PosList& outPos, Room* room, const Pos& pos, int dist) const = 0;
};

////////////////////////////////////////////////////////////////////////
// Some useful types of goals:

// Finds a specific position.
class PosGoal : public Goal {
public:
	explicit PosGoal(Pos pos);

	bool findGoalsInRoom(PosList& outPos, Room* room, const Pos& /*pos*/, int /*dist*/) const {
		if (m_room == room) {
			outPos.push_back(m_pos);
			return true;
		} else {
			return false;
		}
	}

private:
	Pos m_pos;
	Room* m_room;
};

// Find any position in a specific room
class RoomGoal : public Goal {
public:
	explicit RoomGoal(Room* room) :m_room(room) {}

	bool findGoalsInRoom(Room* room, PosList& /*outPos*/, const Pos& /*pos*/, int /*dist*/) const {
		return room==m_room;
	}

private:
	Room* m_room;
};


// Finds food that no other ant is heading for AND is closer to.
class FoodGoal : public Goal {
public:
	bool findGoalsInRoom(PosList& outPos, Room* room, const Pos& pos, int dist) const;
};

// Returns all ants in a room
class AntGoal : public Goal {
public:
	bool findGoalsInRoom(PosList& outPos, Room* room, const Pos& pos, int dist) const;
};


////////////////////////////////////////////////////////////////////////

class PathFinder {
public:
	enum Flags {
		FLAGS_NONE = 0,
		FLAGS_SORT = 1   // Sort returned path by increasing distance from start.
	};

	/* Searh the map for path from "start" to any point accepted by "goals",
	   with a limit of "maxAnswers" goals, and "maxDist" search distance.
	   Paths returned in outPaths, unless NULL.
	   returns the number of hits.
	*/
	static int findPaths(PathList* outPaths, Pos start, const Goal& goal,
						  int maxAnswers=1, int maxDist=std::numeric_limits<int>::max(), Flags flags = FLAGS_SORT);

	////////////////////////////////////////////////////////////////////////
	// Helpers

	// How do we go from 'start' to 'end'?
	// Returns invalid path if fail.
	static Path findPath(Pos start, Pos end);

private:
	PathFinder();
};

#endif // PATHFINDER_HPP
