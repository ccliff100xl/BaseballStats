#include "BaseballDatabase.h"
#include <algorithm>

using namespace std;

BaseballDatabase::BaseballDatabase(const TeamSet* ts_, const GameSet* gs_) : _teams(*ts_)
{
	//////////////////
	/// Populate Players
	//////////////////

	//Loop over games
	for (auto&& game : gs_->getGamesLogs()) {
		//Loop over all starting players for this game and add
		for (auto&& player : game.getStarters()) {
			addPlayer(&player);
		}

		//Loop over all plays and add any subs
		for (auto&& play : game.getPlays()) {
			//Loop over subs (may be none)
			for (auto&& player : play.getSubs()) {
				addPlayer(&player);
			}
		}
	}
	//Done populating players, sort the list
	sort(_players.begin(), _players.end());
	//Lock vector
	_players_locked = true;

	//////////////////
	/// Populate Plays
	//////////////////

	//Loop over games
	//Keep local vector to make sure a game is not counted twice
	vector<string> game_ids_seem;
	for (auto&& game : gs_->getGamesLogs()) {
		//Make sure this game has not been seen already
		vector<string>::const_iterator g = find(game_ids_seem.begin(), game_ids_seem.end(), game.getID());
		if (g != game_ids_seem.end()) {
			//This means it has been seen before, skip it
			cout << game.getID() << " seen multiple times, skipping" << endl;
			continue;
		}

		// Loop over all plays and copy
		for (auto&& play : game.getPlays()) {
			_plays.push_back(PlayRecord(&play, &game, this));
		}
	}
}

//Return true if player was added to database, false if it alread existed
//Needs to be a pointer so other kinds can be passed
bool BaseballDatabase::addPlayer(const Player* player_)
{
	//Check if this is locked
	if (_players_locked) {
		//It's locked, this is an error
		throw exception("Attempted to add to locked players vector");
	}
	
	//Find player
	vector<Player>::const_iterator p = find(_players.begin(), _players.end(), *player_);
	
	//Check if found based on location of p
	if (p != _players.end()) {
		//Do nothing, return false to say not added
		return false;
	} else {
		//Add the player
		_players.push_back(*player_);
		return true;
	}
}

void BaseballDatabase::printPlayerList() const
{
	for (auto&& p : _players) {
		cout << p << " " << p.getID() << endl;
	}
}

void BaseballDatabase::printPlayList() const
{
	for (auto&& p : _plays) {
		cout << p << endl;
	}
}

//TODO: This is not matching the web, probably need to print each at bat and compare
double BaseballDatabase::getPlayerBattingAverage(const std::string player_id_, int& n_atbats_, int& n_hits_) const
{
	//Reset counts
	n_atbats_ = 0;
	n_hits_ = 0;

	//Loop over plays
	for (auto&& p : _plays) {
		//Check hitter
		if (p.getBatterID() == player_id_) {
			//Print play for debug
			//std::cout << p << std::endl;

			//Add this play
			n_atbats_ += p.getNumberAtBats();
			n_hits_ += p.getNumberHits();
		}
	}

	//Calculate average
	return static_cast<double>( n_hits_ ) / n_atbats_;
}

const Player BaseballDatabase::getPlayer(const std::string player_id_) const
{
	//Find matching player
	for (auto&& p : _players) {
		if (p.getID() == player_id_) {
			return p;
		}
	}

	//If it makes it here, its an error
	throw exception("BaseballDatabase::getPlayer Error: Could not final player_id_");
}