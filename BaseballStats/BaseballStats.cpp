// SportsStats.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <iomanip>
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
		//db.printPlayList();

		//Print the results of the games played
		//gs.printGameResults();

		//Print averages
		int n_atbats = -1;
		int n_hits   = -1 ;
		std::string playerid("poseb001");
		double average = db.getPlayerBattingAverage(playerid, n_atbats, n_hits);
		const Player player = db.getPlayer(playerid);
		cout << player << " " << n_hits << "/" << n_atbats << " " << std::setprecision(3) <<average << endl;
	}
	catch (std::exception& e) {
		//Print exception information
		cout << "ERROR: " << endl << e.what() << endl;
		//Return 1 for error
		return 1;
	}
	//Return 0 for success
    return 0;
}

