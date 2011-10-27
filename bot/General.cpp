#include "General.hpp"

///////////////////
///     HQ      ///
///////////////////

void HQ::ScanForBattles() {
	return;
	// Scan over rooms for rooms where room.getNumEnemies > 0, that are not currently assigned to a BattleScene.
	// Add them to a current battlescene if they are close, or create a new one.
}


///////////////////
/// BATTLESCENE ///
///////////////////

// Constructor
BattleScene::BattleScene(std::vector<Room *> *rooms) {
	m_rooms = *rooms;
	update();
	classifyFight();
}

void BattleScene::addRoom(Room* room) {
	//m_rooms.insert(room);
}

// Run all necessery updates each turn.
void BattleScene::update() {
   int numMyrmidons       = 0,
	   numEnemies         = 0,
	   area               = 0,
	   numFallenMyrmidons = 0,
	   numFallenEnemies   = 0;

   for(std::vector<Room*>::iterator rit = m_rooms.begin(); rit != m_rooms.end(); ++rit) {
	   Room *room = *rit;

	  /* These are not yet defined.
	   numMyrmidons       += room->getNumMyrmidons();
	   numEnemies         += room->getNumEnemies();
	   numFallenMyrmidons += room->getNumMyrmidons();
	   numFallenEnemies   += room->getNumEnemies();
	   area               += room->getArea();

	   */
   }

	m_area         = area;
	m_numMyrmidons = numMyrmidons;
	m_numEnemies   = numEnemies;
	m_proportions  = (double)numMyrmidons / (double) numEnemies;

   // Fallen enemies makes history better, fallen heroes makes a poor tale.
	m_battlehistory += numFallenEnemies - numFallenMyrmidons;

   // Ofcourse, memory of historic deeds fade over time, and we shouldn't rely soley on previous merits...
	m_battlehistory -= int(m_battlehistory / 10); // -10 - (-10 / 10) = -9   and 10 - (10/10) = 9

   classifyFight();  // What type of fight this battle is, might have changed.
}

void BattleScene::classifyFight(){
	if(m_numMyrmidons + m_numEnemies < 5)
		m_fighttype = small;
	else if(m_numMyrmidons + m_numEnemies < 20)
		m_fighttype = medium;
	else
		m_fighttype = large;
}

///////////////
/// GENERAL ///
///////////////


General::General(BattleScene* battlescene) {
	m_battlescene = battlescene;
}

General::~General()
{
	// Jonglera
}

 // Execute new commands at each turn.
void General::Command() {

}
