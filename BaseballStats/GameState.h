#pragma once
#include "Player.h"

class PlayRecord;

//This class will hold the state of the game: score, inning, batter, baserunners...
class GameState
{
	//Define players involved
	const Player* _batter = NULL;
	const Player* _baserunner_1 = NULL; //Runner of first
	const Player* _baserunner_2 = NULL; //Runner of second
	const Player* _baserunner_3 = NULL; //Runner of third
public:
	//Method which does most of the work
	void updateStateFromPlay(const PlayRecord* play_);

	//Accessors
	const Player* getBatter() const { return _batter; }

	//Setters
	void setBatter(const Player* batter_) { _batter = batter_; }
};

