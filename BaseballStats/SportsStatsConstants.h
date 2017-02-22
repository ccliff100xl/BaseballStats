#pragma once

#include <map>
//This file is used to hold constants common to many parts of the program

//Define enum for home and visiting teams (int conversion matches retorsheet)
enum TeamType { TEAM_VISITOR = 0, TEAM_HOME = 1  };

//Define Batting Results, these are not just batting results I guess, should be called event result
//or something
enum EventResult {
	GROUND_OUT,
	FLY_OUT,
	STRIKE_OUT,
	CATCHER_INTERFERENCE,
	CAUGHT_STEALING,
	WALK,
	SINGLE,
	DOUBLE,
	TRIPLE,
	HR,
	NO_PLAY,
	ERROR,
	FIELDERS_CHOICE,
	STOLEN_BASE,
	NUMERIC_UNCERTAIN, //This may be GROUND_OUT, FLY_OUT, etc.  Determined in Event::Event()
	NOT_RECOGNIZED,
	NOT_PARSED
};

//Define readable names for batting results
static std::map< EventResult, const char * > BattingResultString = {
	{ GROUND_OUT, "Ground Out" },
	{ FLY_OUT, "Fly Out" },
	{ STRIKE_OUT, "Strike Out" },
	{ CATCHER_INTERFERENCE, "Catcher Interference" },
	{ CAUGHT_STEALING, "Caught Stealing" },
	{ WALK, "Walk" },
	{ SINGLE, "Single" },
	{ DOUBLE, "Double" },
	{ TRIPLE, "Triple" },
	{ HR, "Home Run" },
	{ NO_PLAY, "No Play" },
	{ FIELDERS_CHOICE, "Fielders Choice" },
	{ STOLEN_BASE, "Stolen Base" },
	{ ERROR, "Error" },
	{ NUMERIC_UNCERTAIN, "<NUMERIC_UNCERTAIN>" },
	{ NOT_RECOGNIZED, "<NOT_RECOGNIZED>" },
	{ NOT_PARSED, "<NOT_PARSED>" }
};

enum HalfInning {
	INNING_TOP,
	INNING_BOTTOM
};


//Define enum for modifiers
// "Play modifiers and explanations" from http://www.retrosheet.org/eventfile.htm
enum PlayModifier {
	PM_AP,    //appeal play
	PM_BG,    //ground ball bunt
	PM_BGDP,  //bunt grounded into double play
	PM_BINT,  //batter interference
	PM_BL,    //line drive bunt
	PM_BOOT,  //batting out of turn
	PM_BP,    //bunt pop up
	PM_BPDP,  //bunt popped into double play
	PM_BR,    //runner hit by batted ball
	PM_C,     //called third strike
	PM_COUB,  //courtesy batter
	PM_COUF,  //courtesy fielder
	PM_COUR,  //courtesy runner
	PM_DP,    //unspecified double play
	//PM_E$,    //error on $
	PM_F,     //fly
	PM_FDP,   //fly ball double play
	PM_FINT,  //fan interference
	PM_FL,    //foul
	PM_FO,    //force out
	PM_G,     //ground ball
	PM_GDP,   //ground ball double play
	PM_GTP,   //ground ball triple play
	PM_IF,    //infield fly rule
	PM_INT,   //interference
	PM_IPHR,  //inside the park home run
	PM_L,     //line drive
	PM_LDP,   //lined into double play
	PM_LTP,   //lined into triple play
	PM_MREV,  //manager challenge of call on the field
	PM_NDP,   //no double play credited for this play
	PM_OBS,   //obstruction(fielder obstructing a runner)
	PM_P,     //pop fly
	PM_PASS,  //a runner passed another runner and was called out
	//R$,  //relay throw from the initial fielder to $ with no out made
	PM_RINT,  //runner interference
	PM_SF,    //sacrifice fly
	PM_SH,    //sacrifice hit(bunt)
	PM_TH,    //throw
	//TH%, //throw to base %
	PM_TP,    //unspecified triple play
	PM_UINT,  //umpire interference
	PM_UREV,  //umpire review of call on the field
	PM_HL // modifier is hit location, actual location will have to be saved in some other way
};

//Define readable names for batting results
static std::map< PlayModifier, const char * > PlayModifierString = {
	//{ GROUND_OUT, "Ground Out" },
	{ PM_AP,    "appeal play" },
	{ PM_BG,    "ground ball bunt" },
	{ PM_BGDP,  "bunt grounded into double play" },
	{ PM_BINT,  "batter interference" },
	{ PM_BL,    "line drive bunt" },
	{ PM_BOOT,  "batting out of turn" },
	{ PM_BP,    "bunt pop up" },
	{ PM_BPDP,  "bunt popped into double play" },
	{ PM_BR,    "runner hit by batted ball" },
	{ PM_C,     "called third strike" },
	{ PM_COUB,  "courtesy batter" },
	{ PM_COUF,  "courtesy fielder" },
	{ PM_COUR,  "courtesy runner" },
	{ PM_DP,    "unspecified double play" },
	{ PM_F,     "fly" },
	{ PM_FDP,   "fly ball double play" },
	{ PM_FINT,  "fan interference" },
	{ PM_FL,    "foul" },
	{ PM_FO,    "force out" },
	{ PM_G,     "ground ball" },
	{ PM_GDP,   "ground ball double play" },
	{ PM_GTP,   "ground ball triple play" },
	{ PM_IF,    "infield fly rule" },
	{ PM_INT,   "interference" },
	{ PM_IPHR,  "inside the park home run" },
	{ PM_L,     "line drive" },
	{ PM_LDP,   "lined into double play" },
	{ PM_LTP,   "lined into triple play" },
	{ PM_MREV,  "manager challenge of call on the field" },
	{ PM_NDP,   "no double play credited for this play" },
	{ PM_OBS,   "obstruction(fielder obstructing a runner)" },
	{ PM_P,     "pop fly" },
	{ PM_PASS,  "a runner passed another runner and was called out" },
	{ PM_RINT,  "runner interference" },
	{ PM_SF,    "sacrifice fly" },
	{ PM_SH,    "sacrifice hit(bunt)" },
	{ PM_TH,    "throw" },
	{ PM_TP,    "unspecified triple play" },
	{ PM_UINT,  "umpire interference" },
	{ PM_UREV,  "umpire review of call on the field" },
    { PM_HL,    "<hit location>" }
};