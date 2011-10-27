#ifndef STATE_HPP
#define STATE_HPP

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <stack>

#include "Timer.hpp"
#include "Bug.hpp"
#include "Square.hpp"
#include "Pos.hpp"


class Identifier;
/*
    constants
*/
const int TDIRECTIONS = 4;
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}

typedef std::vector<int> DirVec;

/*
    struct to store current state information
*/
struct State
{
	std::ostream& output; // sinks simulation commands for the turn
    /*
        Variables
    */
    int rows, cols,
        turn, turns,
        noPlayers;
    double attackradius, spawnradius, viewradius;
    double loadtime, turntime;
    std::vector<double> scores;
    bool gameover;

    std::vector<std::vector<Square> > grid;
	std::vector<Pos> myAnts, enemyAnts, myHills, enemyHills, food, deadAnts, deadEnemies;
	std::vector<int> enemyTeams, enemyDeadTeams;

	Identifier* identifier;

    Timer timer;
    Bug bug;

    /*
        Functions
    */
	explicit State(std::ostream& output_stream);
    ~State();

    void setup();
    void reset();

	void makeMove(const Pos &loc, int direction);

	double distance(const Pos &loc1, const Pos &loc2);
	Pos getLocation(const Pos &startLoc, int direction);

	bool isOccupied(const Pos& loc);

    void updateVisionInformation();

	Square& square(Pos const& pos) { return grid[pos[0]][pos[1]]; }
};

std::ostream& operator<<(std::ostream &os, const State &state);
std::istream& operator>>(std::istream &is, State &state);

extern State* g_state;
#endif //STATE_H_
