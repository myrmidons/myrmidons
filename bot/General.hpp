#ifndef GENERAL_H
#define GENERAL_H

#include <vector>
#include "Room.hpp"

#endif // GENERAL_H


enum FightType {unknown, small, medium, large};

class HQ {

public:
	HQ();
	~HQ();
	void ScanForBattles();
private:

};

///////////////////
/// BATTLESCENE ///
///////////////////

class BattleScene {
public:
	BattleScene(std::vector<Room *> *rooms);
	void update(); // Run each turn.
	void addRoom(Room* room);

private:
	std::vector<Room*> m_rooms;
	void classifyFight();

	int m_battlehistory;  // Enemy death increases history by 1, allied death decreases history by 1.
	double m_proportions; // numEnemigos/numAmigos
	double m_crowdedness; // total room area divided by total battle participants
	int m_area;
	int m_numMyrmidons;
	int m_numEnemies;
	FightType m_fighttype;
};


///////////////
/// GENERAL ///
///////////////

class General {

public:
    General(BattleScene* battlescene);
    ~General();
    void Command(); // Execute new commands at each turn.

private:
    BattleScene* m_battlescene;

};




