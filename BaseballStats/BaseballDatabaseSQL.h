#pragma once

//Includes
#include <cppconn/driver.h> //Can't get a forward declaration to work
#include <vector>
#include "sqlColumn.h";
#include <ctime>

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

	//This should be called instead of _stmt->execute
	void executeAdd(const std::string command_);
	void commit();

	//Transactions are used to increase performance
	//The "COMMIT" will happen occasionally, based on how many
	//adds are in the transaction
	unsigned long _count_adds = 0;
	//Not sure what a good value is
	static const unsigned long ADDS_PER_TRANSACTION = 10000;

	//Member to track how much time is spent
	std::clock_t _add_time_total = 0;

	//Strings used to identify mySQL components
	static const std::string DB_NAME;
	static const std::string TABLE_EVENTS;
	static const std::string TABLE_PLAYERS;

	//Details of what is in each table
	static const std::vector<sqlColumn> EVENT_COLUMNS;
	static const std::vector<sqlColumn> PLAYER_COLUMNS;
};

