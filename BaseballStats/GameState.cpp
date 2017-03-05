#include "stdafx.h"
#include "GameState.h"
#include "GameLog.h"
#include "PlayRecord.h"
#include "Team.h"
#include <string>
#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>

GameState::GameState(const GameLog* log_) : _log(log_) {
	//Initialize vector to length 4, all NULL
	_offensive_players.push_back(NULL);
	_offensive_players.push_back(NULL);
	_offensive_players.push_back(NULL);
	_offensive_players.push_back(NULL);
}

void GameState::updateStateFromPlay(const PlayRecord* play_)
{
	//Make sure state of object matches play_
	//Inning
	if (play_->getInning() != _inning) {
		play_->debugPrintDatabasePlays();
		throw std::exception("GameState::updateStateFromPlay: Inning missmatch");
	}
	//Half inning
	if (play_->getTeam() == TEAM_HOME && _inning_half == INNING_TOP) {
		play_->debugPrintDatabasePlays();
		throw std::exception("GameState::updateStateFromPlay: Inning half missmatch (home/top)");
	}
	//Half inning
	if (play_->getTeam() == TEAM_VISITOR && _inning_half == INNING_BOTTOM) {
		play_->debugPrintDatabasePlays();
		throw std::exception("GameState::updateStateFromPlay: Inning half missmatch (visitor/bottom)");
	}

	//Update baserunners, this does everything
	updateBaserunners(play_);

	//Check if there are more than three outs
	if (_outs > 3) {
		std::cout << "GameState::updateStateFromPlay outs: " << _outs << std::endl;
		throw std::exception("GameState::updateStateFromPlay: too many outs");
	}

	//Check if there are three outs
	if (_outs == 3) {
		//This is the end of the half-inning, reset
		_outs = 0;
		//Set all _offensive_players to NULL
		_offensive_players[0] = NULL;
		_offensive_players[1] = NULL;
		_offensive_players[2] = NULL;
		_offensive_players[3] = NULL;
		if (_inning_half == INNING_TOP) {
			_inning_half = INNING_BOTTOM;
		}
		else {
			_outs = 0;
			_inning++;
			_inning_half = INNING_TOP;
		}
	}

	//////////////////////////////
	//Do sanity checks to find errors in design
	
	//Common string to add errors to
	std::string error_info;

	//Is a player on multiple bases, start search a 1 (don't check batter)
	for (int i_p1 = 1; i_p1 < _offensive_players.size(); i_p1++) {
		//If i_p1 is NULL, continue
		if (_offensive_players[i_p1] == NULL) continue;
		for (int i_p2 = i_p1+1; i_p2 < _offensive_players.size(); i_p2++) {
			if (_offensive_players[i_p1] == _offensive_players[i_p2]) {
				error_info.append("Baserunner on multiple bases");
			}
		}
	}
	
	//Was an error found
	if (error_info.length() > 0) {
		//Print plays
		play_->debugPrintDatabasePlays();
		//Print error details
		std::cout << error_info << std::endl;
		//Error out
		throw std::exception("Consistency error found in GameState::updateStateFromPlay");
	}
}

