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

int char2int(char c_)
{
	return c_ - '0';
}

//Will return true if OK, false if error
bool char2int_check(char c_, int& out_)
{
	out_ =  c_ - '0';
	//Only valid values for out_ are -9 to +9
	if (out_ < -9 || out_ > 9) {
		return false;
	}
	else {
		return true;
	}
}

int string2int(std::string s_)
{
	//Just try/catch around stoi
	try {
		return stoi(s_);
	}
	catch (std::exception& e) {
		//Print exception information
		std::cout << "string2int ERROR: " << std::endl << e.what() << std::endl;
		std::cout << "  Could not convert " << s_ << " to int " << std::endl;
		//Throw the exception
		throw(e);
	}
}

long string2longlong(std::string s_)
{
	//Just try/catch around stol
	try {
		return stoll(s_);
	}
	catch (std::exception& e) {
		//Print exception information
		std::cout << "string2longlong ERROR: " << std::endl << e.what() << std::endl;
		std::cout << "  Could not convert " << s_ << " to long long " << std::endl;
		//Throw the exception
		throw(e);
	}
}

//This will print the runtime from start_ to current
void printRuntime(std::clock_t start_)
{
	//Calculate time in seconds
	const double elapsed_s = clock2sec(std::clock() - start_);

	//Print, could expand to adjust units based on magnitude
	std::cout << "Total Time " << elapsed_s << " s" << std::endl;
}

//Convert output of std::clock (std::clock_t) to seconds
double clock2sec(std::clock_t elapsed_)
{
	return elapsed_ / static_cast<double>(CLOCKS_PER_SEC);
}