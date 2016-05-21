#pragma once
#include <string>
#include <vector>

//This class is intended to be a base class for any class that 
// is created from a text file
class FileBasedObject
{
private:
	//This will be populated with all the lines of the file by the constructor
	std::vector<std::string> _lines;

public:
	//Construct for a text file
	FileBasedObject(std::string file_path_);

	//Get const reference to _lines
	const std::vector<std::string>& getLines() const { return _lines; }
protected:
	//Allow derived classes to clear the contents of base
	void clearFileData();
};

