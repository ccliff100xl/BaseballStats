#include "BaseballDatabaseSQL.h"

//Includes needed for mySQL
#include <cppconn/driver.h>
#include <cppconn/statement.h>

//Possible other includes needed (from https://dev.mysql.com/doc/connector-cpp/en/connector-cpp-tutorials-background.html)
//#include "mysql_connection.h"
//#include <cppconn/exception.h>
//#include <cppconn/resultset.h>
//#include <cppconn/prepared_statement.h>

//Includes for BaseballStats
#include "PlayRecord.h"

//Define SQL server info (created db name etc. is hardcoded inline)
#define SQL_HOST "localhost"
#define SQL_USER "root"
#define SQL_PASS "ChrisIsCool"

//Define static labels
const std::string BaseballDatabaseSQL::DB_NAME = "BASEBALL";
const std::string BaseballDatabaseSQL::TABLE_EVENTS = "EVENTS";
const std::string BaseballDatabaseSQL::TABLE_PLAYERS = "PLAYERS";

//Define contents of tables
const std::vector<sqlColumn> BaseballDatabaseSQL::EVENT_COLUMNS = {
	sqlColumn("id_event",  "INT NOT NULL AUTO_INCREMENT"),
	sqlColumn("id_player",  "CHAR(8) NOT NULL"),
	sqlColumn("result",  BattingResultString),
	sqlColumn("PRIMARY KEY",  "(id_event)", true) //Add primary key like a column (must be last)
};

const std::vector<sqlColumn> BaseballDatabaseSQL::PLAYER_COLUMNS = {
	sqlColumn("id_player",  "CHAR(8) NOT NULL"),
	sqlColumn("full_name",  "CHAR(26) NOT NULL"), //26 is the longest name 
	sqlColumn("PRIMARY KEY",  "(id_player)", true) //Add primary key like a column (must be last)
};

BaseballDatabaseSQL::BaseballDatabaseSQL()
{
	//Connect to mySQL server 
	_driver = get_driver_instance();
	_con = _driver->connect(SQL_HOST, SQL_USER, SQL_PASS);

	//stmt can be used to use mySQL language
	_stmt = _con->createStatement();

	//Drop database if it's there
	std::string statement;
	statement.append("DROP DATABASE IF EXISTS ");
	statement.append(DB_NAME);
	_stmt->execute(statement);

	//Create the new database
	statement.clear();
	statement.append("CREATE DATABASE ");
	statement.append(DB_NAME);
	_stmt->execute(statement);

	//Set to current DB
	_con->setSchema(DB_NAME);

	//Add a table to hold all players
	statement.clear();
	statement.append("CREATE TABLE ");
	statement.append(TABLE_PLAYERS);
	statement.append("(");
	for (auto&& col : PLAYER_COLUMNS) {
		statement.append(col.getID());
		statement.append(" ");
		statement.append(col.getDetails());
		//Don't add comma if primary
		if (!col.isPrimary()) {
			statement.append(", ");
		}
	}
	statement.append(")");
	std::cout << statement << std::endl;
	_stmt->execute(statement);
	 
	//Add a table to hold all events
	statement.clear();
	statement.append("CREATE TABLE ");
	statement.append(TABLE_EVENTS);
	statement.append("(");
	for (auto&& col : EVENT_COLUMNS) {
		statement.append(col.getID());
		statement.append(" ");
		statement.append(col.getDetails());
		//Don't add comma if primary
		if (!col.isPrimary()) {
			statement.append(", ");
		}
	}
	statement.append(")");
	std::cout << statement << std::endl;
	_stmt->execute(statement);
}


BaseballDatabaseSQL::~BaseballDatabaseSQL()
{
	//Delete statement
	delete _stmt; 
	_stmt = NULL;

	//Delete connection
	delete _con;
	_con = NULL;
}

//This is the key function which converts whatever is in the PlayRecord
//So mySQL syntax.
void BaseballDatabaseSQL::addEvent(const PlayRecord& pr_)
{
	//Setup command for adding information to database
	std::string command;
	//Need to keep column ids consistent with EVENT_COLUMNS
	command.append("INSERT INTO ");
	command.append(TABLE_EVENTS);
	command.append(" ( id_player , result ) VALUES (\"");
	command.append(pr_.getBatter()->getID());
	command.append("\",\"");
	command.append(BattingResultString[pr_.getEventResult()]);
	command.append("\")");

	//Print command for reference
	//std::cout << command << std::endl;

	//Actually add to the database
	try {
		_stmt->execute(command);
	}
	catch (std::exception& e)
	{
		std::cout << "ERROR adding EVENT to mySQL Datatbase: ";
		std::cout << e.what() << std::endl;
	}
}

//This adds only the player ID name, nothing else is known in Player
void BaseballDatabaseSQL::addPlayer(const Player& player_)
{
	//Setup command for adding information to database
	std::string command;
	//Need to keep column ids consistent with PLAYER_COLUMNS
	//Using IGNORE so that no errors are thrown if player is duplicated
	command.append("INSERT IGNORE INTO ");
	command.append(TABLE_PLAYERS);
	command.append(" ( id_player , full_name ) VALUES (\"");
	command.append(player_.getID());
	command.append("\" , \"");
	command.append(player_.getName());
	//Do NOT add if already there TODO: add "WHERE..."
	command.append("\")");

	//Print command for reference
	//std::cout << command << std::endl;

	//Actually add to the database
	try {
		_stmt->execute(command);
	}
	catch (std::exception& e)
	{
		std::cout << "ERROR adding PLAYER to mySQL Datatbase: ";
		std::cout << e.what() << std::endl;
	}
}