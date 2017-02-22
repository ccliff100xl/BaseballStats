#include "Event.h"
#include "Play.h"
#include "EventInterpretation.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <ctype.h>

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
	const size_t n_chars_event = event_string.size();
	
	//Do additional parsing based on batting result
	switch (_batting_result) {
		//These results should not be set yet
	case GROUND_OUT:
	case FLY_OUT:
		throw("Event::Event: invalid _batting_result from parseBattingResult");
	case NUMERIC_UNCERTAIN:
	{
		//$ A single fielder represents a fly ball out made by the specified fielder
		//$$ Strings of two or more fielders as an event specify a ground out where the put out is credited by the last fielder in the string. Other fielders are credited with assists.
		//$(%)$ $$(%)$ Events of this form are used to code grounded into double plays.
		//$(B)$(%) followed by the modifier LDP is used to indicate a lined into double play.

		//Seperate string based on locations of ( )
		vector<string> event_parsed;
		boost::split(event_parsed, event_string, boost::is_any_of("()"));
		//It's possible that the final cell is empty (if ")" is the last char)
		//If so, remove it
		if (event_parsed.back().size() == 0) {
			event_parsed.pop_back();
		}
	    const size_t n_cell = event_parsed.size();

		//If there is a single cell, it's easy to handle
		if (n_cell == 1) {
			if (event_parsed[0].size() == 1) {
				//Length one, can only be a fly ball
				_batting_result = FLY_OUT;
				_outs_made++;
			}
			else {
				//Multiple numbers is a ground ball, represent with movement
				_batting_result = GROUND_OUT;
				_baserunner_movements.push_back(BaserunnerMovement(0, 1, true));
			}
		}
		else if (n_cell == 2) {
			//If there are two cells, it's this form: play,7,1,tempg001,00,X,54(B)/BG25/SH.1-2
			// If the putout is made at a base not normally covered by the fielder the base runner, 
			// batter in this example, is given explicitly.
			_batting_result = GROUND_OUT;
			//Figure out who got out
			if (event_parsed[1].size() != 1) {
				//Must be a single char for 1-3 or H
				throw("Event::Event: invalid number of chars in ()");
			}
			//Check if batter got out
			if (event_parsed[1] == "H") {
				//Assume out at first
				_baserunner_movements.push_back(BaserunnerMovement(0, 1, true));
			}
			else {
				//Someone other than batter got out
				const int baserunner_out = atoi(event_parsed[1].c_str());
				_baserunner_movements.push_back(BaserunnerMovement(baserunner_out, baserunner_out + 1, true));
				//That means that batter got on
				_baserunner_movements.push_back(BaserunnerMovement(0, 1));
			}
			
		}
		else if (n_cell == 3) {
			//3 cells, grounded into double play, middle cell is runner who got out
			_batting_result = GROUND_OUT;
			const int baserunner_out = atoi(event_parsed[1].c_str());
			//Lead runner out
			_baserunner_movements.push_back(BaserunnerMovement(baserunner_out, baserunner_out+1, true));
			//Batter out
			_baserunner_movements.push_back(BaserunnerMovement(0, 1, true));
		}
	}
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
	case STOLEN_BASE:
	{
		//Figure out what baserunner moved, there may be multiple seperated by ;
		vector<string> event_parsed;
		boost::split(event_parsed, event_string, boost::is_any_of(";"));
		for (auto&& s : event_parsed) {
			//Add movement, format is SB$
			const int base_final = s[2] - '0';
			_baserunner_movements.push_back(BaserunnerMovement(base_final-1, base_final));
		}
	}
		break;
	}
}

EventResult Event::parseBattingResult(const std::string event_string_)
{
	for (auto&& e : EventInterpretation::InterpretationArray) {
		if (e == event_string_) return e.getBattingResult();
	}
	std::cout << event_string_ << " not recognized" << std::endl;
	throw std::exception("Event::parseBattingResult: Play not recognized");
	return EventResult::NOT_RECOGNIZED;
}