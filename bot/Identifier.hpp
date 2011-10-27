#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include <vector>
#include <set>

#include "State.hpp"

class Ant;
class Food; // Not yet.
class Pos;
class Map;

typedef std::set<size_t> IndexSet;
typedef std::set<Pos> PosSet;
typedef std::set<Ant*> AntSet;
typedef std::vector<Ant> AntVec;
typedef std::vector<Pos> PosVec;

class Identifier {

	size_t m_numAnts;
	AntVec m_antStorage;
	AntSet m_liveAnts;
	IndexSet m_deadIndices;

	int m_turn;

public:

	Identifier();

	inline size_t indexOf(Ant* ant) const;
	Map* m_map;

	void turn(int n);
	void water(Pos const& pos);
	void food(Pos const& pos);
	void ant(Pos const& pos, int team);
	void deadAnt(Pos const& pos, int team);
	void hill(Pos const& pos, int team);

	void go();

private:

	void update(State* state);

	struct StateBuffer {
		std::vector<Pos> myAnts, enemyAnts, myHills, enemyHills, food, deadAnts, deadEnemies;
		std::vector<int> enemyTeams, deadEnemyTeams, enemyHillTeams;

		void reset();
	};

	StateBuffer buf;
};

#endif // IDENTIFIER_H
