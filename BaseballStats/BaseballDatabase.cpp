#include "BaseballDatabase.h"
#include "GameSet.h"
#include <algorithm>
#include <iomanip>
#include <iostream>

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
		// This temporary object will be used to track the state of the game
		GameState state(&game);
		for (auto&& play : game.getPlays()) {
			try {
				_plays.push_back(PlayRecord(&play, &state, &game, this));
			}
			catch (std::exception& e) {
				//Print recent plays
				printPlayList(20);
				//Print some information, then throw the same exception
				std::cout << "Error on Play: " << play.getLineRaw() << std::endl;
				throw(e);
			}
		}
		//Print final game informtation from state
		state.printScore(std::cout);
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

//Will print all plays if n_plays_ < 0 (Default)
//Otherwise, it will print the n_plays_ most recent plays
void BaseballDatabase::printPlayList(int n_plays_) const
{
	//Get total number of plays
	const int n_plays_total = (int)_plays.size();

   //Update n_plays if needed
	if (n_plays_ < 0) {
		n_plays_ = n_plays_total;
	}

	//Print plays
	for (int ip = n_plays_total - n_plays_; ip < n_plays_total; ip++) {
		cout << _plays[ip] << endl;
	}
}

void BaseballDatabase::printAllBattingAverages() const
{
	for (auto&& p : _players) {
		int n_atbats = -1;
		int n_hits = -1;
		const std::string playerid = p.getID();
		double average = getPlayerBattingAverage(playerid, n_atbats, n_hits);
		const Player player = getPlayer(playerid);
		cout << player << " " << n_hits << "/" << n_atbats << " " << std::setprecision(3) << std::fixed << average << endl;
	}
}

void BaseballDatabase::printAllSluggingPercentages() const
{
	for (auto&& p : _players) {
		const std::string playerid = p.getID();
		double slugging = getPlayerSluggingPercentage(playerid);

		//Don't print if it's 0
		if (slugging < FLT_EPSILON) continue;

		const Player player = getPlayer(playerid);
		cout << player << " " << std::setprecision(3) << std::fixed << slugging << endl;
	}
}

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

	//If there are no at bats, just return 0
	if (n_hits_ == 0) return 0.0;

	//Calculate average
	return static_cast<double>( n_hits_ ) / n_atbats_;
}

double BaseballDatabase::getPlayerBattingAverage(const std::string player_id_) const
{
	//Make counts
	int n_atbats_ = 0;
	int n_hits_ = 0;

	//Call function
	return getPlayerBattingAverage(player_id_, n_atbats_, n_hits_);
}

double BaseballDatabase::getPlayerSluggingPercentage(const std::string player_id_) const
{
	//Setup counts
	int  n_atbats = 0;
    int  n_bases = 0;

	//Loop over plays
	for (auto&& p : _plays) {
		//Check hitter
		if (p.getBatterID() == player_id_) {
			//Print play for debug
			//std::cout << p << std::endl;

			//Add this play
			n_atbats += p.getNumberAtBats();
			n_bases += p.getNumberBases();
		}
	}

	//If there are no bases, just return 0
	if (n_bases == 0) return 0.0;

	//Calculate average
	return static_cast<double>(n_bases) / n_atbats;
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
