#include "State.hpp"
#include "Identifier.hpp"
#include "Map.hpp"
#include <cassert>
#include "Ant.hpp"

using namespace std;

// initialized in main
State* g_state = NULL;


//constructor
State::State(std::ostream& output)
	: output(output)
{
	identifier = new Identifier();
    gameover = 0;
	turn = 0;
	ASSERT(!g_state && "more than one State constructed");
	bug.open("debug.txt");
};

//deconstructor
State::~State()
{
    bug.close();
}

//sets the state up
void State::setup()
{
    grid = vector<vector<Square> >(rows, vector<Square>(cols, Square()));
	g_map->initMap(rows, cols);
}

//resets all non-water squares to land and clears the bots ant vector
void State::reset()
{
	for(int row=0; row<rows; row++)
		for(int col=0; col<cols; col++)
			if(!grid[row][col].isWater)
				grid[row][col].reset();
}

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
    for(int row=0; row<state.rows; row++)
    {
        for(int col=0; col<state.cols; col++)
        {
            if(state.grid[row][col].isWater)
                os << '%';
            else if(state.grid[row][col].isFood)
                os << '*';
            else if(state.grid[row][col].isHill)
                os << (char)('A' + state.grid[row][col].hillPlayer);
            else if(state.grid[row][col].ant >= 0)
                os << (char)('a' + state.grid[row][col].ant);
            else if(state.grid[row][col].isVisible)
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
			state.identifier->turn(state.turn);
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
                is >> state.rows;
            else if(inputType == "cols")
                is >> state.cols;
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
				state.identifier->water(Pos(row, col));
				state.grid[row][col].isWater = 1;
            }
            else if(inputType == "f") //food square
            {
                is >> row >> col;
				state.identifier->food(Pos(row, col));
                state.grid[row][col].isFood = 1;
//				state.food.push_back(Pos(row, col));
            }
            else if(inputType == "a") //live ant square
            {
                is >> row >> col >> player;
				state.identifier->ant(Pos(row,col),player);
				state.grid[row][col].ant = player;
/*				if(player == 0) {
					state.myAnts.push_back(Pos(row, col));
				}
				else {
					state.enemyAnts.push_back(Pos(row, col));
					state.enemyTeams.push_back(player);

				}
				*/
            }
            else if(inputType == "d") //dead ant square
            {
                is >> row >> col >> player;
				state.identifier->deadAnt(Pos(row,col),player);
				state.grid[row][col].deadAnts.push_back(player);
				/*if(player == 0) {
					state.deadAnts.push_back(Pos(row, col));
				}
				else {
					state.deadEnemies.push_back(Pos(row, col));
					state.enemyDeadTeams.push_back(player);

				}*/
            }
            else if(inputType == "h")
            {
                is >> row >> col >> player;
				state.identifier->hill(Pos(row,col),player);
                state.grid[row][col].isHill = 1;
				state.grid[row][col].hillPlayer = player;
				/*if(player == 0)
					state.myHills.push_back(Pos(row, col));
                else
					state.enemyHills.push_back(Pos(row, col));*/

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
				state.identifier->go();
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


