#pragma once
#include "Play.h"
#include "SportsStatsConstants.h"

//Forward declaration
class BaseballDatabase;

class PlayRecord : public Play
{
	//Reminder: that const* means that the object pointed to cannot be modified.  The pointer can be
	const Player* _batter = NULL;
    const BaseballDatabase * const _db;

	BattingResult _batting_result = NOT_PARSED;
public:
	//Only constructor is from Play* pointer
	PlayRecord(const Play* play_, const BaseballDatabase* const db_);

	//Accessors

	//These functions are a little funny, they are counts but will only return 1 or 0
	int getNumberAtBats() const;
	int getNumberHits() const;

	//Make operator definition a friend
	friend std::ostream& operator<<(std::ostream & os, const PlayRecord & t);

private:
	//Functions to add information to the play
	void addBatter(const std::vector<Player>& players_);

	//Function to parse what the result was for the batter
	BattingResult parseBattingResult(std::string play_string_);
};

