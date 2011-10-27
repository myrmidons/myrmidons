#include "Ant.hpp"
#include "Map.hpp"
#include <iostream>
#include <map>



AntIdentifier::AntIdentifier()
	: m_numAnts(0), m_map(new Map()) {
	m_antStorage.resize(1000); /*taken out of thin air*/
}

void AntIdentifier::update(State& state) {
	// Locate hills able to spawn, must be done before dead ants are removed.
	//
	// FLAG: Think this through one more time!
	//

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
		state.bug << "Dead ant at " << state.deadAnts[i] << std::endl;
		Ant* pAnt = m_map->getAnt(state.deadAnts[i]);
		if(!pAnt) {
			state.bug << "Not found in map :\\" << std::endl;
			continue;
		}
		m_liveAnts.erase(pAnt);
		m_map->removeAnt(pAnt);
		int j = indexOf(pAnt);
		m_deadIndices.insert(j);
	}

	// Posibly spawn new ants.
	for(PosSet::iterator hill = freeHills.begin(); hill != freeHills.end(); ++hill) {
		if(state.square(*hill).ant == 0) {
			// There is a new ant on this hill!! Horray!
			state.bug << "Ant spawned at " << *hill << std::endl;
			IndexSet::iterator it = m_deadIndices.begin();
			size_t i = 0;
			if(it != m_deadIndices.end()) {
				// Our new ant replaces a fallen hero.
				i = *it;
				m_deadIndices.erase(it);
			}
			else {
				// Hurray! Our army grows!
				i = m_numAnts++;
			}

			m_map->addAnt(&(m_antStorage[i] = Ant(*hill)));
		}
	}
}

Ant::Ant(const Pos &loc)
	: m_position(loc) {
}

Pos& Ant::pos() {
	return m_position;
}
