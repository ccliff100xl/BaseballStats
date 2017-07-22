#include "EventInterpretation.h"
#include <boost/algorithm/string/erase.hpp>
#include <iostream>
#include <ctype.h>
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
		//If log char 0 is digit, it's a match
		//Non-zero result means true for isdigit
		return (isdigit(log_pattern[0]) != 0);
	}

	//If it makes it here, the event_string is char(s), get leading string from log_pattern
	const size_t i_digit = log_pattern.find_first_of("0123456789/.+(");
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
const std::vector<EventInterpretation> EventInterpretation::InterpretationArray = {
	EventInterpretation("$", EventResult::NUMERIC_UNCERTAIN), //This is the ONLY one that should start with $
	EventInterpretation("S", EventResult::SINGLE),
	EventInterpretation("D", EventResult::DOUBLE),
	EventInterpretation("DGR", EventResult::GROUND_RULE_DOUBLE),
	EventInterpretation("T", EventResult::TRIPLE),
	EventInterpretation("HR", EventResult::HR),
	EventInterpretation("H", EventResult::HR), //H or HR may be in log
	EventInterpretation("K", EventResult::STRIKE_OUT),
	EventInterpretation("E", EventResult::ERROR),
	EventInterpretation("FC", EventResult::FIELDERS_CHOICE),
	EventInterpretation("FLE", EventResult::ERROR_ON_FOUL_BALL),
	EventInterpretation("W", EventResult::WALK),
	EventInterpretation("I", EventResult::WALK),
	EventInterpretation("IW", EventResult::INTENTIONAL_WALK),
	EventInterpretation("HP", EventResult::HIT_BY_PITCH),
	EventInterpretation("NP", EventResult::NO_PLAY), //NP is used for substitutions
	EventInterpretation("C", EventResult::CATCHER_INTERFERENCE),
	EventInterpretation("BK", EventResult::BALK),
	EventInterpretation("CS", EventResult::CAUGHT_STEALING), 
	EventInterpretation("CSH", EventResult::CAUGHT_STEALING), //Caught stealing at home
	EventInterpretation("DI", EventResult::DEFENSIVE_INDIFFERENCE), 
	EventInterpretation("OA", EventResult::UNKNOWN_ADVANCE),
	EventInterpretation("PB", EventResult::PASSED_BALL),
	EventInterpretation("WP", EventResult::WILD_PITCH),
	EventInterpretation("PO", EventResult::PICKED_OFF),
	EventInterpretation("POCS", EventResult::PICKED_OFF_CAUGHT_STEALING),
	EventInterpretation("POCSH", EventResult::PICKED_OFF_CAUGHT_STEALING),//picked off and caught stealing at home
	EventInterpretation("SB", EventResult::STOLEN_BASE),
};