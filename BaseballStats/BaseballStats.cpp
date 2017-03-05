// SportsStats.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "stdafx.h"
#include "GameSet.h"
#include "TeamSet.h"
#include "BaseballDatabase.h"

//2014 World Series
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\2014PS\\2014WS.EVE"

//2016 Oakland A's
#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\2016RS\\2016OAK.EVA"

//2016 Giants
//#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\2016RS\\2016SFN.EVN"

//Common to all 
#define TEAM_LIST_FILE  "TeamList.txt"

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

