#include "Bot.hpp"
#include "Ant.hpp"
#include "Map.hpp"
#include "Identifier.hpp"

#include <algorithm>
#include <iostream>

/// Kc testar


using namespace std;

//constructor
Bot::Bot(IODevice& io_device)
	: io(io_device)
	, state(*g_state)
{
	setupRandomDirections();
}

void Bot::setupRandomDirections() {
	DirVec dirs(4);
	dirs[0] = 0;
	dirs[1] = 1;
	dirs[2] = 2;
	dirs[3] = 3;

	do {
		dirVecs.push_back(dirs);
	} while(std::next_permutation(dirs.begin(), dirs.end()));
}

int Bot::rankMove(Pos const& currentLoc, int dir) {
	// The new location to try to get to.
	Pos newLoc = state.getLocation(currentLoc, dir);

	int rank = 0;


	if(!safeLocation(newLoc) || state.isOccupied(newLoc))
		return -1000; // Absolutley not, it would be suicide!

	for(int j = 0; j < (int)state.enemyAnts.size(); ++j) {
		double d = state.distance(newLoc,state.enemyAnts[j]);
		if(int(d*d + 0.5) < 13)
			rank -= 20;
	}



	int foodLocationIndex = closestLocation(currentLoc, state.food);
	if(foodLocationIndex >= 0) {
			Pos foodLoc = state.food[foodLocationIndex];
			if(state.distance(newLoc, foodLoc) >= state.distance(currentLoc, foodLoc)) {
				//rank -= 10; // this move would take us further away from the nearest food item.
			}
			else {
				rank += 10; // This move would take us closer to the nearest food item.
			}
	}


	return rank;
}

DirVec const& Bot::randomDirVec() const {
	return dirVecs[rand()%dirVecs.size()];
}
//plays a single game of Ants.
void Bot::playGame() {
	//reads the game parameters and sets up
	io.bufferInputChunk();
	io.input() >> state;
    state.setup();
    endTurn();

	// continues making moves while the game is not over
	while(io.bufferInputChunk() && io.input() >> state)
    {
		state.updateVisionInformation();
		makeMoves();
        endTurn();
		io.flushOutputChunk();
    }
};

int Bot::closestLocation(const Pos& loc, const vector<Pos>& location) {
	int result = -1;
	double minDist = 10000000;
	for(size_t i = 0; i < location.size(); ++i) {
		double dist = state.distance(location[i], loc);
		if(dist < minDist) {
			minDist = dist;
			result = (int)i;
		}
	}

	return result;
}



//makes the bots moves for the turn
void Bot::makeMoves()
{
	size_t nAnts = state.myAnts.size();

	for(size_t ant = 0; ant< nAnts; ant++) {
		// Pick out this ants location from the set for later use.
		Pos antLoc = state.myAnts[ant];

		DirVec const& dirs = randomDirVec();

		int bestMove = 0, bestRank = -10000000;
		for(int i = 0; i<TDIRECTIONS; i++) {
			int d = dirs[i];
			int rank = rankMove(antLoc, d);
			if(rank > bestRank) {
				bestRank = rank;
				bestMove = d;
			}
		}
		if(bestRank > -100) {
			// This will not be needed. Just for testing the identifyer as things stand at the moment.
			g_state->identifier->m_map->moveAnt(antLoc, state.getLocation(antLoc, bestMove));

			state.makeMove(antLoc, bestMove); // Needed because the map is still just a dummy.
		}
	}

    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
}

//finishes the turn
void Bot::endTurn()
{
    if(state.turn > 0)
        state.reset();
    state.turn++;

	io.output() << "go" << endl;
}

bool Bot::safeLocation(const Pos &loc) {
	return !(state.grid[loc[0]][loc[1]].isWater || state.grid[loc[0]][loc[1]].isFood);
}
