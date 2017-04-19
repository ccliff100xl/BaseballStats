#include "Event.h"
#include "stdafx.h"
#include "Play.h"
#include "EventInterpretation.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <ctype.h>

using namespace std;

//Construct the object from a Play* or event_string_
//If event_string_ != NULL, then it is used instead of play to define event_string
Event::Event(const Play* play_, const string* event_string_)
{
	string event_string;
	//Check if string is coming directly from input, ot Play
	if (event_string_ != NULL) {
		//Direct from input
		event_string = *event_string_;
	}
	else {
		//From play
		const StringVector line_parsed = SplitStringToVector(play_->getEventRaw(), "/");
		event_string = line_parsed[0];
	}
	const size_t n_chars_event = event_string.size();

	////DEBUG: Use this to find a specific play
	//std::string event_check = "K+SB3;SB2";
	//if (event_string == event_check) {
	//	std::cout << "FOUND " << event_check << std::endl;
	//}

	//Parse the batting result
	_batting_result = parseBattingResult(event_string);

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
		const StringVector event_parsed = SplitStringToVector(event_string, "()");
	    const size_t n_cell = event_parsed.size();

		//If there is a single cell, it's easy to handle
		if (n_cell == 1) {
			if (event_parsed[0].size() == 1) {
				//Length one, can only be a fly ball
				_batting_result = FLY_OUT;
				//Since he flew out, he got out at home
				_baserunner_movements.push_back(BaserunnerMovement(0, 0, true));
				//Add hit location
				_hit_location = convertPositionIntCharToDefensivePosition(event_parsed[0][0]);
			}
			else {
				//Multiple numbers is a ground ball
				_batting_result = GROUND_OUT;
				_hit_location = convertPositionIntCharToDefensivePosition(event_parsed[0][0]);
				//But, there could be an error, search for E
				if (event_parsed[0].find_first_of('E') != string::npos) {
					//There was an error, hitter goes to first with no out
					_baserunner_movements.push_back(BaserunnerMovement(0, 1, false));
				}
				else {
					//No error, batter is out
					_baserunner_movements.push_back(BaserunnerMovement(0, 1, true));
				}
			}
		}
		else if (n_cell == 2) {
			//If there are two cells, it's this form: play,7,1,tempg001,00,X,54(B)/BG25/SH.1-2
			// If the putout is made at a base not normally covered by the fielder the base runner, 
			// batter in this example, is given explicitly.
			_batting_result = GROUND_OUT;
			_hit_location = convertPositionIntCharToDefensivePosition(event_parsed[0][0]);
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
		else if (n_cell == 4) {
			//4 cells, grounded into double play, but batter probably got on
			_batting_result = GROUND_OUT;
			//Figure out who got out
			const int baserunner_out_1 = atoi(event_parsed[1].c_str());
			const int baserunner_out_2 = atoi(event_parsed[3].c_str());
			//Add the movements
			_baserunner_movements.push_back(BaserunnerMovement(baserunner_out_1, baserunner_out_1 + 1, true));
			_baserunner_movements.push_back(BaserunnerMovement(baserunner_out_2, baserunner_out_2 + 1, true));
			//Batter got on (if it was not one of the listed movements)
			if (baserunner_out_1 != 0 && baserunner_out_2 != 0) {
				_baserunner_movements.push_back(BaserunnerMovement(0, 1));
			}
		}
	}
	break;
	case STRIKE_OUT:
	{
		//Strike out is an out at the plate
		_baserunner_movements.push_back(BaserunnerMovement(0, 0, true));
		//Parse information seperated by + or ; , but before '.' eg. "K+SB3;SB2.2-H(E5)(NR);1-3"
		const StringVector strikeout_parsed = SplitStringToVector(event_string, ".");
		const StringVector additional_plays_parsed = SplitStringToVector(strikeout_parsed[0], "+;");
		//If there is more than one cell, then there is a play
		for (int i_play = 1; i_play < additional_plays_parsed.size(); i_play++) {
			//Parse additional play
			Event event_2(play_, &(additional_plays_parsed[i_play]));
			//Check that result is valid, based on what has been seen and makes sense
			const EventResult result_2 = event_2.getBattingResult();
			if (result_2 != STOLEN_BASE &&
				result_2 != CAUGHT_STEALING &&
				result_2 != UNKNOWN_ADVANCE &&
				result_2 != PICKED_OFF &&
				result_2 != WILD_PITCH &&
				result_2 != PASSED_BALL &&
				result_2 != ERROR) {
				//This is not allowed
				std::cout << "Result: " << BattingResultString[result_2] << std::endl;
				throw exception("K+Event not allowed");
			}

			//Add whatever happened in that event to this one
			_baserunner_movements.insert(std::end(_baserunner_movements),
				std::begin(event_2._baserunner_movements), std::end(event_2._baserunner_movements));
		}
	}
		break;
	case CAUGHT_STEALING:
		//Determine which baserunner got out
		if (event_string[2] == '2') {
			//Out from 1 to 2
			_baserunner_movements.push_back(BaserunnerMovement(1, 2, true));
		} else if (event_string[2] == '3') {
			//Out from 2 to 3
			_baserunner_movements.push_back(BaserunnerMovement(2, 3, true));
		} else if (event_string[2] == 'H') {
			//Out from 3 to H
			_baserunner_movements.push_back(BaserunnerMovement(3, 4, true));
		}
		else {
			//Nothing else allowed
			std::cout << event_string << std::endl;
			throw std::exception("Event::Event: Invalid stolen base");
		}
		break;
	case PICKED_OFF_CAUGHT_STEALING:
	{
		//Need to determine which runner was picked off
		const int i_base_out = char2int(event_string[4]);
		//It seems like the base given is where the out is made as he's stealing, so 
		//he's coming from one before
		_baserunner_movements.push_back(BaserunnerMovement(i_base_out-1, i_base_out, true));
	}
	break;
	case PICKED_OFF:
	{
		//Need to determine which runner was picked off
		const int i_base_out = char2int(event_string[2]);
		//Movement (with no movement) but with out
		_baserunner_movements.push_back(BaserunnerMovement(i_base_out, i_base_out, true));
		//But, was there an error? TODO: Make this a seperate function
		const StringVector event_parsed = SplitStringToVector(event_string, "()");
		if (event_parsed.size() > 1) {
			//There is something in parantheses, check if first char is 'E'
			if (event_parsed[1][0] == 'E') {
				//Found an error, get rid of BaserunnerMovement just added
				_baserunner_movements.pop_back();
				//Failed pickoff results in to baserunner movement
			}
		}
	}
	break;
	case SINGLE:
		//Add movement from batter to 1
		_baserunner_movements.push_back(BaserunnerMovement(0, 1));
		if (event_string.size() > 1) _hit_location = convertPositionIntCharToDefensivePosition(event_string[1]);
		break;
	case DOUBLE:
		//Add movement from batter to 2
		_baserunner_movements.push_back(BaserunnerMovement(0, 2));
		if (event_string.size() > 1) _hit_location = convertPositionIntCharToDefensivePosition(event_string[1]);
		break;
	case GROUND_RULE_DOUBLE:
		//Add movement from batter to 2
		_baserunner_movements.push_back(BaserunnerMovement(0, 2));
		//Don't parse location for ground rule double
		break;
	case TRIPLE:
		//Add movement from batter to 3
		_baserunner_movements.push_back(BaserunnerMovement(0, 3));
		if (event_string.size() > 1) _hit_location = convertPositionIntCharToDefensivePosition(event_string[1]);
		break;
	case HR:
		//Add movement from batter to home
		_baserunner_movements.push_back(BaserunnerMovement(0, 4));
		break;
	case ERROR:
		//Add movement from batter to first, single error is just one base
		_baserunner_movements.push_back(BaserunnerMovement(0, 1, false, true));
		break;
	case FIELDERS_CHOICE:
		//Batter got on
		_baserunner_movements.push_back(BaserunnerMovement(0, 1));
		break;
	case WALK:
	case HIT_BY_PITCH:
	case INTENTIONAL_WALK:
	case CATCHER_INTERFERENCE:
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

void Event::setHitLocation(const DefensivePosition hit_location_)
{
	if (_hit_location != UNKNOWN_DEFENSIVE_POSITION) {
		//Cannot set it twice
		throw std::exception("Event::setHitLocation: _hit_location already set");
	}
	_hit_location = hit_location_;
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

DefensivePosition Event::convertPositionIntCharToDefensivePosition(const char fielder_)
{
	const int fielder = fielder_ - '0';
	//String coverted to number OK
	if (fielder < 1 || fielder > 9) {
		std::cout << "fielder_: " << fielder_ << std::endl;
		throw std::exception("Event::convertPositionIntCharToDefensivePosition: fielder_ out of range");
	}
	return static_cast<DefensivePosition>(fielder - 1);
}