#ifndef ROOM_HPP
#define ROOM_HPP

#include "Pos.hpp"
#include <map>

// This class handles the contents of a room.
// Everything about a room except its connectivity is put into here.
// Implementations in RoomContent.cpp
class RoomContents {
public:
	int getNumMyrmidons(); // The number of enemies in the room this turn.
	int getNumEnemies(); // The number of enemies in the room this turn.
	int getNumFallenMyrmidons(); // Give me the number of Myrmidons that has fallen this turn. (See you in valhalla!)
	int getNumFallenEnemies(); // Give me the number of enemies that have fallen this turn.

	void resetDynamic();

	// Tell the room that it contains an enemy hill and take appropriate action.
	// STUB
	void enemyHillDiscovered(Pos const& pos, int team);

	// Tell the room that it contains a myrmidon hill and take appropriate action.
	// STUB
	void myrmidonHillDiscovered(Pos const& pos);

	int lastVisitTime; // in time steps. trustworthyness of everything depends on this.

	// indices into room positions list?
	IntList food;
	IntList ants;
	IntList hills;
};

//////////////////////////////////////////////////////////////////

class Room;

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

class RoomComp {
public:
	bool operator()(Room* a, Room *b) const;
};

typedef std::vector<Room*> RoomList;
typedef std::set<Room*, RoomComp> RoomSet;

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

	const RoomSet& neighborRooms() const;

	// give info about our connection to this room.
	const NeighborInfo* neighborInfo(Room* room) const;

	///////////////////////////////////////////////

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

	bool m_dirty; // For everything below this:

	typedef std::map<Room*, NeighborInfo> Neighbors;

	mutable RoomSet m_neighbors;
	mutable Neighbors m_neighborInfos;
};

//////////////////////////////////////////////////////////////////

class Rooms
{
public:
	// Room constraints
	int maxRoomArea() const;
	int maxRoomWidth() const;
	float maxRoomRadius() const;
	float maxRoomRadiusSq() const;

	// Called by g_map upon uncovering new grid cells.
	void expandWith(const PosSet& pos);

	void resetDynamicContent();

#ifdef DEBUG
	// Dump a png of the room colorings.
	void dumpImage() const;
#endif

private:
	RoomList m_rooms;
	RoomSet m_open; // rooms not yet closed/finished
};

extern Rooms* g_rooms;

#endif // ROOM_H
