#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include <vector>
#include <set>

#include "Bug.hpp"

class Ant;
class Food; // Not yet.
class Pos;
class Map;

typedef std::set<size_t> IndexSet;
typedef std::set<Pos> PosSet;
typedef std::vector<Pos> PosVec;
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

	void turn(int n);
	void water(Pos const& pos);
	void food(Pos const& pos);
	void ant(Pos const& pos, int team);
	void deadAnt(Pos const& pos, int team);
	void hill(Pos const& pos, int team);

	void go();

	AntSet const& getLiveAnts();


	Bug log;

private:

	void update();

	struct Buffer {
		std::vector<Pos> myAnts, enemyAnts, myHills, food, deadAnts, deadEnemies;
		std::vector<int> enemyTeams, deadEnemyTeams;

		typedef std::pair<Pos, int> EnemyHill;
		typedef std::set<EnemyHill> EnemyHillSet;

		EnemyHillSet newEnemyHills;
		PosSet newHills;

		void reset();
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
