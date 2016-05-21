#include "Player.h"
#include <boost/algorithm/string.hpp>

using namespace std;

Player::Player(std::string id_, std::string name_) : _id(id_)
{
	//Remove quotes which may be around name
	boost::erase_all(name_, "\"");
	_name = name_;
}

bool operator==(const Player & lhs, const Player & rhs)
{
	if (boost::equal(lhs._id, rhs._id)) {
		return true;
	}
	else {
		return false;
	}
}

std::ostream& operator<<(std::ostream & os, const Player & p)
{
	os << p._name;
	return os;
}

bool operator<(const Player & p1, const Player & p2)
{
	//Determine if ID of p1 is before p2 in alphabetical order
	const int result = p1._id.compare(p2._id);
	if (result < 0) {
		//This is means p1 is less than p2
		return true;
	} else {
		return false;
	}
}
