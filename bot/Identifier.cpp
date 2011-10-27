#include "Identifier.hpp"
#include "Ant.hpp"

// Just replace with Map.hpp when the time is ripe. (Also in Bot.cpp)
#include "chixdummymap.hpp"

Identifier::Identifier()
	:m_numAnts(0), m_map(new Map()){
	m_antStorage.resize(10000); // Taken out of thin air.
}

inline size_t Identifier::indexOf(Ant* ant) const { return (ant - &m_antStorage[0]); }


void Identifier::turn(int n) {
	m_turn = n;
	g_state.bug << "turn " << n << ":" << std::endl << "----------------" << std::endl;
	buf.reset();
}

void Identifier::water(Pos const& pos) {
//	m_map->water(pos);
}

void Identifier::food(Pos const& pos) {
	buf.food.push_back(pos);
}

void Identifier::ant(Pos const& pos, int team) {
	if(team) {
		buf.enemyAnts.push_back(pos);
		buf.enemyTeams.push_back(team);
	}
	else {
		buf.myAnts.push_back(pos);
	}
}

void Identifier::deadAnt(Pos const& pos, int team) {
	if(team) {
		buf.deadEnemies.push_back(pos);
		buf.deadEnemyTeams.push_back(team);
	}
	else {
		buf.deadAnts.push_back(pos);
	}
}

void Identifier::hill(Pos const& pos, int team) {
	if(team) {
		buf.enemyHills.push_back(pos);
		buf.enemyHillTeams.push_back(team);
	}
}

void Identifier::go() {
	update(g_state);
}


void Identifier::update(State& state) {

	// Find free anthills.
	state.bug << "Looking for free ant hills...";
	PosSet freeHills;
	for(size_t i = 0; i < state.myHills.size(); ++i) {
		Pos pos = state.myHills[i];
		if(0 == m_map->getAnt(pos)) {
			// This hill is not occupied, and may spawn an ant.
			freeHills.insert(pos);
		}
	}
	state.bug << " Found " << freeHills.size() << std::endl;

	// Remove dead ants.
	for(size_t i = 0; i < state.deadAnts.size(); ++i) {
		state.bug << "Dead ant at " << state.deadAnts[i];
		Ant* pAnt = m_map->getAnt(state.deadAnts[i]);
		if(!pAnt) {
			state.bug << "Not found in map :\\" << std::endl;
			continue;
		}
		m_map->removeAnt(pAnt);
		m_liveAnts.erase(pAnt);
		int j = indexOf(pAnt);
		state.bug << ", index " << j << "." << std::endl;
		m_deadIndices.insert(j);
	}

	// Spawn new ants.
	for(PosSet::iterator hill = freeHills.begin(); hill != freeHills.end(); ++hill) {
		if(state.square(*hill).ant == 0) {
			// There is a new ant on this hill!! Horray!
			IndexSet::iterator it = m_deadIndices.begin();
			size_t i = 0;
			if(it != m_deadIndices.end()) {
				// Our new ant replaces a fallen hero.
				i = *it;
				m_deadIndices.erase(it);
				state.bug << "Spawning with reused index " << i << "." << std::endl;
			}
			else {
				// Hurray! Our army grows!
				i = m_numAnts++;
				state.bug << "Spawning with new index " << i << "." << std::endl;
			}

			m_map->addAnt(&(m_antStorage[i] = Ant(*hill)));
			state.bug << "Ant spawned at " << *hill << std::endl;
		}
	}
}

void Identifier::StateBuffer::reset() {
	myAnts.clear();
	enemyAnts.clear();
	myHills.clear();
	enemyHills.clear();
	food.clear();
	deadAnts.clear();
	deadEnemies.clear();
	enemyTeams.clear();
	deadEnemyTeams.clear();
	enemyHillTeams.clear();
}
