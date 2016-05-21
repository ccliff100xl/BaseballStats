#pragma once
#include <string>
#include <iostream>

class Team
{
private:
	const std::string _id;
	std::string _league;
	std::string _division;
	std::string _location_name;
	std::string _nickname;
	std::string _nickname_alternate;
	std::string _city;
	std::string _state;
public:
	//Constructor takes ID only
	Team(std::string id_) : _id(id_) {}

	//Accessors
	std::string getId() const { return _id; }
	std::string getNickname() const { return _nickname; }

	//Setters
	void setLeague(std::string league_) { _league = league_;  }
	void setDivision(std::string division_) { _division = division_; }
	void setLocationName(std::string location_name_) { _location_name = location_name_; }
	void setNickname(std::string nickname_) { _nickname = nickname_; }
	void setNicknameAlternate(std::string nickname_alternate_) { _nickname_alternate = nickname_alternate_; }
	void setCity(std::string city_) { _city = city_; }
	void setState(std::string state_) { _state = state_; }

	//Make operator definition a friend
	friend std::ostream& operator<<(std::ostream & os, const Team & t);
};


