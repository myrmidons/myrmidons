#include "Tracker.hpp"
#include "Ant.hpp"
#include "Map.hpp"

Tracker* g_tracker = 0;

typedef enum {TheGoodGuys = 0} Team;

Tracker::Tracker()
	:m_numAnts(0) {
	m_antStorage.resize(10000); // Taken out of thin air.
}

inline size_t Tracker::indexOf(Ant* ant) const {
	return (ant - &m_antStorage[0]);
}

void Tracker::turn(int n) {
	m_turn = n;
	g_state->bug << "turn " << n << ":" << std::endl << "----------------" << std::endl;
	buf.reset();
	ASSERT(g_map);
	g_map->newTurn(n);
}

void Tracker::water(Pos const& pos) {
	ASSERT(g_map);
	g_map->water(pos);
}

void Tracker::food(Pos const& pos) {
	buf.food.push_back(pos);
}

void Tracker::ant(Pos const& pos, int team) {
	if(team != TheGoodGuys) {
		buf.enemyAnts.push_back(pos);
		buf.enemyTeams.push_back(team);
	}
	else {
		buf.myAnts.push_back(pos);
	}
}

void Tracker::deadAnt(Pos const& pos, int team) {
	if(team != TheGoodGuys) {
		buf.deadEnemies.push_back(pos);
		buf.deadEnemyTeams.push_back(team);
	}
	else {
		buf.deadAnts.push_back(pos);
	}
}

void Tracker::hill(Pos const& pos, int team) {
	if(team != TheGoodGuys) {
		buf.enemyHills.push_back(pos);
		buf.enemyHillTeams.push_back(team);
	}
	else {
		buf.myHills.push_back(pos);
	}
}

void Tracker::go() {
	update();
	g_map->updateVisionInformation();
	g_state->bug << getLiveAnts().size() << " live ants." << std::endl;
}

void Tracker::update() {

	// Water have already been reported.

	// Find free anthills.
	g_state->bug << "Looking for free ant hills...";
	PosSet freeHills;
	for(size_t i = 0; i < buf.myHills.size(); ++i) {
		Pos pos = buf.myHills[i];
		if(0 == g_map->getAnt(pos)) {
			// This hill is not occupied, and may spawn an ant.
			freeHills.insert(pos);
		}
	}
	g_state->bug << " Found " << freeHills.size() << std::endl;

	// Remove dead ants.
	for(size_t i = 0; i < buf.deadAnts.size(); ++i) {
		g_state->bug << "Dead ant at " << buf.deadAnts[i];
		Ant* pAnt = g_map->getAnt(buf.deadAnts[i]);
		if(!pAnt) {
			g_state->bug << "Not found in map :\\" << std::endl;
			continue;
		}
		g_map->removeAnt(pAnt);
		m_liveAnts.erase(pAnt);
		int j = indexOf(pAnt);
		g_state->bug << ", index " << j << "." << std::endl;
		m_deadIndices.insert(j);
	}

	// Spawn new ants.
	for(PosSet::iterator hill = freeHills.begin(); hill != freeHills.end(); ++hill) {
		g_state->bug << "Trying to check square " << *hill << " in map of dimensions " << g_map->size() << std::endl;
		if(g_map->square(*hill).ant == 0) {
			// There is a new ant on this hill!! Horray!
			IndexSet::iterator it = m_deadIndices.begin();
			size_t i = 0;
			if(it != m_deadIndices.end()) {
				// Our new ant replaces a fallen hero.
				i = *it;
				m_deadIndices.erase(it);
				g_state->bug << "Spawning with reused index " << i << "." << std::endl;
			}
			else {
				// Hurray! Our army grows!
				i = m_numAnts++;
				g_state->bug << "Spawning with new index " << i << "." << std::endl;
			}

			Ant* newAnt = &(m_antStorage[i] = Ant(*hill));
			g_map->addAnt(newAnt);
			m_liveAnts.insert(newAnt);
			g_state->bug << "Ant spawned at " << *hill << std::endl;
		}
	}
}

void Tracker::StateBuffer::reset() {
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

AntSet const& Tracker::getLiveAnts() {
	return m_liveAnts;
}
