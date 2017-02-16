#include "EventInterpretation.h"
#include <boost/algorithm/string/erase.hpp>
#include <iostream>
using namespace std;
bool operator==(const std::string log_pattern_, const EventInterpretation & event_interpretation_)
{
	//Get string to compare to 
    const string& event_string = event_interpretation_._log_pattern;

	//Remove ! (exceptional play) and ? (uncertainty) source: http://www.retrosheet.org/eventfile.htm
	std::string log_pattern = log_pattern_;
	boost::erase_all(log_pattern, "!");
	boost::erase_all(log_pattern, "?");

	//Chech if the first char string of event_string is a digit
	//'$' is code for digit
	if (event_string[0] == '$') {
		//Check if the second char is a digit
		if (event_string[1] == '$') {
			//Must have two digits at beginning of log_pattern_
			return (isdigit(log_pattern[0]) && isdigit(log_pattern[1]));
		} 
		else {
			//Must have one digits at beginning of log_pattern_ (not two)
			return (isdigit(log_pattern[0]) && !isdigit(log_pattern[1]));
		}
	}

	//If it makes it here, the event_string is char(s), get leading string from log_pattern
	const size_t i_digit = log_pattern.find_first_of("0123456789/.");
	//Make sure i_digit is > 0, or it's definitely not a match
	if (i_digit < 1) return false;
	//Get substring from log
	const std::string play_str = log_pattern.substr(0, i_digit);

	//Return if this matches
	if (play_str == event_string) {
		return true;
	} else {
		return false;
	}
}

bool operator==(const EventInterpretation& event_interpretation_, const std::string log_pattern_)
{
	return log_pattern_ == event_interpretation_;
}

//$ means one number, $$ means two numbers
//source: http://www.retrosheet.org/eventfile.htm
//TODO: Need to add modifier for sacrifice to the play
const std::vector<EventInterpretation> EventInterpretation::InterpretationArray = {
	EventInterpretation("S", BattingResult::SINGLE),
	EventInterpretation("D", BattingResult::DOUBLE),
	EventInterpretation("DGR", BattingResult::DOUBLE), //Ground rule double
	EventInterpretation("T", BattingResult::TRIPLE),
	EventInterpretation("HR", BattingResult::HR),
	EventInterpretation("K", BattingResult::STRIKE_OUT),
	EventInterpretation("E", BattingResult::ERROR),
	EventInterpretation("FC", BattingResult::FIELDERS_CHOICE),
	EventInterpretation("W", BattingResult::WALK),
	EventInterpretation("I", BattingResult::WALK),
	EventInterpretation("IW", BattingResult::WALK),
	EventInterpretation("HP", BattingResult::WALK),
	EventInterpretation("NP", BattingResult::NO_PLAY),
	EventInterpretation("C", BattingResult::CATCHER_INTERFERENCE), //Catcher interference
	EventInterpretation("BK", BattingResult::NO_PLAY), //Balk
	EventInterpretation("CS", BattingResult::NO_PLAY), //Caught Stealing
	EventInterpretation("DI", BattingResult::NO_PLAY), //Defensive indiference
	EventInterpretation("OA", BattingResult::NO_PLAY), //Unknown baserunner advance
	EventInterpretation("PB", BattingResult::NO_PLAY), //Passed ball
	EventInterpretation("WP", BattingResult::NO_PLAY), //Wild pitch
	EventInterpretation("PO", BattingResult::NO_PLAY), //Picked off
	EventInterpretation("POCS", BattingResult::NO_PLAY), //Picked off (caught stealing)
	EventInterpretation("SB", BattingResult::NO_PLAY), //Stolen Base
	EventInterpretation("$", BattingResult::FLY_OUT),
	EventInterpretation("$$", BattingResult::GROUND_OUT),
};