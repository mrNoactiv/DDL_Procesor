#include "dataManager.h"

DataManager* DataManager::pinstance = 0;// initialize pointer
SingletonDestroyer<DataManager> DataManager::destroyer;

DataManager* DataManager::Instance() 
{
	if (pinstance == 0)  // is it the first call?
	{  
		pinstance = new DataManager(); // create sole instance
		destroyer.SetDoomed(pinstance); 
	}
	return pinstance; // address of sole instance
}

DataManager::DataManager() : dataStorage(DataStorage(0,0))
{
	//Default settings
	rq_data_file = "rqData.bin";
	input_data_file = "inputData.bin";
	results_data_file = "resultsData.bin";
	time_file = "time.txt";
	fm = 0;
	//mb = new MemoryBuffer();
	bufferLimitSizeInBytes = 1<<28;
}

DataManager::~DataManager(void)
{
	if (fm)
		SAFE_DELETE(fm);
}

bool DataManager::parseCommandLineArguments(int argc, char** argv)
{
	if (argc == 1) return false;

	int item = 0;
	char *switcher = 0;
	bool switcherSet = false;

	int dim = 0;
	int noRQ = 0;

	for (int i=1; i<argc; i++)
	{
		if ((strcmp(argv[i],"-rq") == 0)||
			(strcmp(argv[i],"-i") == 0)||
			(strcmp(argv[i],"-r") == 0)||
			(strcmp(argv[i],"-t") == 0)||
			(strcmp(argv[i],"-dim") == 0)||
			(strcmp(argv[i],"-norq") == 0)||
			(strcmp(argv[i],"-rqsfile") == 0)||
			(strcmp(argv[i],"-bs") == 0))
		{
			switcher = argv[i];
			switcherSet = true;
			continue;
		}

		if (switcherSet)
		{
			if (strcmp(switcher, "-rq") == 0)
				rq_data_file = argv[i];
			else if (strcmp(switcher, "-i") == 0)
				input_data_file = argv[i];
			else if (strcmp(switcher, "-r") == 0)
				results_data_file = argv[i];
			else if (strcmp(switcher, "-t") == 0)
				setTime_file(argv[i]);
			else if (strcmp(switcher, "-dim") == 0)
				dim = (unsigned int )(atoi(argv[i]));
			else if (strcmp(switcher, "-norq") == 0)
				noRQ = atoi(argv[i]);
			else if (strcmp(switcher, "-rqsfile") == 0)
				rqs_file = argv[i];

			else if (strcmp(switcher, "-bs") == 0)
				bufferLimitSizeInBytes = (unsigned int )(atoi(argv[i]));
			switcherSet = false;
		}
	}
	dataStorage = DataStorage(dim, noRQ);
	return true;
}
//For buffer of input vectors in memory
bool DataManager::setParameters(unsigned int dim,unsigned int norq)
{

	results_data_file = "results.r";
	setTime_file("cuda_query.time");
	dim = dim;
	bufferLimitSizeInBytes =268435440;
	dataStorage = DataStorage(dim, norq);
	return true;
}
bool DataManager::setParameters(char* dataFile, char* queryFile,unsigned int dim,unsigned int norq)
{

	input_data_file = dataFile;
	results_data_file = "results.r";
	setTime_file("cuda_query.time");
	dim = dim;
	rqs_file = queryFile;
	bufferLimitSizeInBytes =268435440;

	dataStorage = DataStorage(dim, norq);
	return true;
}

bool DataManager::prepareInputs()
{
	fm = new FileManager();
	if (!fm->openFile(input_data_file, dataStorage.info.dim)) 
	{
		printf("Critical Error: Could not open input file: %s\n", input_data_file);
		exit(0);
		return false;
	}
	if (!fm->allocateBuffer(bufferLimitSizeInBytes, dataStorage.info.dim * sizeof(DATATYPE))) 
	{
		printf("Critical Error: Could not allocate buffer. BufferLimitSIzeInBytes: %d, ItemSizeInBytes: %d\n", bufferLimitSizeInBytes, dataStorage.info.dim * sizeof(DATATYPE));
		exit(0);
		return false;
	}

	dataStorage.data.noInputVectors = fm->getNumberOfItems();
	dataStorage.data.H_inputVectors = (DATATYPE*)fm->buffer;

	//dataStorage.data.H_results = new bool[dataStorage.data.noInputVectors];
	return true;
}
bool DataManager::prepareInputs(unsigned int numberOfItems)
{
	dataStorage.data.noInputVectors =numberOfItems;
	return true;
}
//Prepare inputs for memory buffer
bool DataManager::prepareInputs(MemoryBuffer* _mb)
{
	mb=_mb;
	/*if (!mb->Allocate(bufferLimitSizeInBytes, dataStorage.info.dim * sizeof(DATATYPE))) 
	{
		printf("Critical Error: Could not allocate buffer. BufferLimitSIzeInBytes: %d, ItemSizeInBytes: %d\n", bufferLimitSizeInBytes, dataStorage.info.dim * sizeof(DATATYPE));
		exit(0);
		return false;
	}*/
	dataStorage.data.noInputVectors = mb->GetNumberOfTuples();
	dataStorage.data.H_inputVectors = (DATATYPE*)mb->GetItemArray();
	return true;
}
void DataManager::saveResultsVector(const DATATYPE* buffer, unsigned int count, const bool append) const
{
	FILE * file;

	char* fileName = new char[256];
	//char* fileExt = new char[3];
	//itoa(0, fileExt, 10);			//0 = file extension :-) just for further use

	strcpy(fileName, results_data_file);
	//strcat(fileName, ".");
	//strcat(fileName, fileExt);

	file = (append) ? fopen(fileName,"ab") : fopen(fileName,"wb");
	fwrite(buffer, sizeof(bool), count, file);
	fflush(file);   
	fclose(file);
}


void DataManager::saveTime(const float tQuery, const float tCopyHD, const float tCopyDH, const unsigned int bufferSizeInBytes,const unsigned int resultSize, const bool append) const
{
	FILE * file;
	file = (append) ? fopen(time_file,"at") : fopen(time_file,"wt");

	//fprintf(file, "%u\t%f\t%f\t%f\n", bufferSizeInBytes, tQuery, tCopyHD, tCopyDH);
	unsigned int dim = 8;
	fprintf(file, "%u;%f;%f;%f;%u\n", bufferSizeInBytes, tQuery, tCopyHD, tCopyDH,resultSize);

	fflush(file);   
	fclose(file);
}

bool DataManager::loadRangeQueries() 
{
	if (dataStorage.info.noRQ == 0) return false;
	FILE * file = 0;
	file = fopen(rq_data_file,"rb");
	if (file != 0)
	{
		unsigned int r = (unsigned int)fread(&dataStorage.data.H_rqs[0], dataStorage.info.dim * sizeof(RQElement), dataStorage.info.noRQ, file);
		fclose(file);
		return true;
	}
	return false;
}

unsigned int DataManager::getResultSize(const bool *H_bufferResults,const unsigned int currentInputs) const
{
	unsigned int validResults = 0;
	for (unsigned int i = 0; i < currentInputs;i++)
	{
		if (H_bufferResults[i] == true)
			validResults++;
	}
	return validResults;
}