// SportsStats.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "stdafx.h"
#include "GameSet.h"
#include "TeamSet.h"
#include "BaseballDatabase.h"

//Goal: Parse all information available in the game logs
//
// To Do (in order):
// 1. Track defensive players in game state - DONE
// 2. Track substitutions - DONE
// 3. Track hit locations - DONE
//         a. from hits
//         b. from modifiers - I only see them for homeruns
// 4. Track which defensive player makes plays/errors
// 5. Handle "badj" lines, these come before the bitter and say he batted from an unexpected side

//
// Complicated Issues Encountered
//
//1. DH moves to field, so pitcher needs to be deleted from batting position 0
//   Case in the top of the 8th here: http://www.baseball-reference.com/boxes/OAK/OAK201604070.shtml
//
//2. Trailing / with no modifier following in play.  This is an error in the log, but I deal with it
//   See play,4,1,beltb001,11,SBX,7/F/ in 2016 Giants
//

//2014 World Series
//#define GAME_LOG_FILE_2014_WS "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\2014PS\\2014WS.EVE"

//Path to files
#define GAME_LOG_DIR "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS"
//List of files in GAME_LOG_DIR
#define LOG_FILE_LIST "event_log_file_list.txt"

//Common to all 
#define TEAM_LIST_FILE  "TeamList.txt"

//Flag to control how much is printed
#define BUILD_DB_ONLY =1

using namespace std;

int main()
{
	try {
		//Create TeamSet
		TeamSet ts(TEAM_LIST_FILE);

		//Print team list
		ts.printTeamList();

		////Create log file list for testing
		//StringVector log_files;
		//log_files.push_back(GAME_LOG_FILE_2014_WS);

		//Create path to log file containing many seasons
		string list_file_path(GAME_LOG_DIR);
		list_file_path.append("\\");
		list_file_path.append(LOG_FILE_LIST);

		std::cout << "Loading log file list from: " << list_file_path << std::endl;
		FileBasedObject file_list(list_file_path);

		//Loop over each line in the list and add it to the vector of files to be parsed
		StringVector log_files;
		for (auto&& filename : file_list.getLines()) {
			std::string path_full(GAME_LOG_DIR);
			path_full.append("\\");
			path_full.append(filename);
			log_files.push_back(path_full);
		}

		//Create game set
		GameSet gs(log_files, &ts);

		//Create database
		BaseballDatabase db(&ts, &gs);

		//Check for early exit
#ifdef BUILD_DB_ONLY
		return 0;
#endif

		//Run everyting (sort of a systest)
		//Print Players
		std::cout << std::endl << "PLAYERS" << std::endl;
		db.printPlayerList();
		//Print averages
		std::cout << std::endl << "BATTING AVERAGES" << std::endl;
		db.printAllBattingAverages();
		//Print slugging
		std::cout << std::endl << "SLUGGING PERCENTAGES" << std::endl;
		db.printAllSluggingPercentages();
		//Print Plays
		std::cout << std::endl << "PLAYS" << std::endl;
		db.printPlayList();

		//Make another database to see printing when it happens
		std::cout << std::endl << "DATABASE CREATION" << std::endl;
		BaseballDatabase db2(&ts, &gs);

	} catch (std::exception& e) {
		//Print exception information
		cout << "ERROR: " << endl << e.what() << endl;
		//Return 1 for error
		return 1;
	}
	//Return 0 for success
    return 0;
}

