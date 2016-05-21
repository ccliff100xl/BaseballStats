#pragma once
#include <string>
#include "CsvBasedObject.h"
#include "Team.h"

/////////////////
//Warning: Current implementation only keeps most recent information for team
////////////////

//Will hold the list of teams for reference
class TeamSet : public CsvBasedObject
{
private:
	std::vector<Team> _teams;
public:
	TeamSet(std::string team_list_file_);
	const Team* getTeam(std::string team_id_) const;
	void printTeamList();
};

