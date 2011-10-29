#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include <vector>
#include <set>
#include "State.hpp"
#include "Pos.hpp"
#include "Bug.hpp"
#include "Ant.hpp"

class Food; // Not yet.
class Map;

typedef std::set<size_t> IndexSet;
typedef std::set<Ant*> AntSet;
typedef std::vector<Ant> AntVec;

class Tracker {

	size_t m_numAnts;
	AntVec m_antStorage;
	AntSet m_liveAnts;
	IndexSet m_deadIndices;

	PosSet m_enemyHills;
	PosSet m_hills;

	int m_turn;

public:

	Tracker();

	inline size_t indexOf(Ant* ant) const;

	void beginTurnInput(int n);

	void bufferWater(Pos const& pos);
	void bufferFood(Pos const& pos);
	void bufferAnt(Pos const& pos, int team);
	void bufferDeadAnt(Pos const& pos, int team);
	void bufferHill(Pos const& pos, int team);

	void endTurnInput();

	AntSet&   getAnts();
	EnemySet const& getEnemies() const;
	PosSet const&   getFood() const;

	int turn() const { return m_turn; }

	// All known food, wether visible or not.
	//const PosList& getAllFood() const;

	Bug log;

private:

	void updateMapInfo();

	struct Buffer {

		// Dynamic buffers
		PosSet food;
		PosList water;
		PosList myAnts, deadAnts;
		EnemySet enemyAnts, deadEnemyAnts;
		EnemyHillSet newEnemyHills;
		PosSet    newHills;

		// Static buffers.
		PosSet myHills, enemyHills;

		void resetDynamics();
	};

	Buffer buf;
};

extern Tracker* g_tracker;

#ifdef DEBUG
#	define TRACKER_LOG(msg) g_tracker->log << msg << std::endl << std::flush
#	define TRACKER_LOG_(msg) g_tracker->log << msg
#else
#	define TRACKER_LOG(msg)
#	define TRACKER_LOG_(msg)
#endif

#endif // IDENTIFIER_H
