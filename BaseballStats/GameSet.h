#pragma once
#include "stdafx.h"
#include "GameLog.h"
#include "TeamSet.h"

class GameSet
{
	const TeamSet* _team_set;
	std::vector<GameLog> _logs;
public:
	GameSet(const StringVector logs_file_paths_, TeamSet* team_set_);
	const std::vector<GameLog>& getGamesLogs() const { return _logs; }
};

