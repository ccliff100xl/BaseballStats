#include "ModifierInterpretation.h"
#include <boost/algorithm/string/erase.hpp>
#include <iostream>
using namespace std;

bool operator==(const std::string log_pattern_, const ModifierInterpretation & modifier_interpretation_)
{
	//Get string to compare to 
	const string& modifier_string = modifier_interpretation_._log_pattern;

	//Remove ! (exceptional play) and ? (uncertainty) source: http://www.retrosheet.org/eventfile.htm
	std::string log_pattern = log_pattern_;
	boost::erase_all(log_pattern, "!");
	boost::erase_all(log_pattern, "?");

	//If it makes it here, the modifier_string is char(s), get leading string from log_pattern
	const size_t i_digit = log_pattern.find_first_of("0123456789");
	//If i_digit is 0, it means the first char is a number, which means hit location
	if (i_digit == 0) {
		if (modifier_interpretation_.getModifier() == PM_HL) {
			return true;
		}
		else {
			return false;
		}
	}

	const size_t i_end = log_pattern.find_first_of("0123456789.+(-");
	//If i_digit == string::npos, then nothing was found so the whole string should be compared
	std::string play_str;
	if (i_end == string::npos) {
		//Use full string
		play_str = log_pattern;
	} else {
		//Get substring from log
		play_str = log_pattern.substr(0, i_end);
	}

	//Return if this matches
	if (play_str == modifier_string) {
		return true;
	} else {
		return false;
	}
}

bool operator==(const ModifierInterpretation& modifier_interpretation_, const std::string log_pattern_)
{
	return log_pattern_ == modifier_interpretation_;
}

//Make objects for each result
//Actual hit locations are not saved
const std::vector<ModifierInterpretation> ModifierInterpretation::InterpretationArray = {
	ModifierInterpretation("AP",PM_AP), //appeal play
	ModifierInterpretation("BF",PM_BF), //bunt foul (K)
	ModifierInterpretation("BG",PM_BG), //ground ball bunt
	ModifierInterpretation("BGDP",PM_BGDP), //bunt grounded into double play
	ModifierInterpretation("BINT",PM_BINT), //batter interference
	ModifierInterpretation("BL",PM_BL), //line drive bunt
	ModifierInterpretation("BOOT",PM_BOOT), //batting out of turn
	ModifierInterpretation("BP",PM_BP), //bunt pop up
	ModifierInterpretation("BPDP",PM_BPDP), //bunt popped into double play
	ModifierInterpretation("BR",PM_BR), //runner hit by batted ball
	ModifierInterpretation("C",PM_C), //called third strike
	ModifierInterpretation("COUB",PM_COUB), //courtesy batter
	ModifierInterpretation("COUF",PM_COUF), //courtesy fielder
	ModifierInterpretation("COUR",PM_COUR), //courtesy runner
	ModifierInterpretation("DP",PM_DP), //unspecified double play
	ModifierInterpretation("F",PM_F), //fly
	ModifierInterpretation("FDP",PM_FDP), //fly ball double play
	ModifierInterpretation("FINT",PM_FINT), //fan interference
	ModifierInterpretation("FL",PM_FL), //foul
	ModifierInterpretation("FO",PM_FO), //force out
	ModifierInterpretation("G",PM_G), //ground ball
	ModifierInterpretation("GDP",PM_GDP), //ground ball double play
	ModifierInterpretation("GTP",PM_GTP), //ground ball triple play
	ModifierInterpretation("IF",PM_IF), //infield fly rule
	ModifierInterpretation("INT",PM_INT), //interference
	ModifierInterpretation("IPHR",PM_IPHR), //inside the park home run
	ModifierInterpretation("L",PM_L), //line drive
	ModifierInterpretation("LDP",PM_LDP), //lined into double play
	ModifierInterpretation("LTP",PM_LTP), //lined into triple play
	ModifierInterpretation("MREV",PM_MREV), //manager challenge of call on the field
	ModifierInterpretation("NDP",PM_NDP), //no double play credited for this play
	ModifierInterpretation("OBS",PM_OBS), //obstruction(fielder obstructing a runner)
	ModifierInterpretation("P",PM_P), //pop fly
	ModifierInterpretation("PASS",PM_PASS), //a runner passed another runner and was called out
	ModifierInterpretation("RINT",PM_RINT), //runner interference
	ModifierInterpretation("SF",PM_SF), //sacrifice fly
	ModifierInterpretation("SH",PM_SH), //sacrifice hit(bunt)
	ModifierInterpretation("TH",PM_TH), //throw
	ModifierInterpretation("TP",PM_TP), //unspecified triple play
	ModifierInterpretation("UINT",PM_UINT), //umpire interference
	ModifierInterpretation("UREV",PM_UREV), //umpire review of call on the field
	ModifierInterpretation("",PM_HL), //The hit location is given by a number, this needs special handling
};