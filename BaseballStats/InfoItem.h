#pragma once
#include <string>

class InfoItem
{
	//Label for infromation
	std::string _label;

	//Information itself
	std::string _information;

public:
	//Construct completed item
	InfoItem(std::string label_, std::string information_) : _label(label_) , _information(information_) {}

	//Accessor
	std::string getLabel() const { return _label;  }
	std::string getInformation() const { return _information; }
};