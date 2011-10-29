#include "Tracker.hpp"
#include "Ant.hpp"
#include "Map.hpp"
#include "Util.hpp"
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
	buf.resetDynamics();
	g_map->resetDynamics();
	ASSERT(g_map);
}

void Tracker::water(Pos const& pos) {
	ASSERT(g_map);
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
	if(team != TheGoodGuys) {
		buf.deadEnemies.push_back(pos);
		buf.deadEnemyTeams.push_back(team);
	}
	else {
		buf.deadAnts.push_back(pos);
	}
}

// Track the hills and remember which ones we have already seen.
// buffer the new ones for reporting when the maps visuals have been updated,
// i.e. until the newly discovered hill-position have valid room-affinity.
void Tracker::hill(Pos const& pos, int team) {

	if(team != TheGoodGuys) {
		if(m_enemyHills.find(pos) != m_enemyHills.end()) {
			// This is a newly discovered anemy hill!
			m_enemyHills.insert(pos);
			TRACKER_LOG("Enemy hill for team " << team << " at " << pos);
			buf.newEnemyHills.insert(Buffer::EnemyHill(pos,team));
		}
	}
	else {
		if(m_hills.find(pos) != m_hills.end()) {
			// This is a new hill, horray!
			m_hills.insert(pos);
			TRACKER_LOG("We have a hill at " << pos);
			buf.newHills.insert(pos);
		}
		buf.myHills.push_back(pos);
	}
}

void Tracker::go() {
	STAMP("Before updateVisualInformation");
	g_map->updateVisionInformation(buf.myAnts);

	STAMP("Before update");
	update();
	STAMP("After update");

	TRACKER_LOG(getLiveAnts().size() << " live ants.");
}

void Tracker::update() {
	STAMP("Tracker::update");
	// Water have already been reported.

	STAMP_;
	// Report myrmidon hills to map.
	IT(Buffer::EnemyHillSet, it, buf.newEnemyHills) {
		g_map->enemyHill(it->first, it->second);
	}

	STAMP_;
	// Report enemy hills to map.
	IT(PosSet, it, buf.newHills) {
		g_map->hill(*it);
	}

	STAMP_;
	// Report food to map
	IT(PosVec, it, buf.food) {
		g_map->food(*it);
	}

	STAMP_;
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

	STAMP_;
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

	STAMP_;
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

void Tracker::Buffer::resetDynamics() {
	STAMP("Tracker::Buffer::resetDynamics");
	myAnts.clear();
	enemyAnts.clear();
	myHills.clear();
	food.clear();
	deadAnts.clear();
	deadEnemies.clear();
	enemyTeams.clear();
	deadEnemyTeams.clear();
}

AntSet const& Tracker::getLiveAnts() {
	return m_liveAnts;
}

const PosList& Tracker::getAllFood() const {
	return this->buf.food; // FIXME: Mattias - is this right?
}
