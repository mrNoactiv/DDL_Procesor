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
	
	std::transform(input.begin(), input.end(), input.begin(), ::tolower);
	
	if ( input.find("create table", 0) == 0 || input.find("CREATE TABLE", 0) == 0)
	{
		if (input.find("option:btree") != std::string::npos) {
			std::cout << "btree" << '\n';
			type = CREATE;
		}
		else if (input.find("option:md_table") != std::string::npos) //rstrom
		{
			std::cout << "Rtree" << '\n';
			type = CREATE;
		}
		else if (input.find("option:clustered_table") != std::string::npos) {
			std::cout << "clustered table" << '\n';
			type = CREATE;
		}
		else
		{
			type = CREATE;
		}
		
		
	}
	else if (input.find("create index", 0) == 0 || input.find("CREATE INDEX", 0) == 0)
	{
		type = INDEX;
	}
}