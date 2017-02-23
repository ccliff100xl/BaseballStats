#pragma once
#include "SportsStatsConstants.h"
#include <vector>

class Player;
class Team;
class PlayRecord;
class GameLog;

//This class will hold the state of the game: score, inning, batter, baserunners...
class GameState
{
	//Offensive players, could be fixed length array but I do not want to worry about copying
	// [0] batter
	// [1] runner on first 
	// [2] runner on second 
	// [3] runner on third
	std::vector<const Player*> _offensive_players;

	//TODO: Add defensive players

	//Keep pointer to gamelog for basic information
	const GameLog* _log;

	//Outs
	int _outs = 0;

	//Inning info (intialize to start of game)
	int _inning = 1;
	HalfInning _inning_half = INNING_TOP;

	//Score
	int _runs_visitor = 0;
	int _runs_home = 0;

public:
	//Create based on log
	GameState(const GameLog* log_);

	//Method which does most of the work
	void updateStateFromPlay(const PlayRecord* play_);

	//Accessors
	const Player* getBatter() const { return _offensive_players[0]; }

	//Setters
	void setBatter(const Player* batter_) { _offensive_players[0] = batter_; }

	//Print game informaion
	void printGameInfo() const;

	//Define operators as friends 
	friend std::ostream& operator<<(std::ostream & os_, const GameState & gs_);

private:
	//Methods called by updateStateFromPlay
	void updateBaserunners(const PlayRecord* play_);
	void addRunScored();
};

