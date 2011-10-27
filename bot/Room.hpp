#ifndef ROOM_HPP
#define ROOM_HPP

#include "Pos.hpp"

// This class handles the contents of a room.
// Everything about a room except its connectivity is put into here.
class RoomContents {
public:
	int getNumMyrmidons(); // The number of enemies in the room this turn.
	int getNumEnemies(); // The number of enemies in the room this turn.
	int getNumFallenMyrmidons(); // Give me the number of Myrmidons that has fallen this turn. (See you in valhalla!)
	int getNumFallenEnemies(); // Give me the number of enemies that have fallen this turn.

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
	Pos pos;
	Room* room;
	/* 1 or 2: how many neighbors the cell has to the room
		(3,4 impossible by constraint ofmanhattan-concavity). */
	int neighbors;

	// Less = priority. i.e.: "should a be assigned before b"?
	friend bool operator<(const Interest& a, const Interest& b) {
		if (a.neighbors > b.neighbors) return true;
		if (b.neighbors > a.neighbors) return false;

		return a.room < b.room; // Unrelated tie-breaker.
	}
};

typedef std::set<Interest> InterestSet;

//////////////////////////////////////////////////////////////////

// This class only contains connectivity data. For contents, see 'contents'.
// A room shuld be small enough so that an ant in any part of the room can see
// any other part. This means that an ant will see 2-3 rooms at once.
class Room {
public:
	///////////////////////////////////////////////
	// For users:

	RoomContents* contents() { return m_contents; }

	// Give me the number of Pos the room currently contains.
	int getArea() const { return m_cells.size(); }

	const BB& getBB() const { return m_bb; }

	///////////////////////////////////////////////

private:
	friend class Rooms;
	// For "Rooms" only:

	explicit Room(int ix, Pos seed);
	~Room();

	bool tryExpandWith(Pos pos);
	void add(Pos pos);

	///////////////////////////////////////////////

	const int m_roomIx; // In the global rooms list.
	RoomContents* m_contents;

	PosSet m_cells; // All positions in this Room.
	PosSet m_open; // Positions bordering to unassigned cells.

	// For building:
	InterestSet m_interests; // Cells we want to occupy.

	// Calculate m_interests that coincides with givens positions.
	void calcInterests(const PosSet& pos);

	/////////////////////////////////////////
	// Derived:
	BB m_bb;

	/*
	// for building:
	PosSet closed; // positions that has no neigbor that is not assigned to a room (or water).
	PosSet open; // edge positions with non-assigned neighbors.


	// n^2, where n is the number of cells. symmetric, with zero diagonal.
	// shortestPath[i + j*n] is the length of the shortest path between cells i and j.
	PosList shortestPath;

	/////////////////////////////////////////

	// Have we reached our limit of growth, and is so done?
	bool isFinished() const;

	// Indices of neighboring rooms, if any.
	const IntList& neighborRooms() const;

	// cells inside this room, that borders to neighbor nr "nix" (global index neighborRooms[nix]).
	// from a these border cells there is only one step to the other room.
	// returns indices into our list of positions.
	IntList borderCellsTo(int nix) const;

	// Distance between two neighbor rooms:
	// given two neighbor indices, returns the shortest and longest distance... or something.
	Range neighborDistance(int na, nb) const;
	*/
};
typedef std::vector<Room*> RoomList;

//////////////////////////////////////////////////////////////////

class Rooms
{
public:
	// Called by g_map upon uncovering new grid cells.
	void expandWith(const PosSet& pos);

private:
	RoomList m_rooms;
	RoomList m_open; // rooms not yet closed/finished
};

extern Rooms* g_rooms;

#endif // ROOM_H
