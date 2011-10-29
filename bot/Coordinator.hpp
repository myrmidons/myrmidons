#ifndef COORDINATOR_HPP
#define COORDINATOR_HPP

#include "Ant.hpp"
#include <map>

struct AntMove {
	Ant* ant;
	// what desire is the result of this move, if another ant really wants to move here and we have other desires as well, we can use them.
	int choice;   // which of the desired moves we are currently testing. All lower ones already denied.
	int nDesires;

	// If none of our desires where possible  - we're apatic. If possible, stand still - else go anywhere.
	bool isApatic() { return choice >= nDesires; }
	int desiresLeft() { return isApatic() ? 0 : nDesires - choice; }

	void nextChoice() {
		if (desiresLeft()>0)
			choice++; // he didn't get to do this - maybe something else?
	}
};

typedef std::map<Pos, AntMove> AntMoves;


class Coordinator
{
public:
    Coordinator();
	void moveAntsAfterDesire(AntSet ants);

private:
	AntMoves m_grid; // Ants assigned into this as they are given their final moves.
};

extern Coordinator* g_coordinator;

#endif // COORDINATOR_HPP
