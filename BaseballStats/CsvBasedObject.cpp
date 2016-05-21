#include "CsvBasedObject.h"
#include <boost/algorithm/string.hpp>

using namespace std;

CsvBasedObject::CsvBasedObject(std::string csv_file_path_) : FileBasedObject(csv_file_path_)
{
	//Parse each line of _lines
	for (auto line : getLines()) {
		//Create line to parse to
		CsvLine line_parsed;
		//Parse line with comma delimiter
		boost::split(line_parsed, line, boost::is_any_of(","));
		//Add to vector
		_csv_lines.push_back(line_parsed);
	}

	//Entire file copied to parsed vector, clear parent
	clearFileData();
}

void CsvBasedObject::clearCsvData()
{
	//According to documentation, swapping is how you guarantee
	//allocated memory is freed.  Could be a runtime issue for large files
	vector<CsvLine> temp;
	_csv_lines.swap(temp);
}
