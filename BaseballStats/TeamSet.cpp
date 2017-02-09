#include "TeamSet.h"
#include <iostream>
#include <string>

using namespace std;

//Constructor to create object based on 
TeamSet::TeamSet(std::string team_list_file_) : CsvBasedObject(team_list_file_)
{
	//Parse lines from _csv_lines
	for (auto line : getCsvLines()) {
		//Get Team ID
		string id_team = line[0];

		//Has this team been seen before?
		Team* team_to_update = NULL;
		//&& means loop through references so vector can be changed
		for (auto&& team : _teams) {
			if (id_team == team.getId()) {
				//This team has been seen before, replace it
				team_to_update = &team;
				break;
			}
		}

		//Check if a team was found to replace
		if (team_to_update == NULL) {
			//Team not found, add it
			_teams.push_back(Team(id_team));
			team_to_update = &(_teams.back());
		}

		//Add information for this team
		team_to_update->setLeague(line[2]);
		team_to_update->setDivision(line[3]);
		team_to_update->setLocationName(line[4]);
		team_to_update->setNickname(line[5]);
		team_to_update->setNicknameAlternate(line[6]);
		team_to_update->setCity(line[9]);
		team_to_update->setState(line[10]);
	}

	//Done loading teams, clear object
	clearCsvData();

	cout << "Number of teams loaded: " << _teams.size() << endl;
}
const Team* TeamSet::getTeam(std::string team_id_) const
{
	//Search for ID
	for (auto&& team : _teams) {
		if (team_id_ == team.getId()) {
			//Found match, return
			return &team;
		}
	}
	return NULL;
}
void TeamSet::printTeamList()
{
	for (auto team : _teams) {
		cout << team << endl;
	}
}
////TEAM FILE CONTENTS
//0 Current franchise ID
//1 Franchise ID(may the same as current one) for earlier data
//2 League
//3 Division(blank before divisional play)
//4 Location name(e.g.Brooklyn, Texas)
//5 Nickname
//6 Alternate nicknames(blank if none for the period)
//7 Date of the first game with this combination
//8 Date of the last game with combination(blank if current one)
//9 City
//10 State
//The league codes are :
//
//NA = National Association
//NL = National League
//AA = American Association
//AL = American League