#ifndef ROOM_HPP
#define ROOM_HPP

#include "Pos.hpp"
#include "Ant.hpp"
#include <map>

//////////////////////////////////////////////////////////////////

class Room;
class RoomContents;

// Rooms volenteer interest in unassigned cells.
// This interest is kept track of using this:
struct Interest
{
	Interest() : room(0), neighbors(-1), prio(-1337) {}

	Pos pos;
	Room* room;
	/* 1 or 2: how many neighbors the cell has to the room
		(3,4 impossible by constraint of manhattan-concavity). */
	int neighbors;
	int area; // Of room - smaller is prioritized
	float prio; // Based on wether or not we are expanding the bb to make it more square.
	float centerDistSq; // Distance from room center to cell.
};


// Less = priority. i.e.: "should a be assigned before b"?
bool operator<(const Interest& a, const Interest& b);

typedef std::set<Interest> InterestSet;

//////////////////////////////////////////////////////////////////

class Room;

typedef std::vector<Room*> RoomList;
typedef std::set<Room*, IdComp> RoomSet;

// Only Emil may code here!
// This class only contains connectivity data. For contents, see 'contents'.
// A room shuld be small enough so that an ant in any part of the room can see
// any other part. This means that an ant will see 2-3 rooms at once.
// Implementation in Room.cpp
class Room {
public:
	const int id; // Unique id per room. deterministic.

	///////////////////////////////////////////////
	// For users:

	class NeighborInfo {
	public:
		// Cells in this room, that is one manhattan step away from the neighbor room.
		PosSet cells;
	};

	RoomContents* contents() { return m_contents; }

	// Give me the number of Pos the room currently contains.
	int getArea() const { return m_cells.size(); }

	const BB& getBB() const { return m_bb; }

	/* A point in the room from which all parts of the room can
	   be seen by an ant. */
	Pos centerPos() const;

	const RoomSet& neighborRooms() const;

	// give info about our connection to this room.
	const NeighborInfo* neighborInfo(Room* room) const;

	///////////////////////////////////////////////

	// Helpers:
	Pos closestPosNearNeighbor(Pos from, Room* neighbor, int* outDist=NULL) const;
	Pos closestPosInNeighbor(Pos from, Room* neighbor, int* outDist=NULL) const;

private:
	friend class Rooms;
	// For "Rooms" only:

	explicit Room(Pos seed);
	~Room();

	void add(Pos pos);

	// At maximum size, or otherwise "unexpandable".
	bool isFinished() const;

	bool isClosable(Pos pos) const;

	void makeClean() const; // Lazy-calc everything that is dirty.

	///////////////////////////////////////////////

	RoomContents* m_contents;

	PosSet m_cells; // All positions in this Room.
	PosSet m_open; // Positions bordering to unassigned cells.

	// For building:
	InterestSet m_interests; // Cells we want to occupy.
	PosSet m_interestPos; // the positions of the above set

	// Calculate m_interests that coincides with givens positions.
	void calcInterests(const PosSet& pos);

	/////////////////////////////////////////
	// Derived:
	BB m_bb;
	Pos m_center; // Visual center, euclid minimum bounding circle center.

	mutable bool m_dirty; // For everything below this:

	typedef std::map<Room*, NeighborInfo, IdComp> Neighbors;

	mutable RoomSet m_neighbors;
	mutable Neighbors m_neighborInfos;
};

//////////////////////////////////////////////////////////////////

class Rooms
{
public:
	Rooms();

	const RoomList& rooms() const { return m_rooms; }

	// Room constraints
	int maxRoomArea() const;
	int maxRoomWidth() const;
	float maxRoomRadius() const;
	float maxRoomRadiusSq() const;

	// Called by g_map upon uncovering new grid cells.
	void expandWith(const PosSet& pos);

	void resetDynamics();

private:
	RoomList m_rooms;
	RoomSet m_open; // rooms not yet closed/finished
};

extern Rooms* g_rooms;

#endif // ROOM_H
