#include "EventInterpretation.h"
#include <iostream>
using namespace std;
bool operator==(const std::string log_pattern_, const EventInterpretation & event_interpretation_)
{
	//Get string to compare to 
	const string event_string = event_interpretation_._log_pattern;

	//Chech if the first char string of event_string is a digit
	//'$' is code for digit
	if (event_string[0] == '$') {
		//Check if the second char is a digit
		if (event_string[1] == '$') {
			//Must have two digits at beginning of log_pattern_
			return (isdigit(log_pattern_[0]) && isdigit(log_pattern_[1]));
		} 
		else {
			//Must have one digits at beginning of log_pattern_ (not two)
			return (isdigit(log_pattern_[0]) && !isdigit(log_pattern_[1]));
		}
	}

	//Return if this matches, only search length of event_string
	const int match_result = event_string.compare(0, event_string.length(), log_pattern_, 0, event_string.length());
	if (match_result == 0) {
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
const std::vector<EventInterpretation> EventInterpretation::InterpretationArray = {
	EventInterpretation("S", BattingResult::SINGLE),
	EventInterpretation("D", BattingResult::DOUBLE),
	EventInterpretation("T", BattingResult::TRIPLE),
	EventInterpretation("HR", BattingResult::HR),
	EventInterpretation("K", BattingResult::STRIKE_OUT),
	EventInterpretation("E", BattingResult::ERROR),
	EventInterpretation("W", BattingResult::WALK),
	EventInterpretation("I", BattingResult::WALK),
	EventInterpretation("HP", BattingResult::WALK),
	EventInterpretation("NP", BattingResult::NO_PLAY),
	EventInterpretation("C", BattingResult::NO_PLAY), //Overly simplistic
	EventInterpretation("$", BattingResult::FLY_OUT),
	EventInterpretation("$$", BattingResult::GROUND_OUT),
};