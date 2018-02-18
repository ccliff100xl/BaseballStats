#pragma once
#include <string>
#include "SportsStatsConstants.h"

//This class will hold the information related to a column in SQL
class sqlColumn
{
	std::string _id;
	std::string _details; //datatype and other info (like NOT NULL)
	//This could be setup so that any column could have this true, then the code would combine them automatically
	//For now, this is just a flag to say this isn't a real column
	bool _is_primary; // true if primary key

public:
	sqlColumn(const char* id_, const char* details_, bool is_primary_ = false);
	sqlColumn(const char* id_, std::map< EventResult , const char * >& map_, bool is_primary_ = false);

	const char* getID() const { return _id.c_str(); }
	const char* getDetails() const { return _details.c_str(); }
	bool isPrimary() const { return _is_primary; }
};