//This will update baserunners and update runs and outs if they are part of the baserunning
void GameState::updateBaserunners(const PlayRecord* play_)
{
	//Update baserunners based on event string: http://www.retrosheet.org/eventfile.htm#5
	//play, 3, 0, fielc001, 00, X, S7 / L7LD.3 - H; 2 - H; BX2(7E4)
	std::string event_string = play_->getEventRaw();

	//Get movements from event, then add to them
	std::vector<BaserunnerMovement> movements = play_->getBaserunnerMovements();

	//Find . to add explicit runner location updates (2-3 or 2X3)
	std::vector<std::string> line_parsed_period;
	boost::split(line_parsed_period, event_string, boost::is_any_of("."));

	////Use this to find a specific play
	//std::string event_check = "K+SB3;SB2";
	//if (event_string == event_check) {
	//	std::cout << "FOUND " << event_check << std::endl;
	//}

	//Add to baserunner movements
	if (line_parsed_period.size() > 1) {
		//Baserunning information seperated by ;
		std::vector<std::string> line_parsed_semi;
		boost::split(line_parsed_semi, line_parsed_period[1], boost::is_any_of(";"));
		for (auto&& m_str : line_parsed_semi) {
			//There will always be three basic chars
			//0 : starting base
			//1 : - safe advance, X out
			//2 : ending base
			// Additional chars are FYI about who did what

			//Make sure there actually are three chars
			if (m_str.size() < 3) {
				std::cout << "GameState::updateBaserunners: " << m_str << std::endl;
				throw std::exception("GameState::updateBaserunners movement has < 3 chars");
			}

			//Populate these values
			int base_start = -1;
			int base_end = -1;
			bool made_out = false;

			//Determine starting player, and clear base of starting player
			switch (m_str[0]) {
			case 'B':
				base_start = 0;
				break;
			case '1':
				base_start = 1;
				break;
			case '2':
				base_start = 2;
				break;
			case '3':
				base_start = 3;
				break;
			default:
				std::cout << "GameState::updateBaserunners: " << m_str[0] << std::endl;
				throw std::exception("GameState::updateBaserunners movement invalid start");
			}

			//Check if player advances, or is out
			if (m_str[1] == 'X') {
				//This is an out, increment and continue loop
				made_out = true;
			} else if (m_str[1] != '-') {
				std::cout << "GameState::updateBaserunners: " << m_str[1] << std::endl;
				throw std::exception("GameState::updateBaserunners movement invalid middle char");
			}

			//Determine finishing location
			switch (m_str[2]) {
			case '1':
				base_end = 1;
				break;
			case '2':
				base_end = 2;
				break;
			case '3':
				base_end = 3;
				break;
			case 'H':
				base_end = 4;
				break;
			default:
				std::cout << "GameState::updateBaserunners: " << m_str[2] << std::endl;
				throw std::exception("GameState::updateBaserunners movement invalid end");
			}

			//Now, check if there is more information
			StringVector line_parsed_details = SplitStringToVector(m_str, "()");
			//Get rid of first element, since it's the standard movement
			line_parsed_details.erase(line_parsed_details.begin());
			bool is_error = false;
			//Loop over all details
			for(auto&& details : line_parsed_details){
				//Check if ANY char in the detail is an error
				for (auto&& c : details) {
					if (c == 'E') {
						is_error = true;
					}
				}
			}

			//Add BaserunnerMovement to vector
			movements.push_back(BaserunnerMovement(base_start, base_end, made_out, is_error));
		}
	}

	//Loop over movements, make sure none of them have the same starting base
	//unless it's the batter and there is an error
	for (int i_m1 = 0; i_m1 < movements.size(); i_m1++) {
		for (int i_m2 = i_m1+1; i_m2 < movements.size(); i_m2++) {
			if (movements[i_m1].getStartingBase() == movements[i_m2].getStartingBase()) {

				////Not True: Non-batters can only advance on errors
				//if (movements[i_m1].getStartingBase() != 0 && 
				//	(!movements[i_m1].wasError() && !movements[i_m2].wasError())) {
				//	throw std::exception("GameState::updateBaserunners: Multiple movements for one player that is not batter without error");
				//}

				//This is not correct, runners can advance on throws to other bases (for example)
				//if (!movements[i_m1].wasError() && !movements[i_m2].wasError()) {
				//	throw std::exception("GameState::updateBaserunners: Multiple movements without error");
				//}

				//Delete the movement with the lower ending base
				if (movements[i_m1].getEndingBase() < movements[i_m2].getEndingBase()) {
					//Delete i_m1
					movements.erase(movements.begin()+ i_m1);
					//Decrease i_m1 so cell is not skipped
					--i_m1;
				}
				else if (movements[i_m1].getEndingBase() > movements[i_m2].getEndingBase()) {
					//Delete i_m2
					movements.erase(movements.begin() + i_m2);
					//Decrease i_m2 so cell is not skipped
					--i_m2;
				}
				else if (movements[i_m1].getStartingBase() == 0 &&
					movements[i_m1].getEndingBase() == 4) {
					//This is likely an inside the park homerun, make sure
					if (play_->getEventResult() != HR) {
						play_->debugPrintDatabasePlays();
						throw std::exception("GameState::updateBaserunners: Identical batter to home without HR");
					}
					//This is OK, delete 2nd movement
					movements.erase(movements.begin() + i_m2);
					//Decrease i_m2 so cell is not skipped
					--i_m2;
				}
				else if (movements[i_m1].getStartingBase() == 0 && play_->getEventResult() == FIELDERS_CHOICE) {
					//Fielder's choice sometimes explicitly moves runner and sometime doesn't
					//In this case it does, so delete redundant movement
					movements.erase(movements.begin() + i_m2);
					//Decrease i_m2 so cell is not skipped
					--i_m2;
				}
				else {
					play_->debugPrintDatabasePlays();
					throw std::exception("GameState::updateBaserunners: Identical movements encountered");
				}
			}
		}
	}

	//Sort movements vectore before applying
	sort(movements.begin(), movements.end(), baserunnerMovementCompare);

	//Actually move the baserunners
	const Player* br1 = _offensive_players[1];
	const Player* br2 = _offensive_players[2];
	const Player* br3 = _offensive_players[3];

	//Loop over movements, hopefully order of operations doesn't matter
	for (auto&& m : movements) {

		//Determine starting player, and clear base of starting player
		const Player* p_start = NULL;
		switch (m.getStartingBase()) {
		case 0:
			p_start = _offensive_players[0];
			break;
		case 1:
			p_start = br1;
			_offensive_players[1] = NULL;
			break;
		case 2:
			p_start = br2;
			_offensive_players[2] = NULL;
			break;
		case 3:
			p_start = br3;
			_offensive_players[3] = NULL;
			break;
		default:
			std::cout << "GameState::updateBaserunners: " << m.getStartingBase() << std::endl;
			throw std::exception("GameState::updateBaserunners movement invalid start");
		}

		//p_start can't be NULL here, check to be sure
		if (p_start == NULL) {
			//Print all plays before exit
			play_->debugPrintDatabasePlays();
			throw std::exception("GameState::updateBaserunners: p_start cannot be NULL here.");
		}

		//Check if player advances, or is out
		if (m.wasOutMade()) {
			//This is an out, increment and continue loop
			_outs++;
			continue;
		}
		else {
			//It's an advance, update baserunners
			switch (m.getEndingBase()) {
			case 1:
				_offensive_players[1] = p_start;
				break;
			case 2:
				_offensive_players[2] = p_start;
				break;
			case 3:
				_offensive_players[3] = p_start;
				break;
			case 4:
				//Make sure the object thinks a run was scored
				if (!(m.wasRunScored())) {
					throw std::exception("GameState::updateBaserunners: Runner home without run");
				}
				//A runner made it home
				addRunScored();
				break;
			default:
				std::cout << "GameState::updateStateFromPlay: " << m.getEndingBase() << std::endl;
				throw std::exception("GameState::updateStateFromPlay movement invalid end");
			}
		}
	}
}

