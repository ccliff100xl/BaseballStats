#pragma once

//Includes
#include <cppconn/driver.h> //Can't get a forward declaration to work
#include <vector>
#include "sqlColumn.h";
#include <ctime>

//Forward Declarations
class PlayRecord;
class Player;

//This struct contains the information extracted from the EVENT table
//in the database
//Order and contents should match BaseballDatabaseSQL::EVENT_COLUMNS
struct EventInfoSql
{
	long _id_event;
	std::string _id_player;
	EventResult _result;
	bool _sacrifice;

	std::string _id_pitcher;

	std::string _id_br_1;
	std::string _id_br_2;
	std::string _id_br_3;

	//Doesn't need to be friend, but this counts as a declaration
	friend std::ostream& operator<<(std::ostream & os, const EventInfoSql & e);
};

//This is a wrapper around a mySQL database
class BaseballDatabaseSQL
{
public:
	BaseballDatabaseSQL();
	~BaseballDatabaseSQL();

	//Input
	void addEvent(const PlayRecord& pr_);
	void addPlayer(const Player& player_);

	//Output
	std::vector<EventInfoSql> getEventsForPlayer(const char* id_player_);
private:
	//Connections to the database
	sql::Driver* _driver = NULL;
	sql::Connection* _con = NULL;
	sql::Statement* _stmt = NULL;

	//This should be called instead of _stmt->execute
	void executeAdd(const std::string command_);
	void commit( bool start_new_transaction_ = true);

	//Core function to get events from database
	void getEventsForSQLCommand(const char* command_, std::vector<EventInfoSql>& events_);

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
