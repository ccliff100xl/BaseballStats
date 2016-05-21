#pragma once
#include <string>

class Player
{
	std::string _id;
	std::string _name;
public:
	//Needs defaults for sorting function
	Player(std::string id_ = "aaaaa000", std::string name_ = "Unknown Name");

	//Define operators as friend
	friend bool operator==(const Player& lhs, const Player& rhs);//Make operator definition a friend
	friend std::ostream& operator<<(std::ostream & os, const Player & p);
	friend bool operator< (const Player& p1, const Player& p2);

	//Define accessors
	std::string getID() const { return _id; }
};

