#include "PlayRecord.h"
#include "EventInterpretation.h"
#include "BaseballDatabase.h"
#include <boost/algorithm/string.hpp>
#include <ctype.h>

using namespace std;

//Create from a pointer to a play object
PlayRecord::PlayRecord(const Play* play_, const BaseballDatabase* const db_) : Play(*play_) , _db(db_)
{
	//Add batter
	addBatter(_db->getPlayers());

	//Parse play
	_batting_result = parseBattingResult(getEventRaw());
}

void PlayRecord::addBatter(const vector<Player>& players_)
{
	//Loop over players_ and find match
	for (auto&& player : players_) {
		if (boost::equal(getBatterID(), player.getID())) {
			//These are equal, add the pointer
			_batter = &player;
		}
	}
}

BattingResult PlayRecord::parseBattingResult(string play_string_)
{
	for (auto&& e : EventInterpretation::InterpretationArray) {
		if (e == play_string_) return e.getBattingResult();
	}
	cout << play_string_ << " not recognized" << endl;
	throw exception("Play not recognized");
	return BattingResult::NOT_RECOGNIZED;
}

ostream & operator<<(ostream & os, const PlayRecord & p)
{
	os << *(p._batter) << " " << BattingResultString[p._batting_result];
	return os;
}

//Get the number of at bats
int PlayRecord::getNumberAtBats() const
{
	//Check if this was a valid at bat
	switch (_batting_result) {
	case GROUND_OUT: 
	case FLY_OUT:
	case STRIKE_OUT: 
	case SINGLE:
	case DOUBLE:
	case TRIPLE:
	case HR:
		//These are all valid at bats, return 1
		return 1;
	case WALK:
	case ERROR:
	case NO_PLAY:
		//These are not at bats
		return 0;
	default:
		cout << "Play " << BattingResultString[_batting_result] << " not recognized by getNumberAtBats" << endl;
		throw exception("Error in getNumberAtBats");
	}

	//Should never be here
	return -1;
}
int PlayRecord::getNumberHits() const
{
		//Check if this was a hit
		switch (_batting_result) {
		case SINGLE:
		case DOUBLE:
		case TRIPLE:
		case HR:
			//These are all valid at bats, return 1
			return 1;
		case STRIKE_OUT:
		case GROUND_OUT:
		case FLY_OUT:
		case WALK:
		case ERROR:
		case NO_PLAY:
			//These are nothits
			return 0;
		default:
			cout << "Play " << BattingResultString[_batting_result] << " not recognized by getNumberAtBats" << endl;
			throw exception("Error in getNumberHits");
		}
		//Should never be here
		return -1;
}