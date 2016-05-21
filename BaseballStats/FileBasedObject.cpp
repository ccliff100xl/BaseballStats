#include "FileBasedObject.h"
#include <iostream>
#include <fstream>

using namespace std;

//This will load the file and copy each line as an entry in _lines
FileBasedObject::FileBasedObject(string file_path_)
{
	//Open file
	cout << "Parsing File " << file_path_ << endl;
	ifstream file_stream(file_path_);

	//Check if file was opened
	if (!file_stream.is_open()) {
		throw exception("Could not open file");
	}

	//Loop over lines of the file
	string line; //Hold each line
	while (getline(file_stream, line))
	{
		//Add line to vector
		_lines.push_back(line);
	}

	//Close file
	file_stream.close();
}

void FileBasedObject::clearFileData()
{
	//According to documentation, swapping is how you guarantee
	//allocated memory is freed.  Could be a runtime issue for large files
	vector<string> temp;
	_lines.swap(temp);
}
