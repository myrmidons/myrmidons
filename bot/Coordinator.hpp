#ifndef COORDINATOR_HPP
#define COORDINATOR_HPP

#include "Ant.hpp"
#include <map>

struct AnAntMove {
	Ant* ant;
	// did the ant just stand still? Then it can still be moved by other ants if they really want the spot for themselves
	bool wasIndifferent;
	// what desire is the result of this move, if another ant really wants to move here and we have other desires as well, we can use them.
	int performedDesire;
};

typedef std::map<Pos, AnAntMove*> AntMoves;


class Coordinator
{
	AntMoves movesThisTurn;
public:
    Coordinator();
	void moveAntsAfterDesire(AntSet ants);
};

extern Coordinator* g_coordinator;

#endif // COORDINATOR_HPP
