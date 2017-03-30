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
	if ( input.find("create table", 0) == 0 || input.find("CREATE TABLE", 0) == 0)
	{
		if (input.find("OPTION:BTREE") != std::string::npos) {
			std::cout << "btree" << '\n';
			type = CREATE;
		}
		else if (input.find("OPTION:MD_table") != std::string::npos) //rstrom
		{
			std::cout << "Rtree" << '\n';
			type = CREATE;
		}
		else if (input.find("OPTION:CLUSTERED_table") != std::string::npos) {
			std::cout << "clustered table" << '\n';
			type = CREATE;
		}
		else
		{
			std::cout << "no option found. btree created" << '\n';
			type = CREATE;
		}
		
		
	}
	else if (input.find("create index", 0) == 0 || input.find("CREATE INDEX", 0) == 0)
	{
		type = INDEX;
	}
}