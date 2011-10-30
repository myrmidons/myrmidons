#ifndef ANT_HPP
#define ANT_HPP

#include "Pos.hpp"
#include "Path.hpp"
#include "Util.hpp" // IdComp
#include <vector>

class Ant {
public:
	enum State {
		STATE_NONE,
		STATE_GOING_TO_POS  = 1,
		STATE_GOING_TO_HILL = (STATE_GOING_TO_POS | 2),
		STATE_GOING_TO_FOOD = (STATE_GOING_TO_POS | 4),
		STATE_GOING_TO_ROOM = 8
	};

	explicit Ant(Pos const& loc = Pos());
	~Ant();

	const int id; // Unique id per ant. deterministic.

	Pos pos() const;
	Pos expectedPos();
	State state() const { return m_state; }
	const Path& path() const { return m_path; }

	// Returns false on fail.
	bool goTo(Pos pos);
	bool goToHillAt(Pos pos);
	bool goToFoodAt(Pos pos);
	bool goToRoom(Room* room);
	void stop(); // Stop going towards currnet goal.

	// Ensure our goals are still sound, etc.
	void updateState();

	// update desire
	void calcDesire();

	// Where we would most want to go, based on current state.
	const PosList& getDesire() const { return m_desire; }

	// Sets the expected position for next round, should only be used by the coordinator
	void setExpectedPos(Pos p);

	// Only used by tracker!
	void setPos(Pos p);
private:
	State m_state;
	Pos m_position;
	Pos m_expectedPosition; // What pos do this ant expect to be at the next turn? (used for tracking)
	Path m_path; // Walking along this.

	// Updated each turn:
	PosList m_desire; // Neighbor cells we want to walk to.
};

typedef std::set<Ant*, IdComp> AntSet;
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
inline std::ostream& operator<<(std::ostream &os, const Ant& ant) {
	return os << "Ant[" << ant.id << ", " <<  ant.pos() <<"]";
}

typedef EnemyAnt EnemyHill;
typedef std::vector<EnemyAnt> EnemyList;
typedef std::set<EnemyAnt> EnemyHillSet, EnemySet;

#endif // ANT_HPP
