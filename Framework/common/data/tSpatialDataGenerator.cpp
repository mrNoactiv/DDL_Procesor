#include "tSpatialDataGenerator.h"

tRealData::tRealData(const char *file_name, cDataType *type) 
{ 
	read_buff = new char[10];
	mStream = new cIOStream();
	mDataSetDescriptor = new cDataSetDescriptor();

	printf("*** Test with real binnary data ***\n");
	if (!mStream->Open(file_name, OPEN_EXISTING))
	{
		printf("Critical Error: Open of Real collection %s failed!\n", file_name);
		exit(1);
	}

	mDataSetDescriptor->Read(mStream, type);
}

tRealData::~tRealData()
{
	delete mStream;
	delete read_buff;
}

void tRealData::Close()
{
	mStream->Close();
}

int tRealData::GetNext()
{
	unsigned int item;

	if (mStream->Read(read_buff, sizeof(int)))
	{
		item = *(unsigned int *)read_buff;
	}

	return item;
}


unsigned int tRealData::GetUNext()
{
	unsigned int item;

	if (mStream->Read(read_buff, sizeof(int)))
	{
		item = *(unsigned int *)read_buff;
	}

	return item;
}
