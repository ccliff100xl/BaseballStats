#include "GameState.h"
#include "PlayRecord.h"
#include <string>
#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>

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

	//Add a run if the batter scores earned runs
	if (play_->didBatterScoreEarnedRun()) addRunScored();
	_outs += play_->getOutsFromEvent();

	//Update baserunners (may add runs or outs)
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
		_batter = NULL;
		_baserunner_1 = NULL;
		_baserunner_2 = NULL;
		_baserunner_3 = NULL;
		if (_inning_half == INNING_TOP) {
			_inning_half = INNING_BOTTOM;
		}
		else {
			_outs = 0;
			_inning++;
			_inning_half = INNING_TOP;
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
	//std::string event_check = "62(3)/FO/G.2-3;1-2";
	//if (event_string == event_check) {
	//	std::cout << "FOUND " << event_check << std::endl;
	//}

	//Add to baserunner movements
	if (line_parsed_period.size() > 1) {
		//Baserunning information seperated by ;
		std::vector<std::string> line_parsed_semi;
		boost::split(line_parsed_semi, line_parsed_period[1], boost::is_any_of(";"));
		for (auto&& m_str : line_parsed_semi) {
			//Only three chars need to be parsed
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
				std::cout << "GameState::updateStateFromPlay: " << m_str[2] << std::endl;
				throw std::exception("GameState::updateStateFromPlay movement invalid end");
			}

			//Add BaserunnerMovement to vector
			movements.push_back(BaserunnerMovement(base_start, base_end, made_out));
		}
	}

	//Sort movements vectore before applying
	sort(movements.begin(), movements.end(), baserunnerMovementCompare);

	//Actually move the baserunners
	const Player* br1 = _baserunner_1;
	const Player* br2 = _baserunner_2;
	const Player* br3 = _baserunner_3;

	//Loop over movements, hopefully order of operations doesn't matter
	for (auto&& m : movements) {

		//Determine starting player, and clear base of starting player
		const Player* p_start = NULL;
		switch (m.getStartingBase()) {
		case 0:
			p_start = _batter;
			break;
		case 1:
			p_start = br1;
			_baserunner_1 = NULL;
			break;
		case 2:
			p_start = br2;
			_baserunner_2 = NULL;
			break;
		case 3:
			p_start = br3;
			_baserunner_3 = NULL;
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
				_baserunner_1 = p_start;
				break;
			case 2:
				_baserunner_2 = p_start;
				break;
			case 3:
				_baserunner_3 = p_start;
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
	if (gs_._baserunner_1) {
		os_ << "  " << *(gs_._baserunner_1) << " on first base " << std::endl;
	}
	if (gs_._baserunner_2) {
		os_ << "  " << *(gs_._baserunner_2) << " on second base " << std::endl;
	}
	if (gs_._baserunner_3) {
		os_ << "  " << *(gs_._baserunner_3) << " on third base " << std::endl;
	}
	return os_;
}