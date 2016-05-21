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

	void printPlayerList() const;
	void printPlayList() const;

private:
	bool addPlayer(const Player* player_);
};

