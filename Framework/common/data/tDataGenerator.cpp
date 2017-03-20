#include "tDataGenerator.h"

tNormalizedRealData::tNormalizedRealData(char *file_name) 
{ 
	read_buff = new char[10];
	inverse_buff = new char[10];
	mStream = new cFileInputStream();

	printf("*** Test with real normalized binnary data ***\n");
	if (!mStream->Open(file_name, FILE_OPEN))
	{
		printf("Critical Error: Open of Real collection %s failed!\n", file_name);
		exit(1);
	}
}

tNormalizedRealData::~tNormalizedRealData()
{
	mStream->Close();
	delete mStream;
	delete read_buff;
	delete inverse_buff;
}

int tNormalizedRealData::GetNext()
{
	float real_item = 0;

	if (mStream->Read(read_buff, sizeof(float)))
	{
		inverse_buff[3] = read_buff[0];
		inverse_buff[2] = read_buff[1];
		inverse_buff[1] = read_buff[2];
		inverse_buff[0] = read_buff[3];
		real_item = *(float *)inverse_buff;
	}

	return (int)(real_item * 100000000);
}


unsigned int tNormalizedRealData::GetUNext()
{
	float real_item;

	if (mStream->Read(read_buff, sizeof(float)))
	{
		inverse_buff[3] = read_buff[0];
		inverse_buff[2] = read_buff[1];
		inverse_buff[1] = read_buff[2];
		inverse_buff[0] = read_buff[3];
		real_item = *(float *)inverse_buff;
	}

	return (unsigned int)(real_item * 100000000);
}

// It skip bytes in a normalized data. This because there is a lot of the same tuples
void tNormalizedRealData::Skip(unsigned int bytes)
{
	for (unsigned int i = 0; i < bytes; i++)
	{
		mStream->Read(read_buff, 1);
	}
}
