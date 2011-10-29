#include "Tracker.hpp"
#include "Ant.hpp"
#include "Map.hpp"
#include "Util.hpp"
#include "Logger.hpp"
#include "Room.hpp"
#include <sstream>
#include <sys/time.h>
#include <set>

Tracker* g_tracker = 0;

typedef enum {Myrmidons = 0} Team;

Tracker::Tracker() {
	std::stringstream ss;
	ss << "d_tracker_" << (rand()+clock()) << ".txt";
	log.open(ss.str());
}

void Tracker::beginTurnInput(int n) {
	m_turn = n;
	Logger::instance()->newTurn(n); // Start new logfile
	LOG_DEBUG("turn " << n << ":" << std::endl << "----------------");

	// Reset dynamic content (ants, food) so we can fill it in from scratch
	buf.resetDynamics();
	g_map->resetDynamics();
	g_rooms->resetDynamics();
}

void Tracker::bufferWater(Pos const& pos) {
	buf.water.push_back(pos);
}

void Tracker::bufferFood(Pos const& pos) {
	buf.food.insert(pos);
}

void Tracker::bufferAnt(Pos const& pos, int team) {
	LOG_DEBUG("Live ant at " << pos << ", team " << team);

	if(team != Myrmidons) {
		buf.enemyAnts.insert(EnemyAnt(pos,team));
	}
	else {
		buf.myAnts.push_back(pos);
	}
}

void Tracker::bufferDeadAnt(Pos const& pos, int team) {
	LOG_DEBUG("Dead ant at " << pos << ", team " << team);

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
	LOG_TRACKER("bufferHill(" << pos << ", " << team << ")");
	if(team != Myrmidons) {
		if(buf.enemyHills.find(pos) == buf.enemyHills.end()) {
			LOG_TRACKER("Discovered new enemy hill for team " << team << " at " << pos);
			buf.newEnemyHills.insert(EnemyHill(pos,team));
			buf.enemyHills.insert(pos); // Keep forever
		}
	}
	else {
		if(buf.myHills.find(pos) == buf.myHills.end()) {
			LOG_TRACKER("Discovered that we have a hill at " << pos);
			buf.newHills.insert(pos);
			buf.myHills.insert(pos); // Keep forever
		}
	}
}

void Tracker::endTurnInput() {
	if (g_state->gameover)
		return;

	STAMP("Before update");
	updateMapInfo();
	STAMP("After update");

	ASSERT(buf.myAnts.size() == getAnts().size());
	LOG_TRACKER(getAnts().size() << " live ants.");
}

void Tracker::updateMapInfo() {
	LOG_TRACKER("Reporting new water to map.");
	IT(PosList, it, buf.water) {
		g_map->addWater(*it);
	}
	LOG_TRACKER(buf.water.size() << " water cells reported.");


	LOG_TRACKER("Updating visual information");
	g_map->updateVisionInformation(buf.myAnts);


	LOG_TRACKER("Reporting new myrmidon hills to map.");
	IT(PosSet, it, buf.newHills) {
		g_map->addHill(*it);
	}
	LOG_TRACKER(buf.newHills.size() << " hills reported.");


	LOG_TRACKER("Reporting new enemy hills to map.");
	IT(EnemySet, it, buf.newEnemyHills) {
		g_map->addEnemyHill(*it);
	}
	LOG_TRACKER(buf.newEnemyHills.size() << " hills reported.");


	LOG_TRACKER("Reporting food to map.");
	IT(PosSet, it, buf.food) {
		g_map->addFood(*it);
	}
	LOG_TRACKER(buf.food.size() << " food items reported.");


	TRACKER_LOG_("Looking for free ant hills...");
	PosSet freeHills;
	ITC(PosSet, it, buf.myHills) {
		Pos pos = *it;
		if(0 == g_map->getAntAt(pos)) {
			// This hill is not occupied, and may spawn an ant.
			freeHills.insert(pos);
		}
	}
	LOG_TRACKER(" Found " << freeHills.size());

	/////////////////////////////////////////////////////

	updateAnts();
}

void Tracker::updateAnts() {
	// Remove ants, and re-add them later if still alive. This ensures consistent dynamic data (ant, team, etc).
	ITC(AntSet, ait, m_ants)
		g_map->removeAnt(*ait);

	// Our ants at buf.myAnts now. Update our m_ants by: moving, killing, spawning.
	PosSet unaccountedAntPos; // will be shrunk as the position is set to a new ant.
	unaccountedAntPos.insert(buf.myAnts.begin(), buf.myAnts.end());
	AntSet doneAnts;

	// Check which ants got to go where they wanted:
	ITC(AntSet, ait, m_ants) {
		Ant* ant = *ait;
		Pos expected = ant->expectedPos();
		if (unaccountedAntPos.count(expected)) {
			// There is an ant where this ant went. It must be this ant.
			// Update map/room info about ant.
			ant->setPos(expected);
			g_map->addAnt(ant);

			// Mark position and ant as "tracked":
			unaccountedAntPos.erase(expected);
			doneAnts.insert(ant);
		}
	}

	if (doneAnts.size() != m_ants.size()) {
		// Not all ants got what they wanted.
		LOG_TRACKER((m_ants.size()-doneAnts.size()) << " did not get to go where they wanted.");

		ITC(AntSet, ait, m_ants) {
			if (doneAnts.count(*ait))
				continue;
			Ant* ant = *ait;
			if (unaccountedAntPos.count(ant->pos())) {
				LOG_TRACKER("Ant found at old (expected) positions - its movement was probably denied.");

				// Update dynamic data (team etc)
				g_map->addAnt(ant);

				// Mark as "tracked"
				unaccountedAntPos.erase(ant->pos());
				doneAnts.insert(ant);
			}
		}

		if (doneAnts.size() < m_ants.size()) {
			// Ants not where expected, or in old pos - they must be dead!

			AntSet killSet;
			ITC(AntSet, ait, m_ants)
				if (!doneAnts.count(*ait))
					killSet.insert(*ait);

			LOG_TRACKER("Tracked found " << killSet.size() << " MIA");

			if (killSet.size() != buf.deadAnts.size()) {
				LOG_ERROR("Tracked dead ants did not match buf.deadAnts, of size " << buf.deadAnts.size());
			}

			ITC(AntSet, ait, killSet) {
				Ant* ant = *ait;
				// Dead ant
				LOG_TRACKER("Killing ant at " << ant->pos() << " (expected " << ant->expectedPos() << ")");
				m_ants.erase(ant);
				delete ant; // RIP.
			}
		}
	}

	if (!unaccountedAntPos.empty()) {
		// Still positions not matched to any ant? Must be new ants! Hooray!
		LOG_TRACKER("Tracker found " << unaccountedAntPos.size() << " new ants!");
		ITC(PosSet, pit, unaccountedAntPos) {
			LOG_TRACKER("New ant at " << *pit);
			Ant* ant = new Ant(*pit);
			m_ants.insert(ant);
			g_map->addAnt(ant);
		}
		unaccountedAntPos.clear(); // All accounted for.
	}

	ASSERT(buf.myAnts.size() == m_ants.size());

	/////////////////////////////////////////////////////

	// Set ant teams (reset by resetDynamics)
	IT(PosList, it, buf.myAnts)
		g_map->square(*it).antTeam = 0;

	ITC(EnemySet, eit, buf.enemyAnts)
		g_map->square(eit->pos).antTeam = eit->team;
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
	return m_ants;
}

const EnemySet& Tracker::getEnemies() const {
	return buf.enemyAnts;
}

const PosSet& Tracker::getFood() const {
	return this->buf.food;
}
