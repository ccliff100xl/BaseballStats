#pragma once
#include "FileBasedObject.h"

//Create a typedef to clarify code
typedef std::vector<std::string> CsvLine;

class CsvBasedObject : public FileBasedObject
{
private:
	std::vector<CsvLine> _csv_lines;
public:
	CsvBasedObject(std::string csv_file_path_);
	void clearCsvData();
	const std::vector<CsvLine>& getCsvLines() const { return _csv_lines; }
};

