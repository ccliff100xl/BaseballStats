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
	const int _base_start;
	const int _base_end;
	const bool _made_out;
    bool _run_scored = false;
public:
	//Construct with everything but _run_scored, that is calculated
	BaserunnerMovement(int base_start_, int base_end_, bool made_out_ = false);

	//Accessor
	int getStartingBase() const { return _base_start; }
	int getEndingBase() const { return _base_end; }
	bool wasOutMade() const { return _made_out; }
	bool wasRunScored() const { return _run_scored;  }

};

