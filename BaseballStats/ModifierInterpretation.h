#pragma once
#include "SportsStatsConstants.h"
#include <string>
#include <vector>

//An instance of this will map text from after the / in an event to the database
//These objects are made one for parsing, not for stats calculation
class ModifierInterpretation
{
	//This is the string pattern at the beginning of the event which must match
	const std::string _log_pattern;
	//This is the result which will be assigned to the play
	const PlayModifier _modifier;
public:
	ModifierInterpretation(std::string log_pattern_, PlayModifier _modifier) :
		_log_pattern(log_pattern_),
		_modifier(_modifier) {}

	PlayModifier getModifier() const { return _modifier; }

	friend bool operator==(const std::string log_pattern_, const ModifierInterpretation& modifier_interpretation_);
	friend bool operator==(const ModifierInterpretation& modifier_interpretation_, const std::string log_pattern_);

	//This will hold the conversion information from file to database
	static const std::vector<ModifierInterpretation> InterpretationArray;
};

