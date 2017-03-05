#include "BaserunnerMovement.h"
#include <iostream>

BaserunnerMovement::BaserunnerMovement(int base_start_, int base_end_, bool made_out_, bool is_error_) :
	_base_start(base_start_),
	_base_end(base_end_),
	_made_out(made_out_),
    _is_error(is_error_)
{
	//Make sure bases are valid
	//Cannot go backwards
	if (_base_start > _base_end) {
		throw std::exception("BaserunnerMovement::BaserunnerMovement: base start > base end");
	}
	//Cannot start as 4 (which means scored at home)
	if (_base_start != 0 && _base_start != 1 && _base_start != 2 && _base_start != 3) {
		throw std::exception("BaserunnerMovement::BaserunnerMovement: Invalid base start");
	}
	//Cannot end at 0, unless there is an out
	if (_base_end != 1 && _base_end != 2 && _base_end != 3 && _base_end != 4) {
		//Is _base_end = 0 and out, this is OK
		if (!(_base_end == 0 && _made_out)) {
			throw std::exception("BaserunnerMovement::BaserunnerMovement: Invalid base end");
		}
	}

	//Determine if run was scored
	if (_base_end == 4 && _made_out == false) {
		_run_scored = true;
	}
}

bool baserunnerMovementCompare(const BaserunnerMovement& m1_, const BaserunnerMovement& m2_)
{
	return m1_.getStartingBase() > m2_.getStartingBase();
}