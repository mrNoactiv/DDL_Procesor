#ifndef cDataCollection_h__
#define cDataCollection_h__

#include <stdio.h>
#include <cstring>

#ifdef LINUX
static char* dirSeparator = "/";
#else
 static char* dirSeparator = "\\";
#endif

class cCollection
{
public: 
	typedef enum Collect { RANDOM = -1, XML = 0, METEO = 1, CARS = 2, WORDS = 3, ELECTRICITY = 4, POKER = 5, TIGER = 6, FORECAST = 7, STOCKS = 8, KDDCUP = 9, KDDCUP_DUPLICIES = 10, USCENSUS = 11, USAROAD = 12, USAHIGHWAYS = 13, XMARK = 14, HIGGS = 15, HOUSEHOLD_POWER = 16, PAMAP_PROTOCOL = 17, PAMAP_OPTIONAL = 18, RECORD_LINKAGE = 19, SUSY = 20, TWITTER_SMILEYS = 21, IP_TO_ZIP = 22, GTRANSIT = 23, EL_DATA = 24 };
};
class Computer
{
public: 
	typedef enum Collect { DBSYS = 0, DBEDU = 1, CODD = 2, MK = 3, PB = 4, MV = 5 };
};
class cDataCollection
{
private:
	static char* GetDirectory(int computer)
	{
		char* dataColDir = NULL;

		if (computer == Computer::DBSYS)
		{
			dataColDir = "k:\\data\\_Collections\\";
		}
		else if (computer == Computer::DBEDU)
		{
			dataColDir = "E:\\data\\_Collections\\";
		}
		else if (computer == Computer::PB)
		{
			dataColDir = "d:\\kolekce\\DERI\\";
		}
		else if (computer == Computer::CODD)
		{
			dataColDir = "/data/_Collections/";
		}
		else if (computer == Computer::MV)
		{
			dataColDir = "G:\\Skola\\_Collections\\";
		}
		else
		{
			printf("\nError: cDataCollection::GetDirectory(). Unspecified computer data directory.");
			exit(0);
		}
		return dataColDir;
	}

public:
	//static unsigned int COMPUTER;// = COMPUTER_PB; //set computer
	//static unsigned int COLLECTION ;//= Collection::POKER; //set data collection
	
	static char* COLLECTION_FILE(const int collection,const int computer)
	{
		char *colFileName,*dataColDir, *dataColFolder, *dataColName;
		colFileName = new char[2048];
		dataColDir = GetDirectory(computer);

		switch (collection)
		{
			case cCollection::XML   :  	dataColFolder = "XML_COLLECTION"; 	dataColName="Export_15_8.txt"; break;
			case cCollection::METEO :  	dataColFolder = "METEO"; 			dataColName="Export57.txt"; break;
			case cCollection::CARS  :  	dataColFolder = "CARS"; 			dataColName="Export.txt"; break;
			case cCollection::WORDS :  	dataColFolder = "DOCWORD";			dataColName="Export.txt"; break;
			case cCollection::POKER :  	dataColFolder = "POKER"; 			dataColName="Export.txt"; break;
			case cCollection::TIGER :  	dataColFolder = "TIGER"; 			dataColName="Export.txt"; break;
			case cCollection::FORECAST:	dataColFolder = "FORECAST"; 		dataColName="Export.txt"; break;
			case cCollection::STOCKS :  dataColFolder = "STOCKS"; 			dataColName="Export.txt"; break;
			case cCollection::USCENSUS: dataColFolder = "US_CENSUS_1990"; 	dataColName = "Export.txt"; break;
			case cCollection::USAROAD:	dataColFolder = "USA_ROADS"; 		dataColName = "Export.txt"; break;
			case cCollection::XMARK: 	dataColFolder = "XMARK"; 			dataColName = "Export.txt"; break;
			case cCollection::SUSY: 	dataColFolder = "SUSY"; 			dataColName = "Export.txt"; break;
			case cCollection::HIGGS: 	dataColFolder = "HIGGS"; 			dataColName = "Export.txt"; break;
			case cCollection::KDDCUP:	dataColFolder = "KDDCUP"; 			dataColName = "Export.txt"; break;
			case cCollection::KDDCUP_DUPLICIES:	dataColFolder = "KDDCUP"; 			dataColName="ExportDup.txt"; break;
			case cCollection::RECORD_LINKAGE: 	dataColFolder = "RECORD_LINKAGE"; 	dataColName = "Export.txt"; break;
			case cCollection::USAHIGHWAYS:		dataColFolder = "USA_HIGHWAYS"; 	dataColName = "Export.txt"; break;
			case cCollection::HOUSEHOLD_POWER: 	dataColFolder = "HOUSEHOLD_POWER"; 	dataColName = "Export.txt"; break;
			case cCollection::PAMAP_PROTOCOL: 	dataColFolder = "PAMAP"; 			dataColName = "Protocol.txt"; break;
			case cCollection::PAMAP_OPTIONAL: 	dataColFolder = "PAMAP"; 			dataColName = "Optional.txt"; break;
			case cCollection::TWITTER_SMILEYS: 	dataColFolder = "TWITTER_SMILEYS"; 	dataColName = "Export.txt"; break;
			case cCollection::IP_TO_ZIP: 	dataColFolder = "IP_TO_ZIP"; 			dataColName = "Export.txt"; break;
			case cCollection::GTRANSIT: 	dataColFolder = "GOOGLE_TRANSIT"; 			dataColName = "Export_NL.txt"; break;
			case cCollection::ELECTRICITY:		dataColFolder = "";					dataColFolder = "el2_elnet2012.txt"; break;
			case cCollection::EL_DATA: 	dataColFolder = "EL_DATA"; 			dataColName = "Export_19_11_2015.txt"; break;
			default: printf("\nCollection doesn't exist !!!"); dataColFolder = "";	dataColName = ""; break;
		}
		
		strcpy(colFileName, dataColDir);
		strcpy(colFileName + strlen(dataColDir), dataColFolder);
		strcpy(colFileName + strlen(dataColDir) + strlen(dataColFolder), dirSeparator);
		strcpy(colFileName + strlen(dataColDir) + strlen(dataColFolder) + strlen(dirSeparator), dataColName);
		return colFileName;
	}
	
