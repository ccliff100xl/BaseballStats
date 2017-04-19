#pragma once

#include <map>
//This file is used to hold constants common to many parts of the program

//Define enum for home and visiting teams (int conversion matches retorsheet)
enum TeamType { TEAM_VISITOR = 0, TEAM_HOME = 1  };

//These are in the order on the website: http://www.retrosheet.org/eventfile.htm
enum EventResult {
	GROUND_OUT,
	FLY_OUT,
	CATCHER_INTERFERENCE,
	SINGLE,
	DOUBLE,
	TRIPLE,
	GROUND_RULE_DOUBLE,
	ERROR,
	FIELDERS_CHOICE,
	ERROR_ON_FOUL_BALL,
	HR,
	HIT_BY_PITCH,
	STRIKE_OUT,
	NO_PLAY,
	WALK,
	INTENTIONAL_WALK,
	BALK,
	CAUGHT_STEALING,
	DEFENSIVE_INDIFFERENCE,
	UNKNOWN_ADVANCE,
	PASSED_BALL,
	WILD_PITCH,
	PICKED_OFF,
	PICKED_OFF_CAUGHT_STEALING,
	STOLEN_BASE,
	NUMERIC_UNCERTAIN, //This may be GROUND_OUT, FLY_OUT, etc.  Determined in Event::Event()
	NOT_RECOGNIZED,
	NOT_PARSED
};

//Define readable names for batting results
static std::map< EventResult, const char * > BattingResultString = {
	{ GROUND_OUT, "Ground Out" },
	{ FLY_OUT, "Fly Out" },
	{ CATCHER_INTERFERENCE, "Catcher Interference" },
	{ SINGLE, "Single" },
	{ DOUBLE, "Double" },
	{ TRIPLE, "Triple" },
	{ GROUND_RULE_DOUBLE, "Ground Rule Double" },
	{ ERROR, "Error" },
	{ FIELDERS_CHOICE, "Fielders Choice" },
	{ ERROR_ON_FOUL_BALL, "Error on Foul Ball" },
	{ HR, "Home Run" },
	{ HIT_BY_PITCH, "Hit by Pitch" },
	{ STRIKE_OUT, "Strike Out" },
	{ NO_PLAY, "Substitution" }, //The notes say NP is always substitution
	{ WALK, "Walk" },
	{ INTENTIONAL_WALK, "Intentional Walk" },
	{ BALK, "Balk" },
	{ CAUGHT_STEALING, "Caught Stealing" },
	{ DEFENSIVE_INDIFFERENCE, "Defensive Indifference" },
	{ UNKNOWN_ADVANCE, "Advance (Unknown)" },
	{ PASSED_BALL, "Passed Ball" },
	{ WILD_PITCH, "Wild Pitch" },
	{ PICKED_OFF, "Picked Off" },
	{ PICKED_OFF_CAUGHT_STEALING, "Picked Off (Caught Stealing)" },
	{ STOLEN_BASE, "Stolen Base" },
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
	PM_BF,    //bunt foul (strike out)
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
	PM_E,     //error on $
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
	PM_R,  //relay throw from the initial fielder
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
	{ PM_AP,    "appeal play" },
	{ PM_BF,    "bunt foul" },
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
	{ PM_E,     "error" },
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
	{ PM_R,     "relay throw" },
	{ PM_RINT,  "runner interference" },
	{ PM_SF,    "sacrifice fly" },
	{ PM_SH,    "sacrifice hit(bunt)" },
	{ PM_TH,    "throw" },
	{ PM_TP,    "unspecified triple play" },
	{ PM_UINT,  "umpire interference" },
	{ PM_UREV,  "umpire review of call on the field" },
    { PM_HL,    "<hit location>" }
};

//Define enums for defensive positions
enum DefensivePosition {
	UNKNOWN_DEFENSIVE_POSITION = -1,
	PITCHER = 0,
	CATCHER,
	FIRST_BASEMAN,
	SECOND_BASEMAN,
	THIRD_BASEMAN,
	SHORT_STOP,
	LEFT_FIELD,
	CENTER_FIELD,
	RIGHT_FIELD,
	DESIGNATED_HITTER,
	PINCH_HITTER,
	PINCH_RUNNER
};

//Define readable names for defensive positions
static std::map< DefensivePosition, const char * > DefensivePositionString = {
	{ UNKNOWN_DEFENSIVE_POSITION, "<unknown>" },
	{ PITCHER, "Pitcher" },
	{ CATCHER, "Catcher" },
	{ FIRST_BASEMAN, "First Baseman" },
	{ SECOND_BASEMAN, "Second Baseman" },
	{ THIRD_BASEMAN, "Third Baseman" },
	{ SHORT_STOP, "Short Stop" },
	{ LEFT_FIELD, "Left Field" },
	{ CENTER_FIELD, "Center Field" },
	{ RIGHT_FIELD, "Right Field" },
	{ DESIGNATED_HITTER, "Designated Hitter" },
	{ PINCH_HITTER, "Pinch Hitter" },
	{ PINCH_RUNNER, "Pinch Runner" },
};