//This will add a run scored based on who is hitting
void GameState::addRunScored()
{
	if (_inning_half == INNING_TOP) {
		_runs_visitor++;
	}
	else if (_inning_half == INNING_BOTTOM) {
		_runs_home++;
	}
	else {
		throw std::exception("GameState::addRunScored invalid _inning_half");
	}
}

//This will print the score, mostly for verification
void GameState::printGameInfo() const 
{
	std::cout << _log->getVisitorTeamID() <<" "<< _runs_visitor <<" ";
	std::cout << _log->getHomeTeamID() << " " << _runs_home << std::endl;
}

//Print everything possible from this object
//This isn't printing, _baserunner_1 must always be NULL
std::ostream& operator<<(std::ostream & os_, const GameState & gs_)
{
	//Print inning
	const std::string inning_string = (gs_._inning_half == INNING_TOP) ? "Top" : "Bottom";
	os_ << inning_string << " of inning: " << gs_._inning;
	//Print outs
	os_ << " Outs: " << gs_._outs << std::endl;
	//Print runners
	if (gs_._offensive_players[1]) {
		os_ << "  " << *(gs_._offensive_players[1]) << " on first base " << std::endl;
	}
	if (gs_._offensive_players[2]) {
		os_ << "  " << *(gs_._offensive_players[2]) << " on second base " << std::endl;
	}
	if (gs_._offensive_players[3]) {
		os_ << "  " << *(gs_._offensive_players[3]) << " on third base " << std::endl;
	}
	return os_;
}