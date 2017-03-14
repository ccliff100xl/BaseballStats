#include "PlayRecord.h"
#include "EventInterpretation.h"
#include "ModifierInterpretation.h"
#include "BaseballDatabase.h"
#include <boost/algorithm/string.hpp>
#include <ctype.h>

using namespace std;

//Create from a pointer to a play object
//baserunners_ holds runner on first, second, and third at start of play
PlayRecord::PlayRecord(const Play* play_, GameState* state_, const GameLog * const game_, const BaseballDatabase* const db_) : 
	Play(*play_) , 
	_event(play_), 
	_game(game_), 
	_db(db_), 
	_state_start(*state_)
{
	//Update batter in input state
	state_->setBatter(getBatterID());

	//Parse modifiers
	ParseModifiersToVector(getEventRaw(), _modifiers);

	//Determine if it was a sacrifce
	_sacrifice = parseSacrifice(getEventRaw());

	//Save output to object state
	_state = state_->updateStateFromPlay(this);
}

bool PlayRecord::parseSacrifice(std::string play_string_)
{
	//Sacrifice information comes after /
	vector<string> line_parsed;
	boost::split(line_parsed, play_string_, boost::is_any_of("/"));

	//If line_parsed has length < 2, there is nothing to check
	if (line_parsed.size() < 2) return false;

	//Ignore the tons of information which could be here and just check if the 
	//first char is S:
	//  SF    sacrifice fly
	//	SH    sacrifice hit(bunt)
	if (line_parsed[1][0] == 'S') {
		//This is a sacrifice, return true
		return true;
	}
	else {
		return false;
	}
}

void PlayRecord::ParseModifiersToVector(std::string play_string_, std::vector<PlayModifier>& modifiers_)
{
	//Parse up to ".", which is the end of the modifiers
	vector<string> line_parsed_period;
	boost::split(line_parsed_period, play_string_, boost::is_any_of("."));

	//Parse text after / from text before "." found above
	vector<string> line_parsed;
	boost::split(line_parsed, line_parsed_period[0], boost::is_any_of("/"));

	//If length is less than 2, nothing to do
	if (line_parsed.size() < 2) return;

	//Length > 2 means there are modifiers to parse
	//Loop over inputs and possible matches
	for(int i_s = 1; i_s < line_parsed.size(); i_s++)
	{
		bool match_found = false;
		const string modifier_string_in = line_parsed[i_s];
		for (auto&& m : ModifierInterpretation::InterpretationArray) {
			if (m == modifier_string_in) {
				modifiers_.push_back(m.getModifier());
				match_found = true;
				break;
			}
		}
		//Make sure a match was found
		if (!match_found) {
			cout << "Line: " << play_string_ << endl;
			cout << modifier_string_in << " not recognized" << endl;
			throw exception("Modifier not recognized");
		}
	}
}

ostream & operator<<(ostream & os, const PlayRecord & p)
{
	//This prints the raw line, for debug
	//os << " " << *(p.getBatter()) << " " << BattingResultString[p._batting_result] << " " << p.getLineRaw();
	//Print starting state of play (will always give it's own newline)
	os << std::endl << p._state_start;

	//Print what the batter did
	os << " Batter: " << *(p.getBatter()) << std::endl;
    os << " Result: " << BattingResultString[p.getBattingResult()] << std::endl;

	//Print any subs
	for (auto sub : p.getSubs()) {
		os << "  Sub: " << sub << " " << DefensivePositionString[sub.getDefensivePosition()] << std::endl;
	}

	//DEBUG print the raw line
	os << "   " << p.getLineRaw();

	//Throw error if it's in an unknown result
	EventResult br = p.getBattingResult();
	if (br == NUMERIC_UNCERTAIN || br == NOT_RECOGNIZED || br == NOT_PARSED) {
		//Add endl before error because it won't be added by caller
		os << std::endl;
		throw exception("Invalid batting result");
	}

	return os;
}

//Just the definition of each result
int PlayRecord::getNumberAtBats() const
{
	//If this is a sacrifice, it's not an at bat
	if (_sacrifice) return 0;
	
	//Check if this was a valid at bat
	switch (getBattingResult()) {
	case ERROR:
	case GROUND_OUT: 
	case FLY_OUT:
	case STRIKE_OUT: 
	case SINGLE:
	case DOUBLE:
	case GROUND_RULE_DOUBLE:
	case TRIPLE:
	case HR:
	case FIELDERS_CHOICE:
		//These are all valid at bats, return 1
		return 1;
	case WALK:
	case NO_PLAY:
	case CATCHER_INTERFERENCE:
	case CAUGHT_STEALING:
	case STOLEN_BASE:
	case PICKED_OFF_CAUGHT_STEALING:
	case PICKED_OFF:
	case HIT_BY_PITCH:
	case WILD_PITCH:
	case INTENTIONAL_WALK:
	case PASSED_BALL:
	case BALK:
	case DEFENSIVE_INDIFFERENCE:
	case UNKNOWN_ADVANCE:
	case ERROR_ON_FOUL_BALL:
		//These are not at bats
		return 0;
	default:
		cout << "Play " << BattingResultString[getBattingResult()] << " not recognized by getNumberAtBats" << endl;
		throw exception("Error in getNumberAtBats");
	}

	//Should never be here
	return -1;
}

//Just the definition of each result
int PlayRecord::getNumberHits() const
{
	//If this is a sacrifice, it's not a hit
	if (_sacrifice) return 0;

	//Check if this was a hit
	switch (getBattingResult()) {
	case SINGLE:
	case DOUBLE:
	case GROUND_RULE_DOUBLE:
	case TRIPLE:
	case HR:
		//These are all valid at bats, return 1
		return 1;
	case STRIKE_OUT:
	case GROUND_OUT:
	case FLY_OUT:
	case WALK:
	case ERROR:
	case NO_PLAY:
	case FIELDERS_CHOICE:
	case CATCHER_INTERFERENCE:
	case CAUGHT_STEALING:
	case STOLEN_BASE:
	case PICKED_OFF_CAUGHT_STEALING:
	case PICKED_OFF:
	case HIT_BY_PITCH:
	case WILD_PITCH:
	case INTENTIONAL_WALK:
	case PASSED_BALL:
	case BALK:
	case DEFENSIVE_INDIFFERENCE:
	case UNKNOWN_ADVANCE:
	case ERROR_ON_FOUL_BALL:
		//These are not hits
		return 0;
	default:
		cout << "Play " << BattingResultString[getBattingResult()] << " not recognized by getNumberHits" << endl;
		throw exception("Error in getNumberHits");
	}
	//Should never be here
	return -1;
}

//Should this be moved to Event?
int PlayRecord::getNumberBases() const
{
	//Base this on num hits, to avoid code duplication
	if (getNumberHits() == 0) {
		//No hits means no bases
		return 0;
	}
	
	//If it makes it here it was a hit, check which one
	switch (getBattingResult()) {
	case SINGLE:
		return 1;
	case DOUBLE:
	case GROUND_RULE_DOUBLE:
		return 2;
	case TRIPLE:
		return 3;
	case HR:
		return 4;
	default:
		cout << "Play " << BattingResultString[getBattingResult()] << " not recognized by getNumberBases" << endl;
		throw exception("Error in getNumberBases");
	}
	//Should never be here
	return -1;
}

void PlayRecord::debugPrintDatabasePlays() const
{
	//Only print 100 plays
	_db->printPlayList(10);
}