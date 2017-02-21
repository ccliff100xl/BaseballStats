#pragma once
#include "SportsStatsConstants.h"
#include <vector>

class Player;
class Team;
class PlayRecord;

//This class will hold the state of the game: score, inning, batter, baserunners...
class GameState
{
	//Players involved
	const Player* _batter = NULL;
	const Player* _baserunner_1 = NULL; //Runner of first
	const Player* _baserunner_2 = NULL; //Runner of second
	const Player* _baserunner_3 = NULL; //Runner of third

	//Something like this is better, but still needs to work for copying
	////Offensive players
	//// [0] batter
	//// [1] runner on first 
	//// [2] runner on second 
	//// [3] runner on third
	//std::vector<const Player*> _offensive_players;

	//Teams Invoved
	const Team* _visiting_team = NULL;
	const Team* _home_team = NULL;

	//Outs
	int _outs = 0;

	//Inning info (intialize to start of game)
	int _inning = 1;
	HalfInning _inning_half = INNING_TOP;

	//Score
	int _runs_visitor = 0;
	int _runs_home = 0;

public:
	//Method which does most of the work
	void updateStateFromPlay(const PlayRecord* play_);

	//Accessors
	const Player* getBatter() const { return _batter; }

	//Setters
	void setBatter(const Player* batter_) { _batter = batter_; }
	
	//Define operators as friends 
	friend std::ostream& operator<<(std::ostream & os_, const GameState & gs_);
private:
	//Methods called by updateStateFromPlay
	void updateBaserunners(const PlayRecord* play_);
	void addRunScored();
};

