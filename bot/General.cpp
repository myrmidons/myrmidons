#include "General.hpp"


///////////////////
/// BATTLESCENE ///
///////////////////


BattleScene::BattleScene(std::vector<Room *> *rooms) {
    this->rooms = *rooms;
    battlehistory = 0;
    proportions = calculateProportions();
    crowdedness = calculateCrowdedness();
}

int BattleScene::updateBattleHistory() {
    return 0;
}

double BattleScene::calculateProportions() {

   // int numMyrmidons = 0, numEnemies = 0;

    for(std::vector<Room*>::iterator rit = rooms.begin(); rit != rooms.end(); ++rit)
    {
        //Room *room = *rit;
        //numMyrmidons += room->getNumMyrmidons();
        //numEnemies   += room->getNumEnemies();
    }


    return 42.0; //(double)numMyrmidons / (double) numEnemies;
}

double BattleScene::calculateCrowdedness() {
    return 0.0;
}


// Run all necessery updates each turn.
void BattleScene::Update() {
   battlehistory = updateBattleHistory();
   proportions  = calculateProportions();
   crowdedness = calculateCrowdedness();
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
