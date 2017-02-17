#include "GameState.h"
#include "PlayRecord.h"
#include <string>
#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>

void GameState::updateStateFromPlay(const PlayRecord* play_)
{
	//Update baserunners based on event string: http://www.retrosheet.org/eventfile.htm#5
	//play, 3, 0, fielc001, 00, X, S7 / L7LD.3 - H; 2 - H; BX2(7E4)
	std::string event_string = play_->getEventRaw();

	//Find .
	std::vector<std::string> line_parsed_period;
	boost::split(line_parsed_period, event_string, boost::is_any_of("."));

	//If no "." was found, just return without doing anything
	if (line_parsed_period.size() < 2 ) return;

	//At least one player movement was found, copy current runners to backup
	const Player* br1 = _baserunner_1;
	const Player* br2 = _baserunner_2;
	const Player* br3 = _baserunner_3;

	//Baserunning information seperated by ;
	std::vector<std::string> line_parsed_semi;
	boost::split(line_parsed_semi, line_parsed_period[1], boost::is_any_of(";"));
	//Loop over eacch movement string
	for (auto&& m_str : line_parsed_semi) {
		//Only three chars need to be parsed
		//0 : starting base
		//1 : - safe advance, X out
		//2 : ending base
		// Additional chars are FYI about who did what

		//Make sure there actually are three chars
		if (m_str.size() < 3) {
			std::cout << "GameState::updateStateFromPlay: " << m_str << std::endl;
			throw std::exception("GameState::updateStateFromPlay movement has < 3 chars");
		}

		//Determine starting player, and clear base of starting player
		const Player* p_start = NULL;
		switch (m_str[0]) {
		case 'B': 
			p_start = _batter; 
			break;
		case '1' : 
			p_start = br1;
			_baserunner_1 = NULL;
			break;
		case '2':
			p_start = br2;
			_baserunner_2 = NULL;
			break;
		case '3':
			p_start = br3;
			_baserunner_3 = NULL;
			break;
		default:
			std::cout << "GameState::updateStateFromPlay: " << m_str[0] << std::endl;
			throw std::exception("GameState::updateStateFromPlay movement invalid start");
		}

		//TODO: actually add to outs and runs here

		//Check if player advances, or is out
		if (m_str[1] == 'X') {
			//This is an out, just return
			return;
		}

		//If this is not an out, it must be an advance, but make sure
		if (m_str[1] != '-') {
			std::cout << "GameState::updateStateFromPlay: " << m_str[1] << std::endl;
			throw std::exception("GameState::updateStateFromPlay movement invalid middle char");
		}

		//It's an advance, update baserunners
		switch (m_str[2]) {
		case '1':
			_baserunner_1 = p_start;
			break;
		case '2':
			_baserunner_2 = p_start;
			break;
		case '3':
			_baserunner_3 = p_start;
			break;
		case 'H':
			//add run here
			break;
		default:
			std::cout << "GameState::updateStateFromPlay: " << m_str[2] << std::endl;
			throw std::exception("GameState::updateStateFromPlay movement invalid end");
		}
		
	}
}
