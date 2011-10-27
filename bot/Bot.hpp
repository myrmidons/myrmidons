#ifndef BOT_H_
#define BOT_H_

#include "State.hpp"
#include "Ant.hpp"
#include <iostream>
#include <vector>

// Interface type for game state and command input/output
class IODevice
{
public:
	virtual std::istream& input() = 0;
	virtual std::ostream& output() = 0;

	virtual bool bufferInputChunk() { return true; }
	virtual void flushOutputChunk() {}
};

class Bot
{
	IODevice& io;
	State& state;
	AntIdentifier antID;

public:
	// Used for random ordering of directions.
	std::vector<DirVec> dirVecs;

	explicit Bot(IODevice& io_device);

	void playGame();  //plays a single game of Ants

	void makeMoves(); //makes moves for a single turn
	void endTurn();	  //indicates to the engine that it has made its moves

	bool safeLocation(Pos const& loc);
	int closestLocation(const Pos& loc, const std::vector<Pos>& location);
	void setupRandomDirections();
	int rankMove(Pos const& currentLoc, int dir);

	DirVec const& randomDirVec() const;
};

#endif //BOT_H_
