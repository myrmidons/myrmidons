#ifndef ANT_HPP
#define ANT_HPP

#include "Pos.hpp"
#include "Path.hpp"
#include <vector>

class Ant {
public:
	enum State {
		STATE_NONE,
		STATE_GOING_TO_FOOD,
		STATE_GOING_TO_ROOM
	};

	explicit Ant(Pos const& loc = Pos());
	~Ant();
	/*const*/ int id; // Unique id per ant. deterministic.
	Pos& pos();
	State state() const { return m_state; }
	const Path& path() const { return m_path; }

	// Returns false on fail.
	bool goTo(Pos pos);
	bool goToFoodAt(Pos pos);
	bool goToRoom(Room* room);
	void stop(); // Stop going towards currnet goal.

	// Ensure our goals are still sound, etc.
	void updateState();

	// update desire
	void calcDesire();

	// Where we would most want to go, based on current state.
	const PosList& getDesire() const { return m_desire; }

private:
	State m_state;
	Pos m_position;

	Path m_path; // Walking along this.

	// Updated each turn:
	PosList m_desire; // Neighbor cells we want to walk to.
};

typedef std::set<Ant*> AntSet;
typedef std::vector<Ant> AntVec;

struct EnemyAnt {
	Pos pos;
	int team;
	EnemyAnt(Pos const& p, int t)
		:pos(p),team(t) {
	}
	EnemyAnt(EnemyAnt const& ea)
		:pos(ea.pos),team(ea.team) {
	}
};

inline bool operator < (EnemyAnt const& a, EnemyAnt const& b) {
	return (a.team < b.team)||((a.team == b.team) && (a.pos < b.pos));
}
inline bool operator == (EnemyAnt const& a, EnemyAnt const& b) {
	return (a.team == b.team) && (a.pos == b.pos);
}

typedef EnemyAnt EnemyHill;
typedef std::vector<EnemyAnt> EnemyList;
typedef std::set<EnemyAnt> EnemyHillSet, EnemySet;

#endif // ANT_HPP
