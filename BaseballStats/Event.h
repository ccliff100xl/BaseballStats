#pragma once
#include "SportsStatsConstants.h"
#include "BaserunnerMovement.h"
#include <vector>

class Play;

//This will hold the information in the "event" portion of the play
// See "The event field of the play record" http://www.retrosheet.org/eventfile.htm
// All results stored as baserunner movements, outs and runs are not tracked explicitely
class Event
{
	EventResult _batting_result = NOT_PARSED;
	DefensivePosition _hit_location = UNKNOWN_DEFENSIVE_POSITION;
	std::vector<BaserunnerMovement> _baserunner_movements;

public:
	//Accessors
	EventResult getBattingResult() const { return _batting_result; }
	std::vector<BaserunnerMovement> getBaserunnerMovements() const { return _baserunner_movements; }
	DefensivePosition getHitLocation() const { return _hit_location; };

	//Needed to copy location which may be in modifier
	void setHitLocation( const DefensivePosition hit_location_);

	//Construct from Play object
	Event(const Play* play_, const std::string* event_string_ = NULL);

	//Helper
	static DefensivePosition convertPositionIntCharToDefensivePosition(const char fielder_);
private:
	static EventResult parseBattingResult(const std::string event_string_);
};

