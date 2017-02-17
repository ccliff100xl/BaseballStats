#pragma once
#include "Play.h"
#include "GameLog.h"
#include "GameState.h"
#include "SportsStatsConstants.h"

//Forward declaration
class BaseballDatabase;

//This class will hold most of the details of the state of the game
class PlayRecord : public Play
{
	//Pointers to objects related to this play
	const GameLog* const _game;
	const BaseballDatabase* const _db;

	//State of game
    GameState _state;

	//Results of this play
	BattingResult _batting_result = NOT_PARSED;
	bool _sacrifice = false;
public:
	//Only constructor is from Play* pointer
	PlayRecord(const Play* play_, GameState* state_, const GameLog * const game_, const BaseballDatabase* const db_);

	//Accessors
	const Player* getBatter() const { return _state.getBatter();  }

	//These functions are a little funny, they are counts but will only return 1 or 0
	int getNumberAtBats() const;
	int getNumberHits() const;

	//For slugging
	int getNumberBases() const;

	//Make operator definition a friend
	friend std::ostream& operator<<(std::ostream & os, const PlayRecord & t);

private:
	//Functions to add information to the play
	void addBatter(const std::vector<Player>& players_);

	//Function to parse what the result was for the batter
	static BattingResult parseBattingResult(std::string play_string_);
	static bool parseSacrifice(std::string play_string_);
};

