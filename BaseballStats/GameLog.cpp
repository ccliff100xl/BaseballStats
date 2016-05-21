#include "GameLog.h"
#include "ActivePlayer.h"
#include "Play.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

//This will parse any line from a game log
//Return true if the line was read successfully, false if not
// false means the line is the beginning of a new game so this game is done
bool GameLog::parseLogLine(const string line_)
{
	//Parse line
	vector<string> line_parsed;
	boost::split(line_parsed, line_, boost::is_any_of(","));
	//This must be id,<gameid_string>
	//std::cout << "GameLog::parseLogLine " << line_ << std::endl;
	if (boost::equal(line_parsed[0], string("id"))) {
		//This means a new game has been found, this game is over, 
		//cout << "Game Summary for " << _game_id << endl;
		//cout << "  Number of Plays: " << _plays.size() << endl;
		return false;
	}

	////Parse log line depending on type
	const string line_id = line_parsed[0];

	//If "info", add to array
	if (boost::equal(line_id, string("info"))) {
		_game_info.push_back(InfoItem(line_parsed[1], line_parsed[2]));
	}
	//If "data", add to array
	else if (boost::equal(line_id, string("data"))) {
		_earned_runs.push_back(InfoItem(line_parsed[2], line_parsed[3]));
	}
	//If "start" add to array
	else if (boost::equal(line_id, string("start"))) {
		_starters.push_back(ActivePlayer(line_parsed));
	}
	//If "play" add to array
	else if (boost::equal(line_id, string("play"))) {
		_plays.push_back(Play(line_));
	}
	//If "com" add to previous play
	else if (boost::equal(line_id, string("com"))) {
		//Check if there is a play to add to
		if (_plays.size() == 0) {
			cout << "Comment before play: " << line_ << endl;
		} 
		else {
			_plays.back().addComment(line_);
		}
	}
	//If "sub" add to previous play
	else if (boost::equal(line_id, string("sub"))) {
		//Check if there is a play to add to
		if (_plays.size() == 0) {
			//This breaks everything, error out
			cout << "Sub before play: " << line_ << endl;
			exit(1);
		}
		else {
			_plays.back().addSub(ActivePlayer(line_parsed));
		}
	}
	else {
		//This is an error
		cout << "Line ID: " << line_id << " not recognized" << endl;
		throw exception("Unrecognized game log line");
	}

	return true;
}

//This function will return a vector<int> of length
//two.  The first entry will be the visitors score,
//the second will be the home teams score
std::vector<int> GameLog::getScore() const
{
	//Create vector to hold score
	vector<int> score = { 0, 0 };

	//Loop over plays and add runs
	for (auto&& play : _plays) {
		score[play.getTeam()] += play.getRunsScored();
	}

	return score;
}

const std::string GameLog::getID() const
{
	return getInfoItemInformation("id");
}

const std::string GameLog::getHomeTeamID() const
{
	return getInfoItemInformation("hometeam");
}

const std::string GameLog::getVisitorTeamID() const
{
	return getInfoItemInformation("visteam");
}

std::string GameLog::getInfoItemInformation(std::string label_) const
{
	for (auto&& info : _game_info) {
		if (boost::equal(info.getLabel(), label_)) {
			return info.getInformation();
		}
	}
	return "000";
}
