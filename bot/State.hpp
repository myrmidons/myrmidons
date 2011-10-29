#ifndef STATE_HPP
#define STATE_HPP

#include <iostream>
#include <vector>

#include "Timer.hpp"
#include "Pos.hpp"


class Tracker;
/*
    constants
*/
const int WEST  = 0;
const int SOUTH = 1;
const int EAST  = 2;
const int NORTH = 3;
const int STAY = 4;

const int TDIRECTIONS = 4;
//const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
const char CDIRECTIONS[4] = {'W', 'S', 'E', 'N'};
const int DIRECTIONS[5][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1}, {0, 0}};      //{W, S, E, N}. And STAY

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
	Vec2 m_size;
	int
        turn, turns,
		noPlayers,
		seed;
	int viewRadius2;
    double attackradius, spawnradius, viewradius;
    double loadtime, turntime;
    std::vector<double> scores;
    bool gameover;

	Timer timer;

    /*
        Functions
    */
	explicit State(std::ostream& output_stream);
    ~State();

	void setup();

	void makeMove(const Pos &loc, int direction);

};

std::ostream& operator<<(std::ostream &os, const State &state);
std::istream& operator>>(std::istream &is, State &state);

extern State* g_state;

#endif //STATE_H_
