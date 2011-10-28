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


class Tracker;
/*
    constants
*/
const int TDIRECTIONS = 4;
//const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
const char CDIRECTIONS[4] = {'W', 'S', 'E', 'N'};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{W, S, E, N}

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
    Bug bug;

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

#ifdef DEBUG
#	define LOG_DEBUG(msg) g_state->bug << msg << std::endl << std::flush
#	define STAMP_BODY "STAMP: Function " << __FUNCTION__ << " in file " __FILE__ << " on row " <<  __LINE__
#	define STAMP_ LOG_DEBUG(STAMP_BODY)
#	define STAMP(foo) LOG_DEBUG(STAMP_BODY << ": " << foo)
#else
#	define LOG_DEBUG(msg)
#	define STAMP_
#	define STAMP(foo)
#endif

#endif //STATE_H_
