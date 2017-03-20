/**
 *	\file cItemBufferedSequentialStream.h
 *	\author Michal Kratky
 *	\version 0.1
 *	\date apr 2011
 *	\brief Stream of items
 */

#ifndef __cItemBufferedSequentialStream_h__
#define __cItemBufferedSequentialStream_h__

#include <crtdbg.h>
#include "common/stream/cFileStream.h"
#include "common/stream/cCharStream.h"

using namespace common::stream;

/**
* Stream of character blocks. Can store a set of items.
* This class can serve as a super class for concrete inherited implementations.
* Inherited classes can not have an extra attributes! All attributes must be declared here.
*
* Usage scenario:
* 1. We call Clear method to prepare for writting
* 2. We add items by add method
* 3. We call FinishWrite
* 4. We call Next until it returns false
*
* \author Michal Kratky, Radim Baca
* \version 0.1
* \date apr 2011
**/
namespace dstruct {
  namespace paged {
	namespace core {

class cItemBufferedSequentialStream
{
protected:
	cFileStream *mStream;
	cCharStream *mBuffer;
	bool mEndOfFile;			
	bool mReadMode;				      /// Two modes: read and write.
	bool mFirstSegment;			    /// True if we are still writting into the first segment during the Add.
	char mFilename[1024];
	unsigned int mItemIndex;	  /// Auxiliary counter used during the read.
	unsigned int mItemCount;	  /// Number of items in the stream array.
	unsigned int mItemSize;     /// Size of the item, set 0 if it is a varlen item
	unsigned int mLastItemSize; /// the size of the last item

	// inline unsigned int GetSizeOfItem(char* item);
	bool Create(char *filename);
	bool Close();
	inline void Clear();
	inline bool ReadBuffer();

private:
	static const unsigned int BUFFER_LENGTH = 65536;	/// Size of one buffer page
	static const int EOB = -1;							          /// End Of the Block

public:
	cItemBufferedSequentialStream(unsigned int bufferLength64k = 1, bool readMode = true);
	~cItemBufferedSequentialStream();

	bool Open(char *filename, unsigned int size);

	bool Next();
	inline const char* GetItem();
	//TKey* GetPItem();
	inline bool Add(const char*data);
	inline void FinishWrite();
	inline bool Eof();
	inline unsigned int GetItemCount() const;
};

/**
* Prepare the item stream for writting
*/
void cItemBufferedSequentialStream::Clear()
{
	mItemIndex = 0;
	mItemCount = 0;
	mFirstSegment = true;
	mEndOfFile = false;
	mBuffer->Seek(0);
	mStream->Seek(0);
	mLastItemSize = 0;
	mItemSize = 0;
}

/**
* \return Size of the item
*/
//unsigned int cItemBufferedSequentialStream::GetSizeOfItem(char* item)
//{
//	 // printf("cItemBufferedSequentialStream::GetSizeOfItem - should be implemented in the inherited class\n");
//	 // return -1;
//	return mHeader->GetItemInMemSize();
//}

/**
* Add the data into the stream. Stream must be in a write mode.
* \param data Represents one item.
*/
bool cItemBufferedSequentialStream::Add(const char*data)
{
	assert(!mReadMode);
	unsigned int freeSize = BUFFER_LENGTH - mBuffer->GetOffset();
	if (freeSize < (mItemSize + sizeof(EOB)))
	{
		mFirstSegment = false;
		mBuffer->Write((char*)&EOB, sizeof(EOB));
		mBuffer->Write(mStream, BUFFER_LENGTH);
		mBuffer->Seek(0);
	}
	mItemCount++;
	return mBuffer->Write((char*)data, mItemSize);
}

/**
* Finish writing into the stream and prepare for reading.
*/
void cItemBufferedSequentialStream::FinishWrite()
{
	mReadMode = true;
	mEndOfFile = mItemIndex == 0;
	if (!mFirstSegment)
	{
		mBuffer->Write((char*)&EOB, sizeof(EOB));
		mBuffer->Write(mStream, BUFFER_LENGTH);
		mStream->Seek(0);
		mBuffer->Read(mStream, BUFFER_LENGTH);
	}
	mBuffer->Seek(0);
	mLastItemSize = 0;
}

/**
* \return Pointer to the actual item
*/
const char* cItemBufferedSequentialStream::GetItem()
{
	return mBuffer->GetCharArray();
}

bool cItemBufferedSequentialStream::Eof()
{
	return mEndOfFile;
}

unsigned int cItemBufferedSequentialStream::GetItemCount() const
{
	return mItemCount;
}

}}}
#endif