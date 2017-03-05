#pragma once

//Simple class to track a single baserunner movement
// Definition of bases
// 0: Starting as hitter
// 1: First Base
// 2: Second Base
// 3: Third Base
// 4: Home Plate

class BaserunnerMovement
{
    int _base_start;
    int _base_end;
    bool _made_out; //This is true if there is an X, may not be an out if there is an error
    bool _run_scored = false;
	bool _is_error = false;
public:
	//Construct with everything but _run_scored, that is calculated
	BaserunnerMovement(int base_start_, 
		int base_end_, 
		bool made_out_ = false, 
		bool is_error_ = false);

	//Accessor
	int getStartingBase() const { return _base_start; }
	int getEndingBase() const { return _base_end; }
	//Out cannot be made if there is an error
	bool wasOutMade() const { return _made_out && !_is_error; }
	bool wasRunScored() const { return _run_scored;  }
	bool wasError() const { return _is_error; }
};

//This function will compare baserunner movements for vector sorting
bool baserunnerMovementCompare(const BaserunnerMovement& m1_, const BaserunnerMovement& m2_);