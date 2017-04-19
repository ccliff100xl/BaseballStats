#include "stdafx.h"
#include "GameState.h"
#include "GameLog.h"
#include "PlayRecord.h"
#include "Team.h"
#include <string>
#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>

//This needs to handle log_ == NULL
GameState::GameState(const GameLog* log_) : _log(log_) {
	//Initialize offensive vector to length 4, all NULL
	for (int io = 0; io < 4; io++) {
		_offensive_players.push_back(NULL);
	}
	//Initialize defensive vectors to length 10 to include DH
	for (int io = 0; io < 10; io++) {
		_defensive_players_away.push_back(NULL);
		_defensive_players_home.push_back(NULL);
	}

	//Add more information from log if it exists
	if (log_) {
		//Populate defensive players
		for (auto&& p : log_->getStarters()) {
			if (p.getTeam() == TEAM_VISITOR) {
				_defensive_players_away[p.getDefensivePosition()] = &p;
			}
			else if (p.getTeam() == TEAM_HOME) {
				_defensive_players_home[p.getDefensivePosition()] = &p;
			}
			else {
				throw std::exception("GameState::GameState: Invalid Team");
			}
		}
	}
}

//This will return a GameState reference after all updates for the play are made
//but before baserunners, etc. are updated due to the end of a half inning
const GameState GameState::updateStateFromPlay(const PlayRecord* play_)
{
	//Copy result from play to object
	_result = play_->getEventResult();

	//Make sure this is an acceptable value
	if (_result < GROUND_OUT || _result > NUMERIC_UNCERTAIN) {
		//This is an error
		throw std::exception("GameState::updateStateFromPlay: _result not recognized");
	}

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

	//Check if pitches needs to be reset
	if (_batter_moved) {
		_pitches.clear();
	}

	//Add pitches from string
	addPitches(play_->getPitchesString());

	//Update baserunners, this does everything
	updateBaserunners(play_);

	//Add substitutions
	for (auto&& p_new : play_->getSubs()) {
		//Figure out where to look
		std::vector<const ActivePlayer*>& players_ =
			(p_new.getTeam() == TEAM_HOME) ? _defensive_players_home : _defensive_players_away;

		//Loop over players_ and replace
		bool found_player_to_replace = false;
		for (int i_active = 0; i_active < players_.size(); i_active++) {
			//Skip NULL (probably and error, but will be caught later)
			if (players_[i_active] == NULL) continue;
			//Check if batting  position matches (if a match has not been found)
			if (players_[i_active]->getBattingPosition() == p_new.getBattingPosition()) {
				//Found player to replace
				players_[i_active] = &p_new; //THIS IS THE POINTER TO play_::subs, which seems not robust
				found_player_to_replace = true;
				break;
			}
		}

		//Make sure if was found
		if (!found_player_to_replace) {
			std::cout << "New player: " << p_new << std::endl;
			throw std::exception("GameState::updateStateFromPlay: could not find player to replace with sub");
		}
	}

	//Handle a very specific case: If the new sub was the DH, then the team loses their DH.
	//This will manifest itself by having two pitchers, in this case the one which has batting 
	//position 0 should be removed
	//Only update if no play
	if (_result != NO_PLAY) {
		//Check current defensive players
	    std::vector<const ActivePlayer*>& defensive_players =
			(_inning_half == INNING_TOP) ? _defensive_players_home : _defensive_players_away;
		//Loop over once, and count pitchers
		int n_pitchers = 0;
		for (auto&& p : defensive_players) {
			//Skip if NULL
			if (p == NULL) continue;
			if (p->getDefensivePosition() == PITCHER) n_pitchers++;
		}
		//If there are two pitchers, remove one
		int i_pitcher_delete = -1;
		if (n_pitchers == 2) {
			//Loop over again, this time delete the one with batting position 0
			for (int i_player = 0; i_player < defensive_players.size(); i_player++) {
				const ActivePlayer* p = defensive_players[i_player];
				if (p->getDefensivePosition() == PITCHER && p->getBattingPosition() == 0) {
					//Make sure i_pitcher_delete is not already set
					if (i_pitcher_delete >= 0) {
						//This is an error
						throw std::exception("Multiple pitchers to delete found");
					}
					//Save index to delete
					i_pitcher_delete = i_player;
				}
			}
			//See if a player to delete was found
			if (i_pitcher_delete < 0) {
				//This is an error
				throw std::exception("No pitcher to delete found");
			}
			//Delete pitcher
			defensive_players.erase(defensive_players.begin() + i_pitcher_delete);
		}
	}
	////////////////////End of update based on play/////////////////

	//If this is not a no play, make sure the defensive players are valid
	const std::vector<const ActivePlayer*>& current_defensive_players =
		(_inning_half == INNING_TOP) ? _defensive_players_home : _defensive_players_away;
	if (_result != NO_PLAY) {
		//Loop over possible positions (not including DH)
		for (int pos = PITCHER; pos != DESIGNATED_HITTER; pos++) {
			//Loop over players
			bool match_found = false;
			for (auto&& p : current_defensive_players) {
				//This may be NULL, if no DH
				if (p == NULL) continue;
				if (p->getDefensivePosition() == pos) {
					//Found match
					if (match_found) {
						//Multiple, error
						throw std::exception("Multiple players found for defensive position");
					}
					else {
						match_found = true;
					}
				}
			}
			if (!match_found) {
				//Missing, error
				throw std::exception("No player found for defensive position");
			}
		}
	}

	//Save state for return
	const GameState state_out(*this);

	///////////////Beginning of update based on inning//////////////

	//This should always be set to false, unless it's the end of a half inning
	//NO_PLAYS at the beginning of the inning are kind of an intermediate state, so leave
	//this as is.  
	if (_result != NO_PLAY) _first_play_of_half_inning = false;

	//Check if there are more than three outs
	if (_outs > 3) {
		std::cout << "GameState::updateStateFromPlay outs: " << _outs << std::endl;
		throw std::exception("GameState::updateStateFromPlay: too many outs");
	}

	//Check if there are three outs, 
	if (_outs == 3) {
		//This is the end of the half-inning, reset
		_outs = 0;
		_pitches.clear();
		_first_play_of_half_inning = true;
		//Set all _offensive_players to NULL
		_offensive_players[0] = NULL;
		_offensive_players[1] = NULL;
		_offensive_players[2] = NULL;
		_offensive_players[3] = NULL;
		if (_inning_half == INNING_TOP) {
			_inning_half = INNING_BOTTOM;
		}
		else {
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
		for (int i_p2 = i_p1 + 1; i_p2 < _offensive_players.size(); i_p2++) {
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

	//Return state saved above
	return state_out;
}

void GameState::addPitches(const std::string pitches_string_)
{
	//Loop over string and deal with each char
	std::string info;
	for (auto&& c : pitches_string_) {
		//Check if this is a non-pitch detail: +*.123>
		if (c == '+' || c == '*' || c == '.' || c == '1' || c == '2' || c == '3' || c == '>') {
			info.append(std::string(1, c));
		}
		else {
			//Add pitch to vector and clear info
			_pitches.push_back(Pitch(c, info));
			info.clear();
		}
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
			}
			else if (m_str[1] != '-') {
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
			for (auto&& details : line_parsed_details) {
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
		for (int i_m2 = i_m1 + 1; i_m2 < movements.size(); i_m2++) {
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
					movements.erase(movements.begin() + i_m1);
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
	const ActivePlayer* br1 = _offensive_players[1];
	const ActivePlayer* br2 = _offensive_players[2];
	const ActivePlayer* br3 = _offensive_players[3];

	//Loop over movements, hopefully order of operations doesn't matter
	for (auto&& m : movements) {

		//Determine starting player, and clear base of starting player
		const ActivePlayer* p_start = NULL;
		switch (m.getStartingBase()) {
		case 0:
			p_start = _offensive_players[0];
			//Something is happening to the batter, set flag
			_batter_moved = true;
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

const ActivePlayer* GameState::getBatter() const
{
	//Make sure _offensive_players is setup correctly
	if (_offensive_players.size() == 4 && _offensive_players[0] != NULL) {
		//OK, return batter
		return _offensive_players[0];
	}

	//If it makes it here it's an error
	throw std::exception("GameState::getBatter called for invalid _offensive_players vector");
	return NULL;
}

void GameState::setBatter(const std::string player_id_)
{
	//Find players to search
	const std::vector<const ActivePlayer*>& players_ =
		(_inning_half == INNING_TOP) ? _defensive_players_away : _defensive_players_home;

	//Loop over players
	for (auto&& p : players_) {
		if (p->getID() == player_id_) {
			//Found match, set and return
			_offensive_players[0] = p;
			return;
		}
	}

	//If it made it here, there was no match
	std::cout << "Could not find " << player_id_ << std::endl;
	throw std::exception("GameState::setBatter: could not find batter in active players");
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

//This will print the score
void GameState::printScore(std::ostream& os_) const
{
	os_ << _log->getVisitorTeamID() << " " << _runs_visitor << " ";
	os_ << _log->getHomeTeamID() << " " << _runs_home << std::endl;
}

//This will print the current defensive players
void GameState::printDefensivePlayers(std::ostream& os_) const
{
	//Get correct array of players
	const std::vector<const ActivePlayer*>& players =
		(_inning_half == INNING_TOP) ? _defensive_players_home : _defensive_players_away;
	//Print everything before DH
	for (int i = PITCHER; i != DESIGNATED_HITTER; i++) {
		if (players[i] == NULL) continue;
		DefensivePosition dp = players[i]->getDefensivePosition();
		//Don't allow pinch hitters or runners to print
		if (dp == PINCH_HITTER || dp == PINCH_RUNNER) {
			throw std::exception("GameState::printDefensivePlayers: called with pinch hitter or pinch runner");
		}
		os_ << DefensivePositionString[dp] << ": ";
		os_ << *(players[i]) << std::endl;
	}
}

//Print everything possible from this object
std::ostream& operator<<(std::ostream & os_, const GameState & gs_)
{
	//Print inning
	const std::string inning_string = (gs_._inning_half == INNING_TOP) ? "Top" : "Bottom";
	os_ << inning_string << " of inning: " << gs_._inning;
	//Print outs
	os_ << " Outs: " << gs_._outs << " ";
	//Print score
	os_ << gs_._log->getVisitorTeamID() << " " << gs_._runs_visitor << " ";
	os_ << gs_._log->getHomeTeamID() << " " << gs_._runs_home << std::endl;

	//Print defensive players if this is the first play of an inning
	if (gs_.isFirstPlayOfHalfInning()) {
		gs_.printDefensivePlayers(os_);
		os_ << std::endl;
	}

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