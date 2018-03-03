#pragma once
#include "SportsStatsConstants.h"
#include "Pitch.h"
#include <vector>

class Player;
class ActivePlayer;
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
	std::vector<const ActivePlayer*> _offensive_players;

	//Defensive Players, order does not have meaning
	std::vector<const ActivePlayer*> _defensive_players_away;
	std::vector<const ActivePlayer*> _defensive_players_home;

	//This flag will be set to true when an at bat ends, so that _pitches can be reset
	bool _batter_moved = false;
	//This flag will be set when a half inning ends to control printing
	bool _first_play_of_half_inning = true;

	//This is needed to know if this is the first "real" play of the inning
	//It would be nice to figure out a way to get rid of it
	EventResult _result = NOT_PARSED;

	//Pitches
	std::vector<Pitch> _pitches;

	//Keep pointer to gamelog for basic information
	const GameLog* _log = NULL;

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
	GameState(const GameLog* log_ = NULL);

	//Add pitches based on the pitches string from the log
	void addPitches(const std::string pitches_string_);

	//Method which does most of the work
	const GameState updateStateFromPlay(const PlayRecord* play_);

	//Accessors
	const ActivePlayer* getBatter() const;
	const ActivePlayer* getBaserunner(int base_) const;
	const ActivePlayer* getFielder(DefensivePosition pos_) const;
	TeamType getTeamBatting() const;

	//Setters
	void setBatter(const std::string player_id_);

	//Text output functions
	void printScore( std::ostream& os_) const;
	void printDefensivePlayers(std::ostream& os_) const;

	friend std::ostream& operator<<(std::ostream& os_, const GameState & gs_);

private:
	//Methods called by updateStateFromPlay
	void updateBaserunners(const PlayRecord* play_);
	void addRunScored();

	//Use this to determine if this is the first "real" play of an inning
	//It handles no plays
	bool isFirstPlayOfHalfInning() const { return _first_play_of_half_inning && _result != NO_PLAY; }
};

