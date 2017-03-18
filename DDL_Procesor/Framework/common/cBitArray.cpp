#include "cBitArray.h"

cBitArray::cBitArray(unsigned int type)
{
	mByteSize = 0;
	mSeek = 0;
	mStatus = STATUS_END;
	mType = type;
}

cBitArray::~cBitArray()
{
}