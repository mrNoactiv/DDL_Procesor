#include "cBufferedFileStream.h"

namespace common {
	namespace stream {

cBufferedFileStream::cBufferedFileStream(uint bufferSize, bool parallelIOEnabled):
	cFileStream()
{
	mBuffer = new char*[BUFFER_COUNT];
	for (uint i = 0 ; i < BUFFER_COUNT ; i++)
	{
		mBuffer[i] = new char[BUFFER_SIZE];
		mBufferStatus[i] = STATE_BUFFER_EMPTY;
	}
}

cBufferedFileStream::~cBufferedFileStream()
{
	if (mBuffer != NULL)
	{
		for (uint i = 0 ; i < BUFFER_COUNT ; i++)
		{
			delete mBuffer[i];
		}
		delete mBuffer;
		mBuffer = NULL;
	}

	mFlag_RunIoThread = false;
	mIoThread->join();
}

bool cBufferedFileStream::Open(const char* name, const ushort accessMode, const ushort createMode, const ushort flags)
{
	bool ret = cFileStream::Open(name, accessMode, createMode, flags);

	mFlag_RunIoThread = true;
	mFlag_BufferPrepared = false;
	mReadOrder = 0;

	mIoThread = new std::thread(&cBufferedFileStream::IoThreadLoop, this);
	WaitForData();

	return ret;
}

bool cBufferedFileStream::ReadBuffer(char **mem, uint size, uint &count)
{
	bool ret = true;

	if (mBufferOffset < mBufferSize)
	{
		count = mBufferSize - mBufferOffset;
		*mem = mBufferUsed + mBufferOffset;
		mBufferOffset += count;
	}
	else
	{
		mBufferStatus[mOrder_BufferUsed] = STATE_BUFFER_EMPTY;
		mFlag_BufferPrepared = false;

		WaitForData();

		if (mFlag_NotFileEnd)
		{
			ret = ReadBuffer(mem, size, count);
		} else
		{
			ret = false;
		}
	}

	return ret;
};

void cBufferedFileStream::IoThreadLoop()
{
	while(mFlag_RunIoThread)  // only destructor can break the cycle
	{
		if (!mFlag_BufferPrepared && mBufferStatus[0] == STATE_BUFFER_EMPTY &&
			mBufferStatus[1] == STATE_BUFFER_EMPTY || mFlag_BufferPrepared)
		{
			for (uint i = 0 ; i < BUFFER_COUNT ; i++)
			{
				if (mBufferStatus[i] == STATE_BUFFER_EMPTY)
				{
					mFlag_NotFileEnd = Read(mBuffer[i], BUFFER_SIZE, &mBufferSizes[i]);
					// printf("Read: %d  \n", mReadOrder);
					mBufferStatus[i] = STATE_BUFFER_FULL;
					mReadOrder++;
					break;
				}
			}
		}

		if (mBufferStatus[0] == STATE_BUFFER_EMPTY && mBufferStatus[1] == STATE_BUFFER_FULL)
		{
			SetBufferUsed(1);
		}
		else if (mBufferStatus[0] == STATE_BUFFER_FULL && mBufferStatus[1] == STATE_BUFFER_EMPTY)
		{
			SetBufferUsed(0);
		}
		else if (mBufferStatus[0] == STATE_BUFFER_FULL && mBufferStatus[1] == STATE_BUFFER_FULL)
		{
			SetBufferUsed(NextBufferOrder(mOrder_BufferUsed));
		}
	}
}

void cBufferedFileStream::WaitForData()
{
	while(!mFlag_BufferPrepared && mFlag_NotFileEnd) { };
}

}}