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

//IO includes
#include <sstream>    

//Define SQL server info (created db name etc. is hardcoded inline)
#define SQL_HOST "localhost"
#define SQL_USER "root"
#define SQL_PASS "ChrisIsCool"

//Define static labels
const std::string BaseballDatabaseSQL::DB_NAME = "BASEBALL";
const std::string BaseballDatabaseSQL::TABLE_EVENTS = "EVENTS";
const std::string BaseballDatabaseSQL::TABLE_PLAYERS = "PLAYERS";

//Define contents of tables

//This is the main table format
//To add a column, the following must be updated
//    BaseballDatabaseSQL::EVENT_COLUMNS
//    BaseballDatabaseSQL::addEvent
//    EventInfoSql class members
//    BaseballDatabaseSQL::getEventsForSQLCommand
const std::vector<sqlColumn> BaseballDatabaseSQL::EVENT_COLUMNS = {
	sqlColumn("id_event",  "INT NOT NULL AUTO_INCREMENT"),
	sqlColumn("id_player",  "CHAR(8) NOT NULL"),
	sqlColumn("result",  BattingResultString),
	sqlColumn("sacrifice",  "BOOLEAN NOT NULL"),
	sqlColumn("id_pitcher",  "CHAR(8) NOT NULL"),
	sqlColumn("id_br_1",  "CHAR(8)"),
	sqlColumn("id_br_2",  "CHAR(8)"),
	sqlColumn("id_br_3",  "CHAR(8)"),
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
	//std::cout << statement << std::endl;
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
	//std::cout << statement << std::endl;
	_stmt->execute(statement);

	//Use transactions to make things faster
	_stmt->execute("START TRANSACTION");
}

BaseballDatabaseSQL::~BaseballDatabaseSQL()
{
	//Totally done, commit whatever may be needed
	//Add this to time too
	const std::clock_t start_time_add = std::clock();
	commit();
	_add_time_total += std::clock() - start_time_add;

	//Done with object, print total time adding
	std::cout << "Total BaseballDatabaseSQL ADDING Time " << clock2sec(_add_time_total) << " s" << std::endl;
	
	//Delete statement
	delete _stmt; 
	_stmt = NULL;

	//Delete connection
	delete _con;
	_con = NULL;
}

//This should be called instead of _stmt->execute for adding 
//to database. It will track how many adds have happened and
//decide if they should be commited
void BaseballDatabaseSQL::executeAdd(const std::string command_)
{
	//Start clock
	const std::clock_t start_time_add = std::clock();

	//Actually add to the database
	try {
		_stmt->execute(command_);
	}
	catch (std::exception& e)
	{
		//Print problems
		std::cout << "ERROR adding to mySQL Datatbase: " << std::endl;
		std::cout << "  Command: " << command_ << std::endl;
		std::cout << "  " << e.what() << std::endl;
	}
	//Incremement counter
	_count_adds++;
	//Check counter
	if (_count_adds > ADDS_PER_TRANSACTION) {
		//Commit
		commit();
	}

	//Add time to total
	_add_time_total += std::clock() - start_time_add;
}

//This will send the commit command to mySQL and reset the counter
void BaseballDatabaseSQL::commit(bool start_new_transaction_)
{
	//Actually add to the database
	try {
		_stmt->execute("COMMIT");
	}
	catch (std::exception& e)
	{
		//Print problems
		std::cout << "ERROR COMMITTING to mySQL Datatbase: " << std::endl;
		std::cout << "  " << e.what() << std::endl;
		throw e;
	}
	//Reset counter
	_count_adds = 0;
	
	if (start_new_transaction_) {
		//Restart a transaction with mySQL
		_stmt->execute("START TRANSACTION");
	}
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
	command.append(" ( id_player , result , sacrifice, id_pitcher , id_br_1 , id_br_2 , id_br_3 ) VALUES (\"");
	//Batter ID
	command.append(pr_.getBatter()->getID());
	command.append("\",\"");
	//Event Result
	command.append(BattingResultString[pr_.getEventResult()]);
	command.append("\",\"");
	//Sacrifice?
	const char* sac_bool = (pr_._sacrifice) ? "1" : "0";
	command.append(sac_bool);
	command.append("\",\"");
	//Pitcher ID
	const GameState gs_start = pr_._state_start;
	const ActivePlayer* pitcher = gs_start.getFielder(PITCHER);
	command.append(pitcher->getID());
	command.append("\",\"");
	//Baserunner on first
	const ActivePlayer* br1 = gs_start.getBaserunner(1);
	if (br1 != NULL) command.append(br1->getID());
	command.append("\",\"");
	//Baserunner on second
	const ActivePlayer* br2 = gs_start.getBaserunner(2);
	if (br2 != NULL) command.append(br2->getID());
	command.append("\",\"");
	//Baserunner on third
	const ActivePlayer* br3 = gs_start.getBaserunner(3);
	if (br3 != NULL) command.append(br3->getID());

	//Finish single " and )
	command.append("\")");

	//Print command for reference
	//std::cout << command << std::endl;

	//Actually add to the database
	executeAdd(command);
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
	command.append("\")");

	//Print command for reference
	//std::cout << command << std::endl;

	//Actually add to the database
	executeAdd(command);
}

//Get all the players where a specific player is the hitter
std::vector<EventInfoSql> BaseballDatabaseSQL::getEventsForPlayer(const char* id_player_)
{
	//Create string to send to mySQL
	std::stringstream command;
	command << "SELECT * FROM EVENTS WHERE id_player = \"" << id_player_ << "\";";

	//Create object to return
	std::vector<EventInfoSql> events;

	//Call core function
	getEventsForSQLCommand(command.str().c_str(), events);

	//Return
	return events;
}

void BaseballDatabaseSQL::getEventsForSQLCommand(const char* command_, std::vector<EventInfoSql>& events_)
{
	//Get results from SQL
	sql::ResultSet *res;
	res = _stmt->executeQuery(command_);

	//Iterate over results
	while (res->next()) {
		////Print raw column values for debug
		//for (auto&& c : EVENT_COLUMNS) {
		//	//If this has the primary flag, skip
		//	if (c.isPrimary()) continue;
		//	std::cout << res->getString(c.getID()) << std::endl;
		//}

		//Add information, access via column index, assumes correct ordering
		EventInfoSql e;
		// Get event ID int directly
		e._id_event = res->getInt(1);

		// Get player Batter ID string directly
		e._id_player = res->getString(2);

		// Need to convert SQL enum to string
		std::string result_str = res->getString(3);
		//Convert SQL string to enum
		e._result = string2enum(result_str, BattingResultString);

		//Get sacrific bool from int
		e._sacrifice = res->getBoolean(4);

		// Get player Pitcher ID string directly
		e._id_pitcher = res->getString(5);

		// Get player baserunner 1 ID string directly
		e._id_br_1 = res->getString(6);

		// Get player baserunner 2 ID string directly
		e._id_br_2 = res->getString(7);

		// Get player baserunner 3 ID string directly
		e._id_br_3 = res->getString(8);

		//Add event to output
		events_.push_back(e);
	}
	//Clear allocated
	delete res; res = NULL;
}

//Create << for EventInfoSql
std::ostream& operator<<(std::ostream & os, const EventInfoSql & e)
{
	os << e._id_event << " " << e._id_player << " " << BattingResultString[e._result];
    os << " " << e._sacrifice << " " << e._id_pitcher;
	os << " " << e._id_br_1 << " " << e._id_br_2 << " " << e._id_br_3;
	return os;
}