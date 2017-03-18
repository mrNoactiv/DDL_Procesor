#include "cRTreeLog.h"

/// Constructor
cRTreeLog::cRTreeLog()
{
}

cRTreeLog::~cRTreeLog()
{
	Close();
}

void cRTreeLog::Open(const char *fileName)
{
	/*char tmp[1024];
	strcpy(tmp, fileName);
	strcpy(tmp + strlen(fileName), "_log.txt");
	fopen_s(&mStream, tmp, "a");*/

	strcpy(mFilename, fileName);
	strcpy(mFilename + strlen(fileName), "_log.txt");

	mRqOrder = 1;
	mResultSize = mLeafNodeSearched = mLeafNodeRelevant = 0;
	mDac = mDacReal = 0.0;
}

void cRTreeLog::Close()
{
	// fclose(mStream);
}

void cRTreeLog::Write(int resultSize, float dac, float dacReal, int leafNodeSearched, int leafNodeRelevant)
{
	mStream = fopen(mFilename, "a");

	fprintf(mStream, "%d: ", mRqOrder);
	fprintf(mStream, "res. size: %d, ", resultSize);
	fprintf(mStream, "dac: %d, ", (int)dac);
	/*fprintf(mStream, "dac (real): %.2f, ", dacReal);
	fprintf(mStream, "dac (cached): %.2f, ", dac - dacReal);*/
	fprintf(mStream, "searched: %d, ", leafNodeSearched);
	fprintf(mStream, "relevant: %d, ", leafNodeRelevant);
	fprintf(mStream, "rlv: %.2f\n", ((float)leafNodeRelevant / leafNodeSearched));

	mResultSize += resultSize;
	fprintf(mStream, "res. size: %.2f, ", ((float)mResultSize / mRqOrder));
	mDac += dac;
	fprintf(mStream, "dac: %.2f, ", ((float)mDac / mRqOrder));
	mDacReal += dac;
	/*fprintf(mStream, "dac (real): %.2f, ", (mDacReal / mRqOrder));
	fprintf(mStream, "dac (cached): %.2f, ", ((mDac - mDacReal) / mRqOrder));*/
	mLeafNodeSearched += leafNodeSearched;
	fprintf(mStream, "searched: %.2f, ", ((float)mLeafNodeSearched / mRqOrder));
	mLeafNodeRelevant += leafNodeRelevant;
	fprintf(mStream, "relevant: %.2f, ", ((float)mLeafNodeRelevant / mRqOrder));
	fprintf(mStream, "rlv: %.2f\n\n", ((float)mLeafNodeRelevant / mLeafNodeSearched));

	mRqOrder++;
	// fflush(mStream);
	fclose(mStream);
}