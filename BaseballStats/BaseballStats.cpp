// SportsStats.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "stdafx.h"
#include "GameSet.h"
#include "TeamSet.h"
#include "BaseballDatabase.h"

//Current Issue: File 948 1983SEA.EVA
//Need to handle CSH(13E2)(UR).3-H;1-2 in the special caught stealing section.
//I think it's including the ; in the play, but it should stop at the period I think
//Line 201 of Event.cpp

//Issue Fixed 2/11/18: Need to handle this play from 1997FLO.EVN
//play, 1, 0, lockk001, 22, FCBB1, POCS2(134); CSH(42) / DP
//Cannot handle picked off caught stealing and caught stealing in the same row.
//Combined handling of caught stealing, picked off, and picked off caught stealing
//int he same section of Event()

//Issue on 8/3/17, fixed in GameState::updateBaserunners by comparind the positions that
//made the out to the positions that made the error
// Stuck on this play: play,4,1,lambj001,01,CX,5/P5F/NDP/SF.3-H;2XH(26)(E5/TH);1-2
// in 2015ARI.EVN. Two outs are made on the play, but I an not counting the out E5
// because I'm saying it's an error and not an out.  I think there was a fly ball to 3rd, 
// he threw home (badly, an error) so the runner from 2nd had a chance to score but 
// was out at the plate.  So, after 2XH if there is an error, but also an additional play
// it will turn the play back into an out

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
// 6. Add comments which are before any play to the GameLog some how
// 7. Support info,gwrbi,chamc001, given at the beginning of a game

//
// Complicated Issues Encountered
//
//1. DH moves to field, so pitcher needs to be deleted from batting position 0
//   Case in the top of the 8th here: http://www.baseball-reference.com/boxes/OAK/OAK201604070.shtml
//
//2. Trailing / with no modifier following in play.  This is an error in the log, but I deal with it
//   See play,4,1,beltb001,11,SBX,7/F/ in 2016 Giants
//
//3. Single ? for count when unknown, should be ?? (see play,6,0,koenm101,?,,9 in 1927WS1.EVA)
//
//4. Nothing for count when unknown, should be ?? (See play,1,0,adams101,,,43 in 1930BRO.EVN)
//
//5. Missing version, 1943PHA.EVA, just set to -1
//
//6. Comma inside quotes, I deleted comment IN THE FILES 1944SLN.EVN, 1949NY1.EVN
//
// 7. 'b' instead of 1 or 0 for inning, 1963KC1.EVA, play,5,b,gardb101,??,,NP (b changed to 0 in file)

//If GAME_LOG_FILE is defined it will be used
//2014 World Series
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\2014PS\\2014WS.EVE"
//File with Error (single ?)
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1927WS1.EVA"
//File with Error (missing ?? or ?)
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1930BRO.EVN"
//Files with Error (invalid stoi argument)
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1943PHA.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1944SLN.EVN"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1949NY1.EVN"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1963KC1.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1964NYA.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1965CLE.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\2015BAL.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\2015CHA.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\2015DET.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\2016CHA.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\2016PHI.EVN"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1922NY1.EVN"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1925NY1.EVN"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1937CIN.EVN"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1938SLA.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1941PHA.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1946DET.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1947BRO.EVN"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1947NY1.EVN"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\2013MIL.EVN"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\2007SEA.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1997FLO.EVN"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1996HOU.EVN"

//File which was missing a version line, updated code to not require it
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1978CAL.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1999ANA.EVA"
//File which had comments at the beginning of the file
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1991ATL.EVN"
//First file with "padj"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\1995MON.EVN"

//Files used to debug game tracking
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\2015ANA.EVA"
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS\\2015ARI.EVN"

//Path to files
#define GAME_LOG_DIR "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\ALL_REGULAR_SEASONS"
//List of files in GAME_LOG_DIR
#define LOG_FILE_LIST "event_log_file_list.txt"
//#define LOG_FILE_LIST "event_log_file_list_2016.txt"

//Common to all 
#define TEAM_LIST_FILE  "TeamList.txt"

//Flag to control how much is printed (comment out to disable)
#define BUILD_DB_ONLY =1

using namespace std;

int main()
{
	try {
		//Create TeamSet
		TeamSet ts(TEAM_LIST_FILE);

		//Print team list
		ts.printTeamList();

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
		//Define which file to start with
		//This will be the first file to actually process, zero processes all
		const long i_file_start = 948;

		//Parse files individually to find problems faster 
		const long n_files = static_cast<long>( log_files.size() );
		for (long ifile = i_file_start; ifile < n_files; ifile++) {
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
			BaseballDatabase db(&ts, &gs);

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

		//Print success to be clear
		std::cout << "Exiting without error" << std::endl;
		return 0;

	} catch (std::exception& e) {
		//Print exception information
		cout << "ERROR: " << endl << e.what() << endl;
		//Return 1 for error
		return 1;
	}
	//Print success to be clear
	std::cout << "Exiting without error" << std::endl;
	//Return 0 for success
    return 0;
}

