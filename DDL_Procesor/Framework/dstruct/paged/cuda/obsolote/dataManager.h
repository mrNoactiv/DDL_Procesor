#ifndef __dataManager_h__
#define __dataManager_h__
#pragma once
#include "globalDefs.h"
#include "dataDefs.h"
#include "singletonDestroyer.h"
#include "fileManager.h"
#include "memoryBuffer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	DataManager. Singleton class that holds DataStorage and additional data</summary>
///
/// <remarks>	</remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
class DataManager
{
private:
	static DataManager* pinstance;
	static SingletonDestroyer< DataManager > destroyer; 

	char *rq_data_file;
	char *rqs_file;
	char *input_data_file;
	char *results_data_file;
	char *time_file;

	unsigned int bufferLimitSizeInBytes;

public:
	static DataManager* Instance();
	MemoryBuffer* mb;
	FileManager *fm;
	DataStorage dataStorage;

public:
	DataManager();
	DataManager(const DataManager&);
	~DataManager(void);

	DataManager& operator= (const DataManager&);

	char *getRq_data_file(void) const { return(rq_data_file); };
	char *getRqs_file(void) const { return(rqs_file); };
	void setRq_data_file(char * _rq_data_file) { rq_data_file = _rq_data_file;	};
	char *getInput_data_file(void) const { return(input_data_file);	};
	void setInput_data_file(char *_input_data_file)	{ input_data_file = _input_data_file;	};
	char *getResults_data_file(void) const { return(results_data_file);	};
	void setResults_data_file(char * _results_data_file)	{ results_data_file = _results_data_file;	};
	char *getTime_file(void) const { return(time_file);	};
	void setTime_file(char * _time_file) { time_file = _time_file;	};

	bool parseCommandLineArguments(int argc, char** argv);
	bool setParameters(unsigned int dim,unsigned int norq);
	bool setParameters(char* dataFile, char* queryFile,unsigned int dim,unsigned int norq);
	bool prepareInputs();
	bool prepareInputs(unsigned int numberOfItems);
	bool prepareInputs(MemoryBuffer* mb);
	void checkDataDimensions();

	void saveResultsVector(const DATATYPE* buffer, unsigned int count, const bool append = false) const;
	void saveTime(const float tQuery, const float tCopyHD, const float tCopyDH, const unsigned int bufferSizeInBytes,const unsigned int resultSize, const bool append=false) const;
	bool loadRangeQueries();
	unsigned int getResultSize(const bool *H_bufferResults,const unsigned int currentInputs) const;
};
#endif