	static char* QUERY_FILE(const int collection, const int computer)
	{
		char* fileName =  new char[2048];;
		char *queryFileDir,*queryFolder, *queryFile;

		queryFileDir = GetDirectory(computer);

		switch (collection)
		{
			// it is for dbedu.cs.vsb.cz
			case cCollection::XML   :  	queryFolder = "XML_COLLECTION"; queryFile="Queries_15_8.txt"; break;
			case cCollection::METEO :  	queryFolder = "METEO"; 			queryFile="Queries.txt"; break;
			case cCollection::CARS  :  	queryFolder = "CARS"; 			queryFile="Queries.txt"; break;
			case cCollection::WORDS :  	queryFolder = "DOCWORD"; 		queryFile="Queries.txt"; break;
			case cCollection::POKER :  	queryFolder = "POKER"; 			queryFile="Queries.txt"; break;
			case cCollection::TIGER :  	queryFolder = "TIGER"; 			queryFile="Queries2.txt"; break;
			case cCollection::FORECAST:	queryFolder = "FORECAST"; 		queryFile="Queries.txt"; break;
			case cCollection::STOCKS :  queryFolder = "STOCKS"; 		queryFile="Queries.txt"; break;
			case cCollection::USCENSUS:	queryFolder = "US_CENSUS_1990"; queryFile = "Queries.txt"; break;
			case cCollection::USAROAD:  queryFolder = "USA_ROADS"; 		queryFile = "Queries.txt"; break;
			case cCollection::XMARK: 	queryFolder = "XMARK"; 		queryFile = "Queries.txt"; break;
			case cCollection::SUSY: 	queryFolder = "SUSY"; 		queryFile = "Queries.txt"; break;
			case cCollection::HIGGS: 	queryFolder = "HIGGS"; 		queryFile = "Queries.txt"; break;
			case cCollection::KDDCUP :  queryFolder = "KDDCUP"; 	queryFile="Queries.txt"; break;
			case cCollection::KDDCUP_DUPLICIES:	queryFolder = "KDDCUP"; 		queryFile="Queries.txt"; break;
			case cCollection::RECORD_LINKAGE: 	queryFolder = "RECORD_LINKAGE"; queryFile = "Queries.txt"; break;
			case cCollection::USAHIGHWAYS: 		queryFolder = "USA_HIGHWAYS"; 	queryFile = "Queries.txt"; break;
			case cCollection::HOUSEHOLD_POWER: 	queryFolder = "HOUSEHOLD_POWER";queryFile = "Queries.txt"; break;
			case cCollection::PAMAP_PROTOCOL: 	queryFolder = "PAMAP"; 			queryFile = "Queries_Protocol.txt"; break;
			case cCollection::PAMAP_OPTIONAL: 	queryFolder = "PAMAP"; 			queryFile = "Queries_Optional.txt"; break;
			case cCollection::TWITTER_SMILEYS: 	queryFolder = "TWITTER_SMILEYS"; 	queryFile = "Queries.txt"; break;
			case cCollection::IP_TO_ZIP: 	queryFolder = "IP_TO_ZIP"; 			queryFile = "Queries.txt"; break;
			case cCollection::GTRANSIT: 	queryFolder = "GOOGLE_TRANSIT"; 			queryFile = "Queries.txt"; break;
			case cCollection::ELECTRICITY:		queryFolder = "";				queryFile = "el2_elnet2012.txt"; break;
			case cCollection::EL_DATA:		queryFolder = "EL_DATA";				queryFile = "pointqueries_43.qtf"; break;
			default: printf("\nQuery file doesn't exist !!!"); 	queryFolder = "";	queryFile = ""; break;
		}

		strcpy(fileName, queryFileDir);
		strcpy(fileName + strlen(queryFileDir), queryFolder);
		strcpy(fileName + strlen(queryFileDir) + strlen(queryFolder), dirSeparator);
		strcpy(fileName + strlen(queryFileDir) + strlen(queryFolder) + strlen(dirSeparator), queryFile);
		return fileName;
	}
};
#endif 