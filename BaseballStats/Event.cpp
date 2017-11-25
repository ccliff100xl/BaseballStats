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
		////From play
		//There will be () scattered throughout which need to be handled seperately
		//So, parse those first
		//Make sure first char isn't a (
		if (play_->getEventRaw()[0] == '(') {
			throw("Event::Event: invalid leading ( in event string");
		}
		const StringVector line_parsed_parantheses = SplitStringToVector(play_->getEventRaw(), "()");
		//Now, only search outside of (), index 0, 2, ...
		//Find which comes first, / or ., create string of everything before (including ())
		bool found_event_end = false;
		for (int icell = 0; icell < line_parsed_parantheses.size(); icell++) {
			//If icell is odd, add contents to string inside of parantheses
			if (icell % 2 == 1) {
				event_string.append("(");
				event_string.append(line_parsed_parantheses[icell]);
				event_string.append(")");
				continue;
			}
			//This is even, so search for . or /
			//If not found, add char to string
			for (auto&& c : line_parsed_parantheses[icell]) {
				if (c == '.' || c == '/') {
					found_event_end = true;
					break;
				}
				else {
					//If this is an FYI character, ignore it
					if (c != '!' && c != '?' && c != '#') {
						//No clean way to create string from char?
						const char str[2] = { c, '\0' };
						event_string.append(str);
					}
				}
			}
			if (found_event_end) {
				//Done, break
				break;
			}
		}
	}
	const size_t n_chars_event = event_string.size();

	////////DEBUG: Use this to find a specific play
	//std::string line_check = "play,1,1,tomay001,00,X,FC5/G/DP/MREV.3X3(5);BX1(53)";
	//const string line_current_full = play_->getLineRaw();
	//if (line_current_full == line_check) {
	//	std::cout << "FOUND " << line_check << std::endl;
	//}
	//else {
	//	//std::cout << line_current_full << " NOT " << line_check << std::endl;
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
				//But, there could be an error
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
			if (event_parsed[1] == "B") {
				//Assume out at first
				_baserunner_movements.push_back(BaserunnerMovement(0, 1, true));
			}
			else {
				//Someone other than batter got out
				const int baserunner_out = string2int(event_parsed[1]);
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
			_baserunner_movements.push_back(BaserunnerMovement(baserunner_out, baserunner_out + 1, true));
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
		else if (n_cell == 5) {
			//5 cells, grounded into triple play, batter got out (implicit)
			_batting_result = GROUND_OUT;
			//Figure out who got out
			const int baserunner_out_1 = atoi(event_parsed[1].c_str());
			const int baserunner_out_2 = atoi(event_parsed[3].c_str());
			//Add the movements
			_baserunner_movements.push_back(BaserunnerMovement(baserunner_out_1, baserunner_out_1 + 1, true));
			_baserunner_movements.push_back(BaserunnerMovement(baserunner_out_2, baserunner_out_2 + 1, true));
			//Finally, add batter getting out
			_baserunner_movements.push_back(BaserunnerMovement(0, 1, true));
		}
		else if (n_cell == 6) {
			//6 cells, grounded into triple play, all outs explicit
			_batting_result = GROUND_OUT;
			//Figure out who got out
			const int baserunner_out_1 = atoi(event_parsed[1].c_str());
			const int baserunner_out_2 = atoi(event_parsed[3].c_str());
			const int baserunner_out_3 = atoi(event_parsed[5].c_str());
			//Add the movements
			_baserunner_movements.push_back(BaserunnerMovement(baserunner_out_1, baserunner_out_1 + 1, true));
			_baserunner_movements.push_back(BaserunnerMovement(baserunner_out_2, baserunner_out_2 + 1, true));
			_baserunner_movements.push_back(BaserunnerMovement(baserunner_out_3, baserunner_out_3 + 1, true));
		}
		else {
			//Not expecting so many cells, error
			std::cout << "Event: " << event_string << std::endl;
			throw std::exception("Event::Event: Too many '(...)' in event");
		}
	}
	break;
	case STRIKE_OUT:
	{
		//Strike out is an out at the plate
		_baserunner_movements.push_back(BaserunnerMovement(0, 0, true));
	}
	break;
	case CAUGHT_STEALING:
	{
		//There may be multiples, seperated by ;
		vector<string> event_parsed;
		boost::split(event_parsed, event_string, boost::is_any_of(";"));
		for (auto&& event_string_single : event_parsed) {
			//First decide if there was an error
			const bool error_occured = doesErrorNegateOut(event_string_single);
			//If there was an error, than this is not an out
			const bool made_out = !error_occured;
			//Determine which baserunner got out
			if (event_string_single[2] == '2') {
				//Out from 1 to 2
				_baserunner_movements.push_back(BaserunnerMovement(1, 2, made_out, error_occured));
			}
			else if (event_string_single[2] == '3') {
				//Out from 2 to 3
				_baserunner_movements.push_back(BaserunnerMovement(2, 3, made_out, error_occured));
			}
			else if (event_string_single[2] == 'H') {
				//Out from 3 to H
				_baserunner_movements.push_back(BaserunnerMovement(3, 4, made_out, error_occured));
			}
			else {
				//Nothing else allowed
				std::cout << event_string_single << std::endl;
				throw std::exception("Event::Event: Invalid stolen base");
			}
		}
	}
		break;
	case PICKED_OFF_CAUGHT_STEALING:
	{
		//Need to determine which runner was picked off
		const int i_base_out = (event_string[4] == 'H' ) ? 4 : char2int(event_string[4]);
		//It seems like the base given is where the out is made as he's stealing, so 
		//he's coming from one before
		_baserunner_movements.push_back(BaserunnerMovement(i_base_out-1, i_base_out, true));

		//Check for error
		const bool is_error = doesErrorNegateOut(event_string);
		if (is_error) {
			//Found an error, get rid of BaserunnerMovement just added
			_baserunner_movements.pop_back();
			//Replace it with successful advance due to error
			_baserunner_movements.push_back(BaserunnerMovement(i_base_out - 1, i_base_out, false, true));
		}
	}
	break;
	case PICKED_OFF:
	{
		//Need to determine which runner was picked off
		const int i_base_out = char2int(event_string[2]);
		//Movement (with no movement) but with out
		_baserunner_movements.push_back(BaserunnerMovement(i_base_out, i_base_out, true));

		//Check for error
		const bool is_error = doesErrorNegateOut(event_string);
		if (is_error) {
			//Found an error, get rid of BaserunnerMovement just added
			_baserunner_movements.pop_back();
			//Failed pickoff results in no baserunner movement
		}
	}
	break;
	case SINGLE:
		//Add movement from batter to 1
		_baserunner_movements.push_back(BaserunnerMovement(0, 1));
		//S0 means hit to position 0, which is invalid.  See in 1941PHA.EVA, so check for it
		if (event_string.size() > 1) {
			if (event_string[1] != '0') {
				_hit_location = convertPositionIntCharToDefensivePosition(event_string[1]);
			}
		}
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
			//Add movement, format is SB$, handle H for home (4)
			const int base_final = (s[2] == 'H') ? 4 : s[2] - '0';
			_baserunner_movements.push_back(BaserunnerMovement(base_final-1, base_final));
		}
	}
	break;

	//End of switch for play results
	}

	//Handle possible additional plays. For example, a stolen base on a strikeout
	//Look for plus sign
	const StringVector event_parsed_extra = SplitStringToVector(event_string, "+");
	if (event_parsed_extra.size() > 1) {
		//Only allowed for certain types of plays
		if (_batting_result == WALK ||
			_batting_result == INTENTIONAL_WALK ||
			_batting_result == STRIKE_OUT) {
			//Parse information seperated by ; , but before '.' eg. "K+SB3;SB2.2-H(E5)(NR);1-3"
			const StringVector extra_plays_raw = SplitStringToVector(event_parsed_extra[1], ".");
			const StringVector additional_plays_parsed = SplitStringToVector(extra_plays_raw[0], ";");
			//Loop over all the additional plays
			for (int i_play = 0; i_play < additional_plays_parsed.size(); i_play++) {
				//Parse additional play
				Event event_2(play_, &(additional_plays_parsed[i_play]));
				//Check that result is valid, based on what has been seen and makes sense
				const EventResult result_2 = event_2.getBattingResult();
				if (result_2 != STOLEN_BASE &&
					result_2 != CAUGHT_STEALING &&
					result_2 != UNKNOWN_ADVANCE &&
					result_2 != DEFENSIVE_INDIFFERENCE &&
					result_2 != PICKED_OFF &&
					result_2 != PICKED_OFF_CAUGHT_STEALING &&
					result_2 != WILD_PITCH &&
					result_2 != PASSED_BALL &&
					result_2 != ERROR) {
					//This is not allowed
					std::cout << "Result: " << BattingResultString[result_2] << std::endl;
					throw exception("K+Event not allowed");
				}
				//This play is causing problems: play,7,0,winst101,??,,K+E2.1-2
				//The error only allowed the runner to go to second, it did not allow the batter to
				//get to first, so, I will not add baserunner movement for errors
				//Add whatever happened in that event to this one
				if (result_2 != ERROR) {
					_baserunner_movements.insert(std::end(_baserunner_movements),
						std::begin(event_2._baserunner_movements), std::end(event_2._baserunner_movements));

				}
			}
		}
		else {
			//+ sign encounred for the wrong type of play
			throw std::exception("Event Constructor: '+' after invalid type");
		}
	}
}

void Event::setHitLocation(const DefensivePosition hit_location_)
{
	//This check is not correct.  The hit location may be set based on who fielded
	//it, but that is not necessarily where in the field the play was made, so the 
	//modifier should be allowed to change the location

	//if (_hit_location != UNKNOWN_DEFENSIVE_POSITION) {
	//	//Location is already set, allow it to be set again if it matches
	//	if (_hit_location == hit_location_) {
	//		//This does not change anything, just return
	//		return;
	//	}
	//	//Cannot set it twice
	//	throw std::exception("Event::setHitLocation: _hit_location already set");
	//}
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

//This will parse an event string to see if there is an error in the FIRST trailing ()
//It only negates the out if it is in the first ()
bool Event::doesErrorNegateOut(const std::string event_string_)
{
	//Find anything in ()
	const StringVector event_parsed = SplitStringToVector(event_string_, "()");
	if (event_parsed.size() > 1) {
		const std::string parantheses_contents = event_parsed[1];
		//There is something in parantheses, check if it contains an E
		for (auto&& e : parantheses_contents) {
			if (e == 'E') {
				//This is an error
				return true;
			}
		}
	}
	//Nothing additional in (), or nothing found in (), not an error
	return false;
}