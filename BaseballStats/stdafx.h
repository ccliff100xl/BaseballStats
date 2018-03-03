// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include "SportsStatsConstants.h"

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <ctime>

//This is used a lot, make it a type
typedef std::vector<std::string> StringVector;
//Also, make a function to create it
StringVector SplitStringToVector(const std::string str_, const char* delims_);

//Conversions to numbers
int char2int(char c_);
bool char2int_check(char c_, int& out_);
int string2int(std::string s_);
long long string2longlong(std::string s_);

//Timing
void printRuntime(std::clock_t start_);
double clock2sec(std::clock_t elapsed);

//Strings to enums
EventResult string2enum(std::string str_, std::map< EventResult, const char * >& map_ );