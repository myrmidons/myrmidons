#include "Tracker.hpp"
#include "Ant.hpp"
#include "Map.hpp"
#include <sstream>
#include <sys/time.h>

Tracker* g_tracker = 0;

typedef enum {TheGoodGuys = 0} Team;

Tracker::Tracker()
	:m_numAnts(0) {
	m_antStorage.resize(10000); // Taken out of thin air.

	std::stringstream ss;
	ss << "d_tracker_" << (rand()+clock()) << ".txt";
	log.open(ss.str());
}

inline size_t Tracker::indexOf(Ant* ant) const {
	return (ant - &m_antStorage[0]);
}

void Tracker::turn(int n) {
	m_turn = n;
	TRACKER_LOG("turn " << n << ":" << std::endl << "----------------");
	buf.reset();
	ASSERT(g_map);
	g_map->newTurn(n);
}

void Tracker::water(Pos const& pos) {
	ASSERT(g_map);
	g_map->square(pos).isWater = true;
	g_map->water(pos);
}

void Tracker::food(Pos const& pos) {
	buf.food.push_back(pos);
}

void Tracker::ant(Pos const& pos, int team) {
	ASSERT(g_map);
	g_map->square(pos).ant = team;
	if(team != TheGoodGuys) {
		buf.enemyAnts.push_back(pos);
		buf.enemyTeams.push_back(team);
	}
	else {
		buf.myAnts.push_back(pos);
	}
}

void Tracker::deadAnt(Pos const& pos, int team) {
	TRACKER_LOG("deadAnt(" << pos << ',' << team << ")");
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
	TRACKER_LOG(getLiveAnts().size() << " live ants.");
}

void Tracker::update() {

	// Water have already been reported.

	// Find free anthills.
	TRACKER_LOG_("Looking for free ant hills...");
	PosSet freeHills;
	for(size_t i = 0; i < buf.myHills.size(); ++i) {
		Pos pos = buf.myHills[i];
		if(0 == g_map->getAnt(pos)) {
			// This hill is not occupied, and may spawn an ant.
			freeHills.insert(pos);
		}
	}
	TRACKER_LOG(" Found " << freeHills.size());

	// Remove dead ants.
	for(size_t i = 0; i < buf.deadAnts.size(); ++i) {
		TRACKER_LOG_("Dead ant at " << buf.deadAnts[i]);
		Ant* pAnt = g_map->getAnt(buf.deadAnts[i]);
		if(!pAnt) {
			TRACKER_LOG("Not found in map :\\");
			continue;
		}
		g_map->removeAnt(pAnt);
		m_liveAnts.erase(pAnt);
		int j = indexOf(pAnt);
		TRACKER_LOG(", index " << j << ".");
		m_deadIndices.insert(j);
	}

	// Spawn new ants.
	for(PosSet::iterator hill = freeHills.begin(); hill != freeHills.end(); ++hill) {
		if(g_map->square(*hill).ant == 0) {
			// There is a new ant on this hill!! Horray!
			IndexSet::iterator it = m_deadIndices.begin();
			size_t i = 0;
			if(it != m_deadIndices.end()) {
				// Our new ant replaces a fallen hero.
				i = *it;
				m_deadIndices.erase(it);
				TRACKER_LOG("Spawning with reused index " << i << ".");
			}
			else {
				// Hurray! Our army grows!
				i = m_numAnts++;
				TRACKER_LOG("Spawning with new index " << i << ".");
			}

			Ant* newAnt = &(m_antStorage[i] = Ant(*hill));
			g_map->addAnt(newAnt);
			m_liveAnts.insert(newAnt);
			TRACKER_LOG("Ant spawned at " << *hill);
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
