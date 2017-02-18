// SportsStats.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "stdafx.h"
#include "GameSet.h"
#include "TeamSet.h"
#include "BaseballDatabase.h"

#define GAME_LOG_FILE "C:\\Users\\micro\\OneDrive\\Documents\\BaseballStats\\2014PS\\2014WS.EVE"
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

		//Print Players
		db.printPlayerList();

		//Print Plays
		db.printPlayList();

		//Print averages
		db.printAllBattingAverages();

		//Print slugging
		db.printAllSluggingPercentages();

	} catch (std::exception& e) {
		//Print exception information
		cout << "ERROR: " << endl << e.what() << endl;
		//Return 1 for error
		return 1;
	}
	//Return 0 for success
    return 0;
}

