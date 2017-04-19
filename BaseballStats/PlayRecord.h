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

	//COPY of State at the start of the play
	//This could be a pointer, if I was careful to make sure state passed in
	//continues to exist
	GameState _state_start;

	//State of game at the END of the play before clearing bases etc. for end of a half inning
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

	//Get result of event
	EventResult getEventResult() const { return _event.getBattingResult(); }
	DefensivePosition getHitLocation() const { return _event.getHitLocation(); };

	//These functions are a little funny, they are counts but will only return 1 or 0
	int getNumberAtBats() const;
	int getNumberHits() const;

	//For slugging and determing where the batter ended up
	int getNumberBases() const;

	//Make operator definition a friend
	friend std::ostream& operator<<(std::ostream & os, const PlayRecord & t);

	//Debug function
	void debugPrintDatabasePlays() const;

private:
	//Function to parse what the result was for the play
	//these could be members and just the values, but they are not
	static DefensivePosition ParseModifiersToVector(std::string play_string_, std::vector<PlayModifier>& modifiers_);
	static bool parseSacrifice(std::string play_string_);

	//For printing
	EventResult getBattingResult() const { return _event.getBattingResult(); }
};

