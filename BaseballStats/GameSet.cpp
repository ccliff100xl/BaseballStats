#include "GameSet.h"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>

using namespace std;

//Game sets can only be created from text files
GameSet::GameSet(const StringVector logs_file_paths_, TeamSet* team_set_) : _team_set(team_set_)
{
	//Loop over log files
	for (auto&& log : logs_file_paths_) {
		//Open file
		cout << "Parsing File " << log << endl;
		ifstream game_log_file(log);
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
			//DEBUG !! Print line !! DEBUG
			//std::cout << line << std::endl;

			//If parsing game, just pass line
			if (parsing_game) {
				//Parse line in current log, false return means game is done
				parsing_game = _logs.back().parseLogLine(line);
			}

			//If not parsing game, start a new game
			if (!parsing_game) {
				//Parse line
				StringVector line1_parsed = SplitStringToVector(line, ",");
				//Loop over comments, before id,<gameid_string>
				while (line1_parsed[0] != "id") {
					//Allow a comment
					if (line1_parsed[0] == "com") {
						//Print as info
						std::cout << "Comment at beginning of line: " << line1_parsed[1] << std::endl;
					}
					else {
						//This is an error
						throw exception("First line of game log must be 'id'");
					}
					//Get the next line
					getline(game_log_file, line);
					line1_parsed = SplitStringToVector(line, ",");
				}
				//First line is id, save it
				string game_id = line1_parsed[1];

				//Version is sometimes missing and can always be ignored
				//So, don't try to parse
				////Get second line, which should be version version
				//getline(game_log_file, line);
				//StringVector line2_parsed = SplitStringToVector(line, ",");
				//if (line2_parsed[0] != "version") {
				//	//Sometimes version is missing, in this case just break and start parsing

				//	////This may be an info line, print it and continue
				//	//std::cout << "Ignoring out of place: " << line << std::endl;
				//	////Get next line
				//	//getline(game_log_file, line);
				//	//line2_parsed = SplitStringToVector(line, ",");
				//}
				//int game_version = -1;
				//if (line2_parsed.size() > 1) {
				//	//Read version if it's there
				//     game_version = string2int(line2_parsed[1]);
				//} 

				//Create GameLog object
				const int game_version = 1;//harcoded, doesn't matter, could be removed
				GameLog game_log(game_id, game_version);

				//Copy game to log
				_logs.push_back(game_log);
				parsing_game = true;
			}
		}
		//Done with file, close it
		game_log_file.close();
	}
	cout << "Games Parsed: " << _logs.size() << endl;
}

