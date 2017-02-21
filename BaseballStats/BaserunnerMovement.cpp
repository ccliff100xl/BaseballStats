#include "BaserunnerMovement.h"
#include <iostream>

BaserunnerMovement::BaserunnerMovement(int base_start_, int base_end_, bool made_out_) :
	_base_start(base_start_),
	_base_end(base_end_),
	_made_out(made_out_)
{
	//Make sure bases are valid
	if (_base_start != 0 && _base_start != 1 && _base_start != 2 && _base_start != 3) {
		throw std::exception("BaserunnerMovement::BaserunnerMovement: Invalid base start");
	}
	if (base_end_ != 1 && base_end_ != 2 && base_end_ != 3 && base_end_ != 4) {
		throw std::exception("BaserunnerMovement::BaserunnerMovement: Invalid base end");
	}

	//Determine if run was scored
	if (_base_end == 4 && _made_out == false) {
		_run_scored = true;
	}
}