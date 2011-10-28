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

void BattleScene::addRoom(Room* /*room*/) {
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
	   //Room *room = *rit;

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

FightType BattleScene::getFightType() {
	return m_fighttype;
}

double BattleScene::getProportions() {
	return m_proportions;
}

double BattleScene::getCrowdedness() {
	return m_proportions;
}

int BattleScene::getBattleHistory() {
	return m_battlehistory;
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
	if(m_battlescene->getFightType() == small) { // Brute force calculating
		// Gather all possible move-combinations (both Myrmidons and enemies)
		// We doesn't see the enemys move, before we have to choose our own.
		// This forces us to make educated guesses,

		// Example:

		// Myrmidons  | Enemy  | Evaluation after moves, from myrmidon
		// Move       | Move   | perspective (positive is good).
		// ----------------------------------
		// A          | A       |  0
		// A          | B       | -1
		// A          | C       |  8

		// B          | A       |  2
		// B          | B       | -1
		// B          | C       |  5

		// C          | A       |  10
		// C          | B       |  10
		// C          | C       | -5

		// How to determine a usefull criteria to pick the most sound move?
		// Picking 'C' as myrmidon, gets you +10 points, if the enemy does't pick 'C' as well.
		// If the enemy does indeed pick 'C', myrmidons lose 5 points.

		// However, from the enemys standpoint, picking 'C', leads to either 5 points, or losing 8 points if
		// Myrmidons pick A, and lose 5 points if myrmidons pick B.

		// Picking 'C' as the enemy, does seem risky. So can myrmidons assume that the enemy chooses A or B, and therefor
		// safely pick 'C'?

		// What if the enemy also comes to this conclusion, and therefor does indeed pick 'C'!?

		// Doublefool, tripplefool...


		// Is there some way to group all results together, so that we can get somewhat reaonable heuristics on all move-sets available to us?
		// F.ex Choosing 'A' as myrmidon, has utility (0 + (-1) + 8)/3 = 3. 'B' = (2-1+5)/3 = 2, 'C' = (10+10-5)/3 = 5.

		// Is looking as these heuristic, and making a choice from this a sound idea?
		// How about some heuristic that depends on the battle-history and other parameters, combined with the result of the resulting position?

		// Let the heuristic multiply the expected wins with the battle history, such that a general with high berserkervalue is more likely to
		// Do reckless manuvers?
		// If the odds are against us, choose more konservetive moves, that aim toward such a safe small loss as possible?


	}
	else if(m_battlescene->getFightType() == medium) { // ???

	}
	else { // Large, initialize clever boiding behaviour.
		// 1) Identify:
		//  -protectee (what to protect?).
		//  -mass centre of participant myrmidons.
		//  -mass centre of participant enemies.

		// 2) Determine if each of these three points should repell the myrmidons, or attract them.
		//	i.e Determine the force (negative or possitive) that each of the 3 points interacts with the myrmidons with.

		// 3) Give the ants instructions to move around, Taking the forces into account.
		//	this could probably benefit from some random noise, and possibly also include other parameters.
	}

}
