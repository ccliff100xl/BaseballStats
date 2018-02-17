#pragma once
#include <vector>
#include "TeamSet.h"
#include "Player.h"
#include "PlayRecord.h"

class GameSet;
class BaseballDatabaseSQL;

class BaseballDatabase
{
	const TeamSet _teams;

	//When I created this, I had the idea that all pointers to players in the whole program would 
	//point to _players here.  But, local ActivePlayers have taken over to handle substitutions.
	std::vector<Player> _players;
	bool _players_locked = false; //If this is true, cannot call addPlayer
	std::vector<PlayRecord> _plays;
public:
	BaseballDatabase(const TeamSet* ts_, const GameSet* gs_, BaseballDatabaseSQL* dbsql_);

	const std::vector<Player>& getPlayers() const { return _players; }
	const Player getPlayer( const std::string player_id_ ) const;

	void printPlayerList() const;
	void printPlayList(int n_plays = -1) const;
	void printAllBattingAverages() const;
	void printAllSluggingPercentages() const;

	double getPlayerBattingAverage(const std::string player_id_, int& n_atbats_, int& n_hits_) const;
	double getPlayerBattingAverage(const std::string player_id_) const;
	double getPlayerSluggingPercentage(const std::string player_id_) const;

private:
	bool addPlayer(const Player* player_);
};

