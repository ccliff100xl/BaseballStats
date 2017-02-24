// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>

//This is used a lot, make it a type
typedef std::vector<std::string> StringVector;
//Also, make a function to create it
StringVector SplitStringToVector(const std::string str_, const char* delims_);

int char2int(char c_);
