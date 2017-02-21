#pragma once
#include "Play.h"
#include "Event.h"
#include "GameState.h"
#include "SportsStatsConstants.h"
#include "BaserunnerMovement.h"

class GameLog;
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
	Event _event;
	std::vector<PlayModifier> _modifiers;
	bool _sacrifice = false;
public:
	//Only constructor is from Play* pointer
	PlayRecord(const Play* play_, GameState* state_, const GameLog * const game_, const BaseballDatabase* const db_);

	//Accessors
	const Player* getBatter() const { return _state.getBatter(); }
	std::vector<BaserunnerMovement> getBaserunnerMovements() const { return _event.getBaserunnerMovements(); }

	//These functions are a little funny, they are counts but will only return 1 or 0
	int getNumberAtBats() const;
	int getNumberHits() const;

	//For slugging and determing where the batter ended up
	int getNumberBases() const;

	bool didBatterScoreEarnedRun() const;
	bool didBatterGetOnWithoutHit() const;
	int getOutsFromEvent() const;

	//Make operator definition a friend
	friend std::ostream& operator<<(std::ostream & os, const PlayRecord & t);

	//Debug function
	void debugPrintDatabasePlays() const;

private:
	//Functions to add information to the play
	void addBatter(const std::vector<Player>& players_);

	//Function to parse what the result was for the play
	//these could be members and just the values, but they are not
	static void ParseModifiersToVector(std::string play_string_, std::vector<PlayModifier>& modifiers_);
	static bool parseSacrifice(std::string play_string_);

	//For printing
	BattingResult getBattingResult() const { return _event.getBattingResult(); }
};

