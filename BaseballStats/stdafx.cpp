// stdafx.cpp : source file that includes just the standard includes
// BaseballStats.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

//This is a wrapper around boost:split which just returns the StringVector
//by value and removes any empty cells, which basically treats multiple delimiters
//as one and ignores delimiters as the last char
StringVector SplitStringToVector(const std::string str_, const char* delims_)
{
	//Create object to parse into
	StringVector string_parsed_raw;
	//Create object to return
	StringVector string_parsed;

	//Parse to object
	boost::split(string_parsed_raw, str_, boost::is_any_of(delims_));
	//Loop over result and remove any empty cells
	for (StringVector::iterator it = string_parsed_raw.begin(); it != string_parsed_raw.end(); ++it) {
		//Check if this is empty
		if (it->size() > 0) {
			//Not empty, add to output
			string_parsed.push_back(*it);
		}
	}

	//Return vector without empties
	return string_parsed;
}