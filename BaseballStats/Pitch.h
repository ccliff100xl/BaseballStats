#pragma once
#include <string>
#include <vector>

class Pitch
{
public:
	enum Type {
		B = 'B', //ball
		C = 'C', //called strike
		F = 'F', //foul
		H = 'H', //hit batter
		I = 'I', //intentional ball
		K = 'K', //strike(unknown type)
		L = 'L', //foul bunt
		M = 'M', //missed bunt attempt
		N = 'N', //no pitch(on balks and interference calls)
		O = 'O', //foul tip on bunt
		P = 'P', //pitchout
		Q = 'Q', //swinging on pitchout
		R = 'R', //foul ball on pitchout
		S = 'S', //swinging strike
		T = 'T', //foul tip
		U = 'U', //unknown or missed pitch
		V = 'V', //called ball because pitcher went to his mouth
		X = 'X', //ball put into play by batter
		Y = 'Y', //ball put into play on pitchout
		UNSET //default value before it's set
	};
private:
	Type _type = UNSET;
	bool _pickoff_by_catcher = false;
	//Array of bases thrown to as pickoff attempts, if negative, throw as by catcher
	std::vector<int> _pickoff_bases;
	bool _block = false;
	bool _runner_going = false;
	bool _batter_not_involved = false;
public:
	Pitch(char type_, std::string info_);
};

