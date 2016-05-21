#pragma once
#include "Player.h"
class PlayerRecord : public Player
{
	int _at_bats = 0;
	int _hits = 0;
public:
	void addAtBat();
};

