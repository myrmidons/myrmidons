#ifndef ROOM_H
#define ROOM_H

#include "Pos.hpp"


// This class handles the contents of a room.
// Everything about a room except its connectivity is put into here.
class RoomContents {
	int lastVisitTime; // in time steps. trustworthyness of everything depends on this.

	IntList food;
	IntList ants;
	IntList hills;
};

// This class only contains connectivity data. For contents, see 'contents'.
// A room shuld be small enough so that an ant in any part of the room can see
// any other part. This means that an ant will see 2-3 rooms at once.
class Room {
	int roomIx; // In the global rooms list.
	RoomContents* contents;

	PosList list; // All positions in this Room (in the order they where added).

	// for building:
	PosSet closed; // positions that has no neigbor that is not assigned to a room (or water).
	PosSet open; // edge positions with non-assigned neighbors.

	/////////////////////////////////////////
	// Derived:
	BB bound;

	// n^2, where n is the number of cells. symmetric, with zero diagonal.
	// shortestPath[i + j*n] is the length of the shortest path between cells i and j.
	PosList shortestPath;

	/////////////////////////////////////////
	// Useful functions:

	int size() const {
		return m_size;
	}

	void resize(int newSize) const {
		m_size = newSize;
		...
	}

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
};
typedef vector<Room*> RoomPtrVec;

class Rooms {
	RoomPtrVec m_rooms;

	// Called by g_map upon uncovering new grid cells.
	void Rooms::expandWith(const PosList& pos);


	int roomAt(Pos pos) const;
};

#endif // ROOM_H
