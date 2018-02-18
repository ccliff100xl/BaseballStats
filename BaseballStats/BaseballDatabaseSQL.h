#pragma once

//Includes
#include <cppconn/driver.h> //Can't get a forward declaration to work
#include <vector>
#include "sqlColumn.h";

//Forward Declarations
class PlayRecord;
class Player;

//This is a wrapper around a mySQL database
class BaseballDatabaseSQL
{
public:
	BaseballDatabaseSQL();
	~BaseballDatabaseSQL();

	void addEvent(const PlayRecord& pr_);
	void addPlayer(const Player& player_);
private:
	//Connections to the database
	sql::Driver* _driver = NULL;
	sql::Connection* _con = NULL;
	sql::Statement* _stmt = NULL;

	//Strings used to identify mySQL components
	static const std::string DB_NAME;
	static const std::string TABLE_EVENTS;
	static const std::string TABLE_PLAYERS;

	//Details of what is in each table
	static const std::vector<sqlColumn> EVENT_COLUMNS;
	static const std::vector<sqlColumn> PLAYER_COLUMNS;
};

