#include "State.hpp"
#include "Tracker.hpp"
#include "Map.hpp"
#include <cassert>
#include "Ant.hpp"

#include <sstream>

using namespace std;

// initialized in main
State* g_state = NULL;


//////////////////////////

/*bool State::isOccupied(const Pos& loc) {
	return grid[loc[0]][loc[1]].ant != -1;
}*/

//////////////////////////


//constructor
State::State(std::ostream& output)
	: output(output)
{
    gameover = 0;
	turn = 0;
	ASSERT(!g_state && "more than one State constructed");

	// Random logfile name....
	std::stringstream ss;
	ss << "debug_" << rand() << ".txt";
	bug.open(ss.str());
};

//deconstructor
State::~State()
{
    bug.close();
}

//sets the state up
void State::setup() {
	g_map->initMap(m_size);
}

//resets all non-water squares to land and clears the bots ant vector


//outputs move information to the engine
void State::makeMove(const Pos &loc, int direction)
{
	output << "o " << loc[0] << " " << loc[1] << " " << CDIRECTIONS[direction] << endl;

}


/*
    This is the output function for a state. It will add a char map
    representation of the state to the output stream passed to it.

    For example, you might call "cout << state << endl;"
*/
ostream& operator<<(ostream &os, const State &state)
{
	for(int y=0; y<state.m_size.y(); y++)
    {
		for(int x=0; x<state.m_size.x(); x++)
        {
			Pos pos(x,y);

			if(g_map->square(pos).isWater)
                os << '%';
			else if(g_map->square(pos).isFood)
                os << '*';
			else if(g_map->square(pos).isHill)
				os << (char)('A' + g_map->square(pos).hillPlayer);
			else if(g_map->square(pos).ant >= 0)
				os << (char)('a' + g_map->square(pos).ant);
			else if(g_map->square(pos).isVisible)
                os << '.';
            else
                os << '?';
        }
        os << endl;
    }

    return os;
}

//input function
istream& operator>>(istream &is, State &state)
{
    int row, col, player;
    string inputType, junk;

    //finds out which turn it is
    while(is >> inputType)
    {
        if(inputType == "end")
        {
            state.gameover = 1;
            break;
        }
        else if(inputType == "turn")
        {
            is >> state.turn;
			g_tracker->turn(state.turn);
            break;
        }
        else //unknown line
            getline(is, junk);
    }

    if(state.turn == 0)
    {
        //reads game parameters
        while(is >> inputType)
        {
            if(inputType == "loadtime")
                is >> state.loadtime;
            else if(inputType == "turntime")
                is >> state.turntime;
            else if(inputType == "rows")
				is >> state.m_size.y();
            else if(inputType == "cols")
				is >> state.m_size.x();
            else if(inputType == "turns")
                is >> state.turns;
            else if(inputType == "viewradius2")
            {
                is >> state.viewradius;
                state.viewradius = sqrt(state.viewradius);
            }
            else if(inputType == "attackradius2")
            {
                is >> state.attackradius;
                state.attackradius = sqrt(state.attackradius);
            }
            else if(inputType == "spawnradius2")
            {
                is >> state.spawnradius;
                state.spawnradius = sqrt(state.spawnradius);
            }
            else if(inputType == "ready") //end of parameter input
            {
                state.timer.start();
                break;
            }
            else    //unknown line
                getline(is, junk);
        }
    }
    else
    {
        //reads information about the current turn
        while(is >> inputType)
		{
			if(inputType == "w") //water square
            {
                is >> row >> col;
				g_tracker->water(Pos(row, col));
            }
            else if(inputType == "f") //food square
            {
                is >> row >> col;
				g_tracker->food(Pos(row, col));
            }
            else if(inputType == "a") //live ant square
            {
                is >> row >> col >> player;
				g_tracker->ant(Pos(row,col),player);
            }
            else if(inputType == "d") //dead ant square
            {
                is >> row >> col >> player;
				g_tracker->deadAnt(Pos(row,col),player);
			}
            else if(inputType == "h")
            {
                is >> row >> col >> player;
				g_tracker->hill(Pos(row,col),player);
            }
            else if(inputType == "players") //player information
                is >> state.noPlayers;
            else if(inputType == "scores") //score information
            {
                state.scores = vector<double>(state.noPlayers, 0.0);
                for(int p=0; p<state.noPlayers; p++)
                    is >> state.scores[p];
            }
            else if(inputType == "go") //end of turn input
            {
				g_tracker->go();
                if(state.gameover)
                    is.setstate(std::ios::failbit);
                else
                    state.timer.start();
                break;
            }
            else //unknown line
                getline(is, junk);
        }
    }

    return is;
}


