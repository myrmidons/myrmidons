#include "Coordinator.hpp"
#include "Logger.hpp"
#include "Util.hpp"
#include "State.hpp"
#include "Map.hpp"
#include "State.hpp"
#define LOG_COORD(x) LOG_DEBUG("Coordinator " << x)

Coordinator* g_coordinator = 0;

Coordinator::Coordinator()
{
}

void Coordinator::moveAntsAfterDesire(AntSet ants) {

	ITC(AntSet, it, ants) {
		LOG_COORD("Deciding ant move...");

		Ant* ant = *it;
		Pos pos = ant->pos();
		AnAntMove move;
		move.ant = ant;

		ant->calcDesire();
		PosList desire = ant->getDesire();
		bool validMove = false;
		int desirecounter = 0;
		while(!validMove) {
			Pos dest = Pos(-1,-1); // should not be

			// move the ant depending on desire (or if none, dont do nathing)
			if (desire.empty() || ant->state() == Ant::STATE_NONE) {
				dest = pos;
				move.wasIndifferent = true;
				LOG_COORD(*ant << " : no desire. Standing still");
			}
			else {
				if(desirecounter < desire.size()) { // have we tried all the ants desires?
					dest = desire[desirecounter];
					Vec2 d = g_map->difference(pos, dest);
					int dir = STAY;
					if (d.x()<0)      dir = WEST;
					else if (d.x()>0) dir = EAST;
					else if (d.y()<0) dir = NORTH;
					else if (d.y()>0) dir = SOUTH;
				}
			}
			if(movesThisTurn[dest]) {


			}
			desirecounter++;
		}

		if (desire.empty() || ant->state() == Ant::STATE_NONE) {
			move.wasIndifferent = true;
			LOG_COORD(*ant << " : no desire. Standing still");
		} else {
			// Follow desire.
			Pos dest = desire.front();
			LOG_COORD(*ant << " desire to go from " << pos << " to " << dest);
			Vec2 d = g_map->difference(pos, dest);
			int dir = -1;
			if (d.x()<0)      dir = WEST;
			else if (d.x()>0) dir = EAST;
			else if (d.y()<0) dir = NORTH;
			else if (d.y()>0) dir = SOUTH;
			if (dir != -1) {
				LOG_COORD(*ant << " : following desire");
				if(!movesThisTurn[dest]) {
					// no ant has moved here yet(i).
				}

				ant->setExpectedPos(dest);
				g_state->makeMove(pos, dir);
			}
			else {
				LOG_COORD(*ant << " : ants desire is to stand still");
			}
		}
	}
}

AnAntMove* IsMoveTaken(Pos p){
	//movesThisTurn.
	return 0;
}
