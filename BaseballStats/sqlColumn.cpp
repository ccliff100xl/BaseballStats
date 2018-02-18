#include "sqlColumn.h"
#include "SportsStatsConstants.h"

//Constructor based on direct input of details
sqlColumn::sqlColumn(const char* id_, const char* details_, bool is_primary_) : 
	_id(id_) , _details(details_) , _is_primary(is_primary_)
{

}

//Constructor which creates details of type ENUM based on std::map
sqlColumn::sqlColumn(const char* id_, std::map< EventResult, const char * >& map_, bool is_primary_) :
	_id(id_), _is_primary(is_primary_)
{
	//Create details
	_details.append("ENUM(");
	for (auto&& p : map_)
	{
		_details.append("\"");
		_details.append(p.second);
		_details.append("\",");
	}
	//Remove final ','
	_details.pop_back();
	_details.append(")");
}