#pragma once
#include <string>
#include <vector>

#include "InfoItem.h"
#include "Play.h"
#include "ActivePlayer.h"

//This stores the unparsed information from a 
class GameLog
{
	//Game id
	const std::string _game_id;

	//TODO: Parse _game_id to get date. Apparent boost has a date class

	//version
	const int _version;

	//Vector of objects for "info" lines
	std::vector<InfoItem> _game_info;

	//Vector of starters
	std::vector<ActivePlayer> _starters;

	//Vector of object for play lines and accessories
	std::vector<Play> _plays;

	//Vector of objects for "data" lines
	//These are always earned run, so just need name and runs
	std::vector<InfoItem> _earned_runs;

public:
	/////////////
	// Methods to create object
	
	//Create game with basic information
	GameLog(std::string game_id_, int version_) : _game_id(game_id_), _version(version_) {};

	//Parse a single line from a play file
	bool parseLogLine(const std::string line_);

	/////////////
	// Methods to get information from object

	//Get game ID
	const std::string getID() const;

	//Get team IDs
	const std::string getHomeTeamID() const;
	const std::string getVisitorTeamID() const;

	//Get vectors to iterate over
	const  std::vector<ActivePlayer>& getStarters() const { return _starters; }
	const  std::vector<Play>& getPlays() const { return _plays; }
private:
	std::string getInfoItemInformation(std::string label_) const;
};
