#include "Pitch.h"
#include <iostream>

Pitch::Pitch(char type_, std::string info_)
{
	//Set type (enum setup so value == char)
	_type = static_cast<Pitch::Type>(type_);
	
	//Make sure this is a valid type (put most common first)
	if (_type != B &&
		_type != S &&
		_type != C &&
		_type != X &&
		_type != F &&
		_type != K &&
		_type != H &&
		_type != I &&
		_type != L &&
		_type != M &&
		_type != N &&
		_type != O &&
		_type != P &&
		_type != Q &&
		_type != R &&
		_type != T &&
		_type != U &&
		_type != V &&
		_type != Y) {
		//This is not valid, for now just accept it
		_type = Pitch::UNKNOWN;
		//Print warning
		std::cout << "Warning, unknown pitch type: "<< type_ << std::endl;
		//throw std::exception("Unrecognized pitch type character");
	}

	//Set members from info
	//+following pickoff throw by the catcher
	//	*  indicates the following pitch was blocked by the catcher
	//	.marker for play not involving the batter
	//	1  pickoff throw to first
	//	2  pickoff throw to second
	//	3  pickoff throw to third
	//>  Indicates a runner going on the pitch
	for (auto&& c : info_) {
		switch (c) {
		case '+':
			_pickoff_by_catcher = true;
			break;
		case '*':
			_block = true;
			break;
		case '.':
			_batter_not_involved = true;
			break;
		case '1':
		case '2':
		case '3':
			_pickoff_bases.push_back(c - '0');
			break;
		case '>':
			_runner_going = true;
			break;
		default:
			std::cout << c << std::endl;
			throw std::exception("Unrecognized info_ character");
		}
	}
}
