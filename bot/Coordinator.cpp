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

bool mayMoveTo(Pos pos) {
	Square& s = g_map->square(pos);
	return !s.isWater;
}

void Coordinator::moveAntsAfterDesire(AntSet ants) {
	m_grid.clear();

	typedef std::vector<AntMove> Queue;

	Queue q;
	// Add all ants to queue
	ITC(AntSet, it, ants) {
		AntMove move;
		move.ant = *it;
		move.choice = 0; // First choice if possible

		move.ant->calcDesire();

		move.nDesires = move.ant->getDesire().size();
		PosList desires = move.ant->getDesire();

		ITC(PosList, dit, desires) {
			LOG_COORD(*move.ant << " " << *dit);
		}
		move.ant->setExpectedPos(move.ant->pos()); // until we say otherwise
		q.push_back(move);
		LOG_COORD(*move.ant << " has " << move.nDesires << " desires");
	}

	AntSet unassigned; // Ants that can't go anywhere

	// When apatic - which directions should we try? STILL must be first, the rest can be better (individual)
	int apaticDirs[5] = {STAY, NORTH, EAST, SOUTH, WEST};

	int iter=0;
	const int MaxIter = ants.size() * 5; // We should have resolved by this time...

	while (!q.empty()) {
		++iter;
		if (iter >= MaxIter) { // SAFETY FIRST
			LOG_ERROR("Coordinator::moveAntsAfterDesire did not converge! Ants will move... intrestingly.");
			break;
		}

		AntMove move = q.back();
		q.pop_back();

		// Try to find a place for this ant...
		if (!move.isApatic()) {
			LOG_COORD(*move.ant << " is not apathic");

			// Try next desire
			const PosList& desires = move.ant->getDesire();
			Pos desire = desires[move.choice];

			// May we go there?
			if (!mayMoveTo(desire)) {
				LOG_WARNING(*move.ant << " desired to move to water");
				move.nextChoice();
				q.push_back(move);
			}
			else if (m_grid.count(desire)==0) {
				LOG_COORD(*move.ant << " has been chosen for pos " << desire);
				// Wohoo - go there!
				m_grid[desire] = move;
				continue;
			} else {
				// Grid occupied - should we push him?
				AntMove other = m_grid[desire];

				// If the guy was apatic to start iwth - push him.
				// or, if we have no options left and he does: push him.
				bool push = (other.nDesires==0 ||
							(move.desiresLeft()==0 && other.desiresLeft()>0));
				if (push) {
					LOG_COORD(*move.ant << " pushing " << *other.ant << " at " << desire);

					// We push him
					other.nextChoice();
					q.push_back(other);
					m_grid[desire] = move; // We move there!
				} else {
					LOG_COORD(*move.ant << " wants to go to " << desire << " but is already occupied by " << *other.ant);
					// We can't push him. We reinsert ourselves.
					move.nextChoice();
					q.push_back(move);
					// Dont assign to grid this turn.
				}
			}
		} else {
			// We're apatic - move to any free square.
			// prefer standing still - often best (not a step backward).
			LOG_COORD(*move.ant << " is apatic");
			bool assigned = false;

			for (int i=0; i<5; ++i) {
				int dir = apaticDirs[i];
				Pos desire = g_map->getLocation(move.ant->pos(), dir);
				if (mayMoveTo(desire) && m_grid.count(desire)==0) {
					// yay
					m_grid[desire] = move;
					assigned = true;
					break;
				}
			}

			if (!assigned) {
				LOG_COORD("WARNING: Ant " << *move.ant << " has no freedoms - it will DIIIIEEEE!");
				unassigned.insert(move.ant);
			}
		}
	}

	// All ants been assigned a place to go. Send out these commands.

	ITC(AntMoves, mit, m_grid) {
		Pos pos = mit->first;
		AntMove move = mit->second;

		// inform ant..
		move.ant->setExpectedPos(pos);
	}

	ITC(AntSet, ait, unassigned) {
		// Our failures
		// TODO: go to where they to least damage.
		Ant* ant = *ait;
		ant->setExpectedPos(ant->pos()); // Stay and wait for death.
	}

	///////////////////////////////////////////////////////////////////
	// All ants have expected positions - commit!

	ITC(AntSet, ait, ants) {
		Ant* ant = *ait;
		Pos current = ant->pos();
		Pos desire = ant->expectedPos();

		Vec2 d = g_map->difference(current, desire);
		int dir = STAY;
		if (d.x()<0)      dir = WEST;
		else if (d.x()>0) dir = EAST;
		else if (d.y()<0) dir = NORTH;
		else if (d.y()>0) dir = SOUTH;
		if (dir != STAY) {
			LOG_COORD(*ant << " will try to go from " << current << " to " << desire);
			g_state->makeMove(current, dir);
		}
		else {
			LOG_COORD(*ant << " : ants desire is to stand still. Want to go from " << current << " to " << desire);
		}
	}
}
