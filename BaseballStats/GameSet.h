#pragma once
#include "GameLog.h"
#include "TeamSet.h"

class GameSet
{
	const TeamSet* _team_set;
	std::vector<GameLog> _logs;
public:
	GameSet(std::string logs_file_path_, TeamSet* team_set_);
	const std::vector<GameLog>& getGamesLogs() const { return _logs; }
	void printGameResults();
};

