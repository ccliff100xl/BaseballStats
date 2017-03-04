#pragma once
#include "SportsStatsConstants.h"
#include "BaserunnerMovement.h"
#include <vector>

class Play;

//This will hold the information in the "event" portion of the play
// See "The event field of the play record" http://www.retrosheet.org/eventfile.htm
class Event
{
	EventResult _batting_result = NOT_PARSED;
	int _outs_made = 0;
	int _runs_scored = 0;
	std::vector<BaserunnerMovement> _baserunner_movements;

public:
	//Accessors
	EventResult getBattingResult() const { return _batting_result; }
	int getOutsMade() const { return _outs_made;  }
	int getRunsScored() const { return _runs_scored; }
	std::vector<BaserunnerMovement> getBaserunnerMovements() const { return _baserunner_movements; }

	//Construct from Play object
	Event(const Play* play_, const std::string* event_string_ = NULL);
private:
	static EventResult parseBattingResult(const std::string event_string_);
};

