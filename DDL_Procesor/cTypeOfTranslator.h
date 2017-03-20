#pragma once
#include "cTranslatorCreate.h"


enum Type { CREATE_BTREE,CREATE_RTREE , INDEX ,CREATE };
class cTypeOfTranslator
{

public:

	Type type;

	void SetType(string input);
	cTypeOfTranslator();



};

cTypeOfTranslator::cTypeOfTranslator() :type()
{

}

void cTypeOfTranslator::SetType(string input)
{
	if (std::size_t foundDDL = input.find("create table", 0) == 0)
	{
		if (input.find("option:BTREE") != std::string::npos) {
			std::cout << "btree" << '\n';
			type = CREATE;
		}
		else if (input.find("option:RTREE") != std::string::npos) {
			std::cout << "Rtree" << '\n';
			type = CREATE;
		}
		else
		{
			std::cout << "no option found. btree created" << '\n';
			type = CREATE;
		}
		
		
	}
	else if (std::size_t foundDDL = input.find("create index", 0) == 0)
	{
		type = INDEX;
	}
}