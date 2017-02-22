#pragma once
#include "SportsStatsConstants.h"
#include <string>
#include <vector>

//An instance of this will map text from before the / in an event to the database
//These objects are made one for parsing, not for stats calculation
class EventInterpretation
{
	//This is the string pattern at the beginning of the event which must match
	const std::string _log_pattern;
	//This is the result which will be assigned to the play
	const EventResult _batting_result;
public:
	EventInterpretation(std::string log_pattern_, EventResult batting_result_) :
		_log_pattern(log_pattern_) ,
		_batting_result(batting_result_) {}

	EventResult getBattingResult() const { return _batting_result; }

	friend bool operator==(const std::string log_pattern_, const EventInterpretation& event_interpretation_);
	friend bool operator==(const EventInterpretation& event_interpretation_, const std::string log_pattern_);

	//This will hold the conversion information from file to database
	static const std::vector<EventInterpretation> InterpretationArray;
};

