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

BaseballDatabaseSQL::BaseballDatabaseSQL()
{
	//Connect to mySQL server 
	_driver = get_driver_instance();
	_con = _driver->connect(SQL_HOST, SQL_USER, SQL_PASS);

	//stmt can be used to use mySQL language
	_stmt = _con->createStatement();

	//Create a new database (delete first)
	_stmt->execute("DROP DATABASE IF EXISTS BASEBALL");
	_stmt->execute("CREATE DATABASE BASEBALL");
	_con->setSchema("BASEBALL");

	//Add a table to hold all events
	//Currently just 1.) id and 2.) player id
	_stmt->execute("CREATE TABLE EVENTS(id_event INT NOT NULL AUTO_INCREMENT,id_player CHAR(8) NOT NULL, PRIMARY KEY ( id_event ))");
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

void BaseballDatabaseSQL::addPlay(const PlayRecord& pr_)
{
	//Setup command for adding information to database
	std::string command;
	command.append("INSERT INTO EVENTS ( id_player ) VALUES (\"");
	command.append(pr_.getBatter()->getID());
	command.append("\")");

	//Print command for reference
	//std::cout << command << std::endl;

	//Actually add to the database
	try {
		_stmt->execute(command);
	}
	catch (std::exception& e)
	{
		std::cout << "ERROR adding to mySQL Datatbase: ";
		std::cout << e.what() << std::endl;
	}
}
