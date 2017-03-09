#pragma once
#include <string>
#include "ActivePlayer.h"
#include "SportsStatsConstants.h"

class Play
{
	/////////////////////
	//Raw line from file
	const std::string _line_raw;

	/////////////////////
	//Details of play line
	int _inning;
	TeamType _team;
	std::string _player_id;
	int _count[2] = {-1,-1};
	std::string _pitch_details;
	std::string _event_raw;

	/////////////////////
	//Vector of comments after play
	std::vector<std::string> _comments;

	/////////////////////
	//Vector of subs after play
	std::vector<ActivePlayer> _subs;

public:
	/////////////
	// Methods to create object

	Play(std::string line_raw_);

	void addComment(std::string comment_line)
	{
		_comments.push_back(comment_line);
	}

	void addSub(ActivePlayer& sub_line)
	{
		_subs.push_back(sub_line);
	}
	/////////////
	// Methods to get information from object
	int getInning() const { return _inning; }
	TeamType getTeam() const { return _team; }
	std::string getBatterID() const { return _player_id; }
	std::string getPitchesString() const { return _pitch_details; }
	std::string getEventRaw() const { return _event_raw; }
	std::string getLineRaw() const { return _line_raw; }

	const std::vector<ActivePlayer>& getSubs() const { return _subs; }
};

