#include "Tracker.hpp"
#include "Ant.hpp"
#include "Map.hpp"
#include "Util.hpp"
#include <sstream>
#include <sys/time.h>
#include <set>

Tracker* g_tracker = 0;

typedef enum {Myrmidons = 0} Team;

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

void Tracker::beginTurnInput(int n) {
	m_turn = n;
	TRACKER_LOG("turn " << n << ":" << std::endl << "----------------");
	buf.resetDynamics();
	ASSERT(g_map);
	g_map->resetDynamics();
}

void Tracker::bufferWater(Pos const& pos) {
	buf.water.push_back(pos);
}

void Tracker::bufferFood(Pos const& pos) {
	buf.food.insert(pos);
}

void Tracker::bufferAnt(Pos const& pos, int team) {
	if(team != Myrmidons) {
		buf.enemyAnts.insert(EnemyAnt(pos,team));
	}
	else {
		buf.myAnts.push_back(pos);
	}
}

void Tracker::bufferDeadAnt(Pos const& pos, int team) {
	if(team != Myrmidons) {
		buf.deadEnemyAnts.insert(EnemyAnt(pos,team));
	}
	else {
		buf.deadAnts.push_back(pos);
	}
}

// Track the hills and remember which ones we have already seen.
// buffer the new ones for reporting when the maps visuals have been updated,
// i.e. until the newly discovered hill-position have valid room-affinity.
void Tracker::bufferHill(Pos const& pos, int team) {
	TRACKER_LOG("bufferHill(" << pos << ", " << team << ")");
	if(team != Myrmidons) {
		if(buf.enemyHills.find(pos) == buf.enemyHills.end()) {
			TRACKER_LOG("Discovered new enemy hill for team " << team << " at " << pos);
			buf.newEnemyHills.insert(EnemyHill(pos,team));
			buf.enemyHills.insert(pos); // Keep forever
		}
	}
	else {
		if(buf.myHills.find(pos) == buf.myHills.end()) {
			TRACKER_LOG("Discovered that we have a hill at " << pos);
			buf.newHills.insert(pos);
			buf.myHills.insert(pos); // Keep forever
		}
	}
}

void Tracker::endTurnInput() {

	STAMP("Before update");
	updateMapInfo();
	STAMP("After update");

	TRACKER_LOG(getAnts().size() << " live ants.");
}

void Tracker::updateMapInfo() {
	TRACKER_LOG("Reporting new water to map.");
	IT(PosList, it, buf.water) {
		g_map->addWater(*it);
	}
	TRACKER_LOG(buf.water.size() << " water cells reported.");


	TRACKER_LOG("Updating visual information");
	g_map->updateVisionInformation(buf.myAnts);


	TRACKER_LOG("Reporting new myrmidon hills to map.");
	IT(PosSet, it, buf.newHills) {
		g_map->addHill(*it);
	}
	TRACKER_LOG(buf.newHills.size() << " hills reported.");


	TRACKER_LOG("Reporting new enemy hills to map.");
	IT(EnemySet, it, buf.newEnemyHills) {
		g_map->addEnemyHill(*it);
	}
	TRACKER_LOG(buf.newEnemyHills.size() << " hills reported.");


	TRACKER_LOG("Reporting food to map.");
	IT(PosSet, it, buf.food) {
		g_map->addFood(*it);
	}
	TRACKER_LOG(buf.food.size() << " food items reported.");


	TRACKER_LOG_("Looking for free ant hills...");
	PosSet freeHills;
	ITC(PosSet, it, buf.myHills) {
		Pos pos = *it;
		if(0 == g_map->getAntAt(pos)) {
			// This hill is not occupied, and may spawn an ant.
			freeHills.insert(pos);
		}
	}
	TRACKER_LOG(" Found " << freeHills.size());

	TRACKER_LOG("Reporting dead ants.");
	// Remove dead ants.
	for(size_t i = 0; i < buf.deadAnts.size(); ++i) {
		TRACKER_LOG_("Dead ant at " << buf.deadAnts[i]);
		Ant* pAnt = g_map->getAntAt(buf.deadAnts[i]);
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

	TRACKER_LOG("Spawning dead ants.");

	// Set ant positions in map.
	IT(PosList, it, buf.myAnts) {
		g_map->square(*it).ant = 0;
	}

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
	food.clear();
	water.clear();
	myAnts.clear();
	deadAnts.clear();
	enemyAnts.clear();
	deadEnemyAnts.clear();

	newEnemyHills.clear();
	newHills.clear();
}

AntSet& Tracker::getAnts() {
	return m_liveAnts;
}

EnemySet& Tracker::getEnemies() {
	return buf.enemyAnts;
}

PosSet& Tracker::getFood() {
	return buf.food;
}
