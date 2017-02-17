#include "PlayRecord.h"
#include "EventInterpretation.h"
#include "BaseballDatabase.h"
#include <boost/algorithm/string.hpp>
#include <ctype.h>

using namespace std;

//Create from a pointer to a play object
//baserunners_ holds runner on first, second, and third at start of play
PlayRecord::PlayRecord(const Play* play_, GameState* state_, const GameLog * const game_, const BaseballDatabase* const db_) : Play(*play_) , _state(*state_), _game(game_), _db(db_)
{
	//Add batter
	addBatter(_db->getPlayers());

	//Parse play
	_batting_result = parseBattingResult(getEventRaw());

	//Determine if it was a sacrifce
	_sacrifice = parseSacrifice(getEventRaw());

	//Update input state
	state_->setBatter(getBatter());
	state_->updateStateFromPlay(this);
}

void PlayRecord::addBatter(const vector<Player>& players_)
{
	//Loop over players_ and find match
	for (auto&& player : players_) {
		if (boost::equal(getBatterID(), player.getID())) {
			//These are equal, add the pointer
			_state.setBatter(&player);
			break;
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

bool PlayRecord::parseSacrifice(std::string play_string_)
{
	//Sacrifice information comes after /
	vector<string> line_parsed;
	boost::split(line_parsed, play_string_, boost::is_any_of("/"));

	//If line_parsed has length < 2, there is nothing to check
	if (line_parsed.size() < 2) return false;

	//Ignore the tons of information which could be here and just check if the 
	//first char is S:
	//  SF    sacrifice fly
	//	SH    sacrifice hit(bunt)
	if (line_parsed[1][0] == 'S') {
		//This is a sacrifice, return true
		return true;
	}
	else {
		return false;
	}
}

ostream & operator<<(ostream & os, const PlayRecord & p)
{
	os << " " << *(p.getBatter()) << " " << BattingResultString[p._batting_result] << " " << p.getLineRaw();
	return os;
}

//Get the number of at bats
int PlayRecord::getNumberAtBats() const
{
	//If this is a sacrifice, it's not an at bat
	if (_sacrifice) return 0;
	
	//Check if this was a valid at bat
	switch (_batting_result) {
	case ERROR:
	case GROUND_OUT: 
	case FLY_OUT:
	case STRIKE_OUT: 
	case SINGLE:
	case DOUBLE:
	case TRIPLE:
	case HR:
	case FIELDERS_CHOICE:
		//These are all valid at bats, return 1
		return 1;
	case WALK:
	case NO_PLAY:
	case CATCHER_INTERFERENCE:
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
	//If this is a sacrifice, it's not a hit
	if (_sacrifice) return 0;

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
	case FIELDERS_CHOICE:
	case CATCHER_INTERFERENCE:
		//These are no thits
		return 0;
	default:
		cout << "Play " << BattingResultString[_batting_result] << " not recognized by getNumberAtBats" << endl;
		throw exception("Error in getNumberHits");
	}
	//Should never be here
	return -1;
}

int PlayRecord::getNumberBases() const
{
	//Base this on num hits, to avoid code duplication
	if (getNumberHits() == 0) {
		//No hits means no bases
		return 0;
	}
	
	//If it makes it here it was a hit, check which one
	switch (_batting_result) {
	case SINGLE:
		return 1;
	case DOUBLE:
		return 2;
	case TRIPLE:
		return 3;
	case HR:
		return 4;
	default:
		cout << "Play " << BattingResultString[_batting_result] << " not recognized by getNumberBases" << endl;
		throw exception("Error in getNumberBases");
	}
	//Should never be here
	return -1;
}
