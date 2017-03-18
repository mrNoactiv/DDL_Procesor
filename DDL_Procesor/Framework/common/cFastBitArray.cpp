#include "cFastBitArray.h"

cFastBitArray::cFastBitArray(unsigned int type)
{
	mByteSize = 0;
	mSeek = 0;
	mStatus = STATUS_END;
	mType = type;
}

cFastBitArray::~cFastBitArray()
{
}