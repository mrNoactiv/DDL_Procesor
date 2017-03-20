#ifndef __cRTreeLog_h__
#define __cRTreeLog_h__

#include "stdio.h"
#include "string.h"

class cRTreeLog
{
private:
	char mFilename[1024];
	FILE *mStream;

	int mRqOrder;
	int mResultSize;
	float mDac;
	float mDacReal;
	int mLeafNodeSearched;
	int mLeafNodeRelevant;

public:
	cRTreeLog();
	~cRTreeLog();

	void Open(const char *fileName);
	void Close();

	void Write(int resultSize, float dac, float dacReal, int leafNodeSearched, int leafNodeRelevant);
};
#endif;
