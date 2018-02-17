#pragma once

//Includes
#include <cppconn/driver.h> //Can't get a forward declaration to work

//Forward Declarations
class PlayRecord;

//This is a wrapper around a mySQL database
class BaseballDatabaseSQL
{
public:
	BaseballDatabaseSQL();
	~BaseballDatabaseSQL();

	void addPlay(const PlayRecord& pr_);
private:
	sql::Driver* _driver;
	sql::Connection* _con;
	sql::Statement* _stmt;
};

