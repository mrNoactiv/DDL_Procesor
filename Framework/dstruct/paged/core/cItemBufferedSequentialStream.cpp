#include "dstruct/paged/core/cItemBufferedSequentialStream.h"

namespace dstruct {
  namespace paged {
	namespace core {

//cItemBufferedSequentialStream::cItemBufferedSequentialStream(cQuickDB* quickDB, unsigned int bufferLength64k, bool readMode)
//{
//	mBuffer = new cCharStream(BUFFER_LENGTH * bufferLength64k);
//	mStream = new cIOStream();
//	mReadMode = readMode;
//	mQuickDB = quickDB;
//}

cItemBufferedSequentialStream::cItemBufferedSequentialStream(unsigned int bufferLength64k, bool readMode)
{
	mBuffer = new cCharStream(BUFFER_LENGTH * bufferLength64k);
	mStream = new cFileStream();
	mReadMode = readMode;
}

cItemBufferedSequentialStream::~cItemBufferedSequentialStream() 
{
	Close();
	delete mBuffer;
	delete mStream;
}


bool cItemBufferedSequentialStream::Create(char *filename) 
{
	bool ret = false;
	mEndOfFile = false;
	strcpy(mFilename, filename);

	if (!mStream->Open(filename, CREATE_ALWAYS, FILE_FLAG_NO_BUFFERING))
	{
		printf("cItemBufferedSequentialStream::Create - error, result set was not opened\n");
	}
	Clear();
	return ret;
}

bool cItemBufferedSequentialStream::Open(char *filename, unsigned int itemSize) 
{
	bool ret = true;
	Clear();
	mItemSize = itemSize;

	strcpy(mFilename, filename);

	if (!mStream->Open(filename, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING))
	{
		printf("cItemBufferedSequentialStream::Open - error, result set was not opened\n");
		ret = false;
	}
	else
	{
		ReadBuffer();
	}
	return ret;
}

bool cItemBufferedSequentialStream::Close() 
{
	if (!mReadMode)
	{
		// write mode? Then flush the buffer
		mBuffer->Write((char*)&EOB, sizeof(EOB));
		mBuffer->Write(mStream, BUFFER_LENGTH);
	}
	else
	{
		mEndOfFile = true;
		Clear();
	}
	return mStream->Close();
}

/**
* This method should be copied into every inherited class due to the fact that the
* GetSizeOfItem method is specific for every inherited method.
* Read the next item from this item stream.
*/
bool cItemBufferedSequentialStream::Next()
{
	bool ret = true;

	if (mEndOfFile)
	{
		ret = false;
	}
	else
	{
		// seek in the array and detect EOB
		mBuffer->SeekAdd(mLastItemSize);

		if (*((int*)mBuffer->GetCharArray()) != EOB)
		{
			mLastItemSize = mItemSize; // GetSizeOfItem(mBuffer->GetCharArray());
			mItemIndex++;
		}
		else
		{
			if ((ret = ReadBuffer()))
			{
				mLastItemSize = 0; // reset the last item size
				ret = Next();
			}
		}
	}

	if (!ret)
	{
		Close();
	}

	return ret;
}

inline bool cItemBufferedSequentialStream::ReadBuffer()
{
	// uint count = 1048576;
	return mBuffer->Read(mStream, BUFFER_LENGTH);
	// printf("cItemBufferedSequentialStream::Next - error during the reading of the result set data file\n");
	// exit(0);
}
}}}