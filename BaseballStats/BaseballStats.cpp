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
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\2014PS\\2014WS.EVE"
#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\2014PS\\2014WS_Game1Trim.EVE"

//2016 Oakland A's
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\2016RS\\2016OAK.EVA"

//2016 Giants
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\2016RS\\2016SFN.EVN"

//Common to all 
#define TEAM_LIST_FILE  "TeamList.txt"

//Flag to control how much is printed
//#define BUILD_DB_ONLY =1

using namespace std;

int main()
{
	try {
		//Create TeamSet
		TeamSet ts(TEAM_LIST_FILE);

		//Print team list
		ts.printTeamList();

		//Create a game set object
		GameSet gs(GAME_LOG_FILE, &ts);

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

