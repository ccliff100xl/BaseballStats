#pragma once
#include <string>
#include <vector>
#include "stdafx.h"
#include "SportsStatsConstants.h"
#include "Player.h"
class ActivePlayer : public Player
{
	const TeamType _team;
	const int _batting_position;
	const int _field_position; //Standard Pitcher 1, Catcher 2...
public:
	//Constructor based on values
	ActivePlayer(
		std::string id_,
		std::string name_,
		int team_, 
		int batting_position_, //1-9, pitcher in DH game is 0
		int field_position_) : Player(id_, name_), 
		_team((TeamType) team_) ,
		_batting_position(batting_position_) ,
        _field_position(field_position_) {}
	
	//Constructor based on starter or sub line parsed
	ActivePlayer(std::vector<std::string> line_parsed) : Player(line_parsed[1], line_parsed[2]),
		_team((TeamType) string2int(line_parsed[3])),
		_batting_position(string2int(line_parsed[4])),
		_field_position(string2int(line_parsed[5])) { }

	//Accessors
	TeamType getTeam() const { return _team; }
	int getBattingPosition() const { return _batting_position; };
	//Convert _field_position to DefensivePosition enum
	DefensivePosition getDefensivePosition() const { return static_cast<DefensivePosition>(_field_position - 1); }
};

