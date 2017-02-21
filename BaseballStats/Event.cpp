#include "Event.h"
#include "Play.h"
#include "EventInterpretation.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

//Construct the object from a Play*
Event::Event(const Play* play_) 
{
	//Parse the batting result
	_batting_result = parseBattingResult(play_->getEventRaw());

	//Parse event string
	vector<string> line_parsed;
	boost::split(line_parsed, play_->getEventRaw(), boost::is_any_of("/"));
	const string event_string = line_parsed[0];
	
	//Do additional parsing based on batting result

	//I think this is the core problem now: I need to understand flyouts, ground outs, double plays
	//etc.
	//$ A single fielder represents a fly ball out made by the specified fielder
	//$$ Strings of two or more fielders as an event specify a ground out where the put out is credited by the last fielder in the string. Other fielders are credited with assists.
	//$(%)$ $$(%)$ Events of this form are used to code grounded into double plays.
	//$(B)$(%) followed by the modifier LDP is used to indicate a lined into double play.
	// parseBattingResult will have to be updated to I expect

	switch (_batting_result) {
	case GROUND_OUT:
		//Search for a complicated result (one or more plays in ()
		for (int i = 0; i < event_string.length(); i++) {
			if (event_string[i] == '(') {
				const int i_base = event_string[++i] - '0';
				_baserunner_movements.push_back(BaserunnerMovement(i_base, i_base, true));
			}
		}
		//Check if it anything was found
		if (_baserunner_movements.size() == 0) {
			//Nothing found, just increment outs
			_outs_made++;
		}
		break;
	case FLY_OUT:
		_outs_made++;
		break;
	case STRIKE_OUT:
		_outs_made++;
		break;
	case CAUGHT_STEALING:
		_outs_made++;
		break;
	case SINGLE:
		//Add movement from batter to 1
		_baserunner_movements.push_back(BaserunnerMovement(0, 1));
		break;
	case DOUBLE:
		//Add movement from batter to 2
		_baserunner_movements.push_back(BaserunnerMovement(0, 2));
		break;
	case TRIPLE:
		//Add movement from batter to 3
		_baserunner_movements.push_back(BaserunnerMovement(0, 3));
		break;
	case HR:
		//Add movement from batter to home
		_baserunner_movements.push_back(BaserunnerMovement(0, 4));
		break;
	case ERROR:
		//Add movement from batter to first, single error is just one base
		_baserunner_movements.push_back(BaserunnerMovement(0, 1));
		break;
	case WALK:
		//Add movement from batter to first for walk
		_baserunner_movements.push_back(BaserunnerMovement(0, 1));
		break;
	}
}

BattingResult Event::parseBattingResult(const std::string event_string_)
{
	for (auto&& e : EventInterpretation::InterpretationArray) {
		if (e == event_string_) return e.getBattingResult();
	}
	std::cout << event_string_ << " not recognized" << std::endl;
	throw std::exception("Event::parseBattingResult: Play not recognized");
	return BattingResult::NOT_RECOGNIZED;
}