#pragma once
#include <vector>
#include "TeamSet.h"
#include "GameSet.h"
#include "Player.h"
#include "PlayRecord.h"
class BaseballDatabase
{
	const TeamSet _teams;
	std::vector<Player> _players;
	bool _players_locked = false; //If this is true, cannot call addPlayer
	std::vector<PlayRecord> _plays;
public:
	BaseballDatabase(const TeamSet* ts_, const GameSet* gs_);

	const std::vector<Player>& getPlayers() const { return _players; }
	const Player getPlayer( const std::string player_id_ ) const;

	void printPlayerList() const;
	void printPlayList() const;
	void printAllBattingAverages() const;
	void printAllSluggingPercentages() const;

	double getPlayerBattingAverage(const std::string player_id_, int& n_atbats_, int& n_hits_) const;
	double getPlayerBattingAverage(const std::string player_id_) const;
	double getPlayerSluggingPercentage(const std::string player_id_) const;

private:
	bool addPlayer(const Player* player_);
};

