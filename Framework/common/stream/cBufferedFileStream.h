/**
 *	\file cFileStream.h
 *	\author Jiøí Dvorský (1998), Michal Krátký (2014)
 *	\version 0.2
 *	\date jan 2014
 *	\brief File streams
 */

#ifndef __cBufferedFileStream_h__
#define __cBufferedFileStream_h__

#include <thread>
#include <atomic>

#include "common\cCommon.h"
#include "cFileStream.h"

using namespace common;

namespace common {
	namespace stream {

class cBufferedFileStream: public cFileStream
{
	char** mBuffer;          // all buffers
	uint mBufferStatus[2];   // status of buffers
	int mBufferSizes[2];    // status of buffers

	char* mBufferUsed;       // the buffer currently used for reading
	uint mBufferOffset;      // the offset in the buffer used
	int mBufferSize;         // the size of the buffer used
	uint mOrder_BufferUsed;  // the order of the buffer used in mBuffer

	// flags
	std::atomic_bool mFlag_BufferPrepared;
	std::atomic_bool mFlag_NotFileEnd;
	std::atomic_bool mFlag_RunIoThread;

	std::thread* mIoThread;
	uint mReadOrder;

private:
	static const uint BUFFER_SIZE = 524288; // 65536 131072   2097152   524288    8192   
	static const uint BUFFER_COUNT = 2;

	static const uint STATE_BUFFER_EMPTY = 0;
	static const uint STATE_BUFFER_FULL = 1;
	static const uint STATE_BUFFER_READ = 2;

private: 
	void IoThreadLoop();
	void WaitForData();
	inline uint NextBufferOrder(uint currentBufferOrder);
	inline void SetBufferUsed(uint bufferOrder);

public:
	cBufferedFileStream(uint bufferSize = BUFFER_SIZE, bool parallelIOEnabled = true);
	~cBufferedFileStream();

	virtual bool Open(const char* name, const ushort accessMode, const ushort createMode, const ushort flags = FLAGS_NORMAL);

	bool ReadBuffer(char **mem, uint size, uint &count);
};

uint cBufferedFileStream::NextBufferOrder(uint currentBufferOrder)
{
	uint nextBufferOrder = 0;
	if (currentBufferOrder == 0)
	{
		nextBufferOrder = 1;
	}
	return nextBufferOrder;
}

void cBufferedFileStream::SetBufferUsed(uint bufferOrder)
{
	mOrder_BufferUsed = bufferOrder;
	mBufferUsed = mBuffer[mOrder_BufferUsed];
	mBufferStatus[mOrder_BufferUsed] = STATE_BUFFER_READ;
	mBufferSize = mBufferSizes[mOrder_BufferUsed];
	mBufferOffset = 0;
	mFlag_BufferPrepared = true;
}

}}

#endif  //  __cBufferedFileStream_h__