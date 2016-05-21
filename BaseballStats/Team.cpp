#include "Team.h"

//Overload operator
std::ostream& operator<<(std::ostream & os, const Team & t)
{
	os << t._nickname;
	return os;
}
