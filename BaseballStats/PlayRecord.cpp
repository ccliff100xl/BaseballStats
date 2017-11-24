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
	const DefensivePosition hit_location =  ParseModifiersToVector(getEventRaw(), _modifiers);
	//If hit_location_ is valid, use it to set value in _event
	if (hit_location != UNKNOWN_DEFENSIVE_POSITION) {
		_event.setHitLocation(hit_location);
	}

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

//The hit location is returned as an DefensivePosition if one of the modifiers is hit location
DefensivePosition PlayRecord::ParseModifiersToVector(std::string play_string_, std::vector<PlayModifier>& modifiers_)
{
	//This will be updated if an appropriate modifier is parsed
	DefensivePosition hit_location = UNKNOWN_DEFENSIVE_POSITION;
	
	//Parse up to ".", which is the end of the modifiers
	vector<string> line_parsed_period;
	boost::split(line_parsed_period, play_string_, boost::is_any_of("."));

	//There may be a () which contains a /, so parse for that first
	StringVector line_parsed_parantheses = SplitStringToVector(line_parsed_period[0], "()");

	//Parse text after / from text before "." found above
	vector<string> line_parsed;
	if (line_parsed_parantheses.size() > 1) {
		//There was one of more (), take info after (), which is final entry
		boost::split(line_parsed, line_parsed_parantheses[line_parsed_parantheses.size()-1], boost::is_any_of("/"));
	}
	else {
		//No parantheses, parse original
		boost::split(line_parsed, line_parsed_period[0], boost::is_any_of("/"));
	}



	//If length is less than 2, nothing to do
	if (line_parsed.size() < 2) return hit_location;

	//Length > 2 means there are modifiers to parse
	//Loop over inputs and possible matches
	for(int i_s = 1; i_s < line_parsed.size(); i_s++)
	{
		bool match_found = false;
		const string modifier_string_in = line_parsed[i_s];
		//There appear to be trailing / with no modifier following it.
		//This means modifier_string_in has no length, in which case just skip it
		if (modifier_string_in.length() == 0) continue;
		for (auto&& m : ModifierInterpretation::InterpretationArray) {
			if (m == modifier_string_in) {
				modifiers_.push_back(m.getModifier());
				match_found = true;

				//Check if this is a hit location
				if (m.getModifier() == PM_HL) {
					//Make sure this has not already been set
					if (hit_location != UNKNOWN_DEFENSIVE_POSITION) {
						throw exception("PlayRecord::ParseModifiersToVector: Hit location set twice by modifiers");
					}
					//Set hit_location based on modifier_string_in
					hit_location = Event::convertPositionIntCharToDefensivePosition(modifier_string_in[0]);
				}
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
	//Return the hit location
	return hit_location;
}

ostream & operator<<(ostream & os, const PlayRecord & p)
{
	//This prints the raw line, for debug
	//os << " " << *(p.getBatter()) << " " << BattingResultString[p._batting_result] << " " << p.getLineRaw();

	//Start with newline to give space from previous info
	os << std::endl;

	//Print starting state of play (will always give it's own newline)
	//Skip for NO_PLAY to let subs happen
	if ( p.getBattingResult() != NO_PLAY) {
		os << p._state_start;
	}
	
	//Print what the batter did
	os << " Batter: " << *(p.getBatter()) << std::endl;
	os << " Result: " << BattingResultString[p.getBattingResult()];
	//Print hit location if set
	if (p.getHitLocation() != UNKNOWN_DEFENSIVE_POSITION) {
		os << " ( " << DefensivePositionString[p.getHitLocation()] << " )" ;
	}
	os << std::endl;

	//Print modifiers
	if (p._modifiers.size() > 0) {
		os << " Details: ";;
		for (auto m : p._modifiers) {
			os << PlayModifierString[m] << ", ";
		}
		//Overwrite final comma
		cout << '\b' << '\b' << " ";
		os << std::endl;
	}

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
	//Only print the most recent plays
	const int n_plays_print = 50;
	_db->printPlayList(n_plays_print);
}