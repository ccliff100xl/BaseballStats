#pragma once

#include <map>
//This file is used to hold constants common to many parts of the program

//Define enum for home and visiting teams (int conversion matches retorsheet)
enum TeamType { TEAM_VISITOR = 0, TEAM_HOME = 1  };

//Define Batting Results
enum BattingResult {
	GROUND_OUT,
	FLY_OUT,
	STRIKE_OUT,
	CATCHER_INTERFERENCE,
	WALK,
	SINGLE,
	DOUBLE,
	TRIPLE,
	HR,
	NO_PLAY,
	ERROR,
	FIELDERS_CHOICE,
	NOT_RECOGNIZED,
	NOT_PARSED
};

//Define readable names for batting results
static std::map< BattingResult, const char * > BattingResultString = {
	{ GROUND_OUT, "Ground Out" },
	{ FLY_OUT, "Fly Out" },
	{ STRIKE_OUT, "Strike Out" },
	{ CATCHER_INTERFERENCE, "Catcher Interference" },
	{ WALK, "Walk" },
	{ SINGLE, "Single" },
	{ DOUBLE, "Double" },
	{ TRIPLE, "Triple" },
	{ HR, "Home Run" },
	{ NO_PLAY, "No Play" },
	{ FIELDERS_CHOICE, "Fielders Choice" },
	{ ERROR, "Error" },
	{ NOT_RECOGNIZED, "." },
	{ NOT_PARSED, "NOT_PARSED" }
};


