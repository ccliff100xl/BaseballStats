#include "GameSet.h"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>

using namespace std;

//Game sets can only be created from text files
GameSet::GameSet(string logs_file_path_, TeamSet* team_set_) : _team_set(team_set_)
{
	//Open file
	cout << "Parsing File " << logs_file_path_ << endl;
	ifstream game_log_file(logs_file_path_);
	//Check if file was opened
	if (!game_log_file.is_open()) {
		throw exception("Could not open file");
	}

	//Loop over lines of the file
	//Possible lines that need to be handled
	//  id,<gameid_string> //This is the first line of a game
	//  version, <version_int> //This is the second line of a game
	//  All other lines are handled by GameLog object
	string line; //Hold each line
	bool parsing_game = false; //True if parsing a game, false if about to parse a new game
	while (getline(game_log_file, line))
	{
		//Print line
		//cout << line << '\n';

		//If parsing game, just pass line
		if (parsing_game) {
			//Parse line in current log, false return means game is done
			parsing_game = _logs.back().parseLogLine(line);
		}

		//If not parsing game, start a new game
		if (!parsing_game) {
			//Parse line
			vector<string> line_parsed;
			boost::split(line_parsed, line, boost::is_any_of(","));
			//This must be id,<gameid_string>
			if (!boost::equal(line_parsed[0], string("id"))) {
				//This is an error
				throw exception("First line of game log must be 'id'");
			}
			//First line is id, save it
			string game_id = line_parsed[1];

			//Get second line, which is version
			getline(game_log_file, line);
			boost::split(line_parsed, line, boost::is_any_of(","));
			int game_version = stoi(line_parsed[1]);

			//Create GameLog object
			GameLog game_log(game_id, game_version);
			//Copy game to log
			_logs.push_back(game_log);
			parsing_game = true;
		}
	}
	cout << "Games Parsed: " << _logs.size() << endl;
	//Done with file, close it
	game_log_file.close();

}

