#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include "Path.hpp"

class Goal {
public:
	/* If room has any goals, put them in outPos.
	   If several goal points are returned and the Path finder
	   is set to only return one goal path, the closest one
	   to the start will be picked.

	   if true is returned and no outPos is empty,
	   the entire room is assumed to be a goal, and the closest cell
	   in the room to the start will be picked.
	   i.e., returning true and not filling outPos, is equivalent
	   to filling it with all positions in a room (or its border).

	   if false is returned, no goals are in the room, and outPos is expected to be empty.
	*/
	virtual bool findGoalsInRoom(Room* room, PosList& outPos) const = 0;
};

////////////////////////////////////////////////////////////////////////
// Some useful types of goals:

// Finds a specific position.
class PosGoal : public Goal {
public:
	explicit PosGoal(Pos pos);

	bool findGoalsInRoom(Room* room, PosList& outPos) const {
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

	bool findGoalsInRoom(Room* room, PosList& /*outPos*/) const {
		return room==m_room;
	}

private:
	Room* m_room;
};


////////////////////////////////////////////////////////////////////////

class PathFinder {
public:
	/* Searh the map for path from "start" to any point accepted by "goals",
	   with a limit of "maxAnswers" goals, and "maxDist" search distance.
	   Paths returned in outPaths, unless NULL.
	   returns the number of hits.
	*/
	static int findPaths(PathList* outPaths, Pos start, const Goal& goal,
						  int maxAnswers=1, int maxDist=std::numeric_limits<int>::max());

	////////////////////////////////////////////////////////////////////////
	// Helpers

	// How do we go from 'start' to 'end'?
	// Returns invalid path if fail.
	static Path findPath(Pos start, Pos end);

private:
	PathFinder();
};

#endif // PATHFINDER_HPP
