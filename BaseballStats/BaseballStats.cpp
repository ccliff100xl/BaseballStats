// SportsStats.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "stdafx.h"
#include "GameSet.h"
#include "TeamSet.h"
#include "BaseballDatabase.h"
#include "BaseballDatabaseSQL.h"

//Current Status 3/2/18
//Can read/write from database. Need to build up BaseballDatabaseSQL code.

//Initial Goal: Parse all information available in the game logs
//
// To Do (in order):
// 1. Track defensive players in game state - DONE
// 2. Track substitutions - DONE
// 3. Track hit locations - DONE
//         a. from hits
//         b. from modifiers - I only see them for homeruns
// 4. Track which defensive player makes plays/errors
// 5. Handle "badj" lines, these come before the bitter and say he batted from an unexpected side
// 6. Add comments which are before any play to the GameLog some how
// 7. Support info,gwrbi,chamc001, given at the beginning of a game

//Define file index (0 processes all)
#define FILE_INDEX_START 0

//Path to files (for all regular seasons)
//#define GAME_LOG_DIR "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS"
//List of files in GAME_LOG_DIR
//#define LOG_FILE_LIST "event_log_file_list.txt"
//#define LOG_FILE_LIST "event_log_file_list_2016.txt"

//Path to files (for 2014 World Series)
#define GAME_LOG_DIR "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\2014PS"
#define LOG_FILE_LIST "world_series_log_file_list.txt"

//Common to all 
#define TEAM_LIST_FILE  "TeamList.txt"

//Flag to control how much is printed (comment out to disable)
#define BUILD_DB_ONLY 1

using namespace std;

int main()
{
	//Start overall timer
	const std::clock_t start_time_main = std::clock();
	try {
		//Create SQL database to hold everything loaded and processed
		BaseballDatabaseSQL dbsql;

		//Create TeamSet
		TeamSet ts(TEAM_LIST_FILE);

		//Print team list
		//ts.printTeamList();

		//Object to hold all file paths
		StringVector log_files;

#ifdef GAME_LOG_FILE
		std::cout << "LOADING SINGLE FILE ONLY: " << GAME_LOG_FILE << std::endl;
		////Create log file list for testing
		log_files.push_back(GAME_LOG_FILE);
#else
		//Create path to log file containing many seasons
		string list_file_path(GAME_LOG_DIR);
		list_file_path.append("\\");
		list_file_path.append(LOG_FILE_LIST);

		std::cout << "Loading log file list from: " << list_file_path << std::endl;
		FileBasedObject file_list(list_file_path);

		//Loop over each line in the list and add it to the vector of files to be parsed
		for (auto&& filename : file_list.getLines()) {
			std::string path_full(GAME_LOG_DIR);
			path_full.append("\\");
			path_full.append(filename);
			log_files.push_back(path_full);
		}
#endif
		//Parse files individually to find problems faster 
		const long n_files = static_cast<long>( log_files.size() );
		for (long ifile = FILE_INDEX_START; ifile < n_files; ifile++) {
			//Check of this should be loaded
			std::cout << "Reading File " << ifile << std::endl;
			//Try last files first
			const long i_array = n_files - ifile - 1;
			const std::string filepath = log_files[i_array];
			//Make vector with just one file to match interface
			StringVector log_file_single;
			log_file_single.push_back(filepath);
			//Create game set
			GameSet gs(log_file_single, &ts);

			//Create database
			std::cout << "Creating BaseballDatabase from GameSet and TeamSet" << std::endl;
			BaseballDatabase db(&ts, &gs, &dbsql);

#ifndef BUILD_DB_ONLY
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
#endif // !BUILD_DB_ONLY
			
		}

		//Database created, do operations
		std::vector<EventInfoSql>& events = dbsql.getEventsForPlayer("poseb001");
		//Print as test
		for (auto&& e : events) {
			std::cout << e << std::endl;
		}


	} catch (std::exception& e) {
		//Print exception information
		cout << "ERROR: " << endl << e.what() << endl;
		//Return 1 for error
		printRuntime(start_time_main);
		return 1;
	}

	//Print the total runtime
	printRuntime(start_time_main);

	//Print success to be clear
	//Return 0 for success
	std::cout << "Exiting without error" << std::endl;
    return 0;
}

