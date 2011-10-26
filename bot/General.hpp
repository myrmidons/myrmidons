#ifndef GENERAL_H
#define GENERAL_H

#include <vector>
#include "Room.hpp"

#endif // GENERAL_H

class HQ {

public:
    HQ();
    ~HQ();
    void ScanForBattles();
private:

};

class BattleScene {

public:
    BattleScene(std::vector<Room *> *rooms);
    int battlehistory;  // Enemy death increases history by 1, allied death decreases history by 1.
    double proportions; // numEnemigos/numAmigos
    double crowdedness; // total room area divided by total battle participants.
    void Update(); // Run each turn.
private:
    std::vector<Room*> rooms;
    int updateBattleHistory();
    double calculateProportions();
    double calculateCrowdedness();

};

class General {

public:
    General(BattleScene* battlescene);
    ~General();
    void Command(); // Execute new commands at each turn.

private:
    BattleScene* m_battlescene;

};




