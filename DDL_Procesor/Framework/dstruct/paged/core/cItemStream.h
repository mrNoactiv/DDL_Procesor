/**
 *	\file cItemStream.h
 *	\author Michal Kratky
 *	\version 0.1
 *	\date apr 2011
 *	\brief Stream of items
 */

#ifndef __cItemStream_h__
#define __cItemStream_h__

#include "common/stream/cFileStream.h"
#include "common/stream/cCharStream.h"
#include "dstruct/paged/core/cNodeHeader.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"

using namespace common::datatype::tuple;
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
		
class cQuickDB;

template <class TKey> 
class cItemStream
{
protected:
	cFileStream *mStream;
	cCharStream *mBuffer;
	cDTDescriptor* mDTDescriptor;
	cNodeHeader* mNodeHeader;		/// Node header that may contain info about the stored data.
	cQuickDB* mQuickDB;
	bool mEndOfFile;			
	bool mReadMode;				/// Two modes: read and write.
	bool mFirstSegment;			/// True if we are still writting into the first segment during the Add.
	char mFilename[1024];
	unsigned int mItemCounter;	/// Auxiliary counter used during the read.
	unsigned int mItemCount;	/// Number of items in the stream array.
	unsigned int mLastItemSize; /// the size of the last item

	inline unsigned int GetItemSize(const char* item);
	bool Create(char *filename);
	bool Close();
	inline bool ReadBuffer();

private:
	//static const unsigned int BUFFER_LENGTH = 65536;	/// Size of one buffer page
	static const unsigned int BUFFER_LENGTH = 10000000;	/// Size of one buffer page //val644 - aby se neukladal result set
	static const int EOB;								/// End Of Buffer

public:
	cItemStream(char* fileName, cQuickDB* quickDB, unsigned int bufferLength64k = 1, bool readMode = true);
	~cItemStream();

	inline void SetDTDescriptor(cDTDescriptor* dtDesc);
	inline cDTDescriptor* GetDTDescriptor();
	bool Next();
	inline const char* GetItem();
	inline char* GetBuffer();
	//TKey* GetPItem();
	inline bool Add(const char*data);
	inline void FinishWrite();
	inline void Clear();
	inline bool Eof();
	inline unsigned int GetItemCount() const;
	void CloseResultSet();

	bool Open(char *filename);
	inline void SetFirstItem();
	// inline unsigned int GetDimension();
};
}}}

#include "dstruct/paged/core/cQuickDB.h"

namespace dstruct {
  namespace paged {
	namespace core {
template <class TKey> const int cItemStream<TKey>::EOB = -1;							/// End Of Buffer

template <class TKey>
cItemStream<TKey>::cItemStream(char *filename, cQuickDB* quickDB, unsigned int bufferLength64k, bool readMode)
{
	mBuffer = new cCharStream(BUFFER_LENGTH * bufferLength64k);
	mStream = new cFileStream();
	mReadMode = readMode;
	mQuickDB = quickDB;
	Create(filename);
}

template <class TKey>
cItemStream<TKey>::~cItemStream() 
{
	Close();
	delete mBuffer;
	delete mStream;
}

template <class TKey>
bool cItemStream<TKey>::Create(char *filename) 
{
	bool ret = false;
	mEndOfFile = false;
	strcpy(mFilename, filename);

	if (!mStream->Open(filename, ACCESS_READWRITE, FILE_CREATE, DIRECT_IO))
	{
		printf("cItemStream::Create - error, result set was not opened\n");
	}
	Clear();

	return ret;
}

//bool cItemStream::Open(char *filename) 
//{
//	bool ret = true;
//	mEndOfFile = false;
//	Clear();
//	mItemCounter = 0;
//
//	strcpy(mFilename, filename);
//
//	if (!mStream->Open(filename, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING))
//	{
//		printf("cItemStream::Open - error, result set was not opened\n");
//		ret = false;
//	}
//	else
//	{
//		ReadBuffer();
//	}
//	return ret;
//}

template <class TKey>
bool cItemStream<TKey>::Close() 
{
	if (!mReadMode)
	{
		// write mode? Then flush the buffer
		mBuffer->Write((char*)&EOB, sizeof(EOB));
		mBuffer->Write(mStream, BUFFER_LENGTH);
	}
	return mStream->Close();
}

/**
* This method should be copied into every inherited class due to the fakt that the
* GetSizeOfItem method is specific for every inherited method.
* Read the next item from this item stream.
*/
template <class TKey>
bool cItemStream<TKey>::Next()
{
	bool ret = true;

	if (mEndOfFile)
	{
		ret = false;
	}
	else
	{
		assert(mItemCounter < mItemCount);

		// try to read EOB
		if (*((int*)mBuffer->GetCharArray()) != EOB)
		{
			// read item
			mBuffer->SeekAdd(mLastItemSize);
			mLastItemSize = GetItemSize(mBuffer->GetCharArray());
			mItemCounter++;
		}
		else
		{
			if (ReadBuffer())
			{
				ret = Next();
			}
		}
	}

	if (mItemCounter == mItemCount)
	{
		ret = false;
	}

	if (!ret)
	{
		CloseResultSet();
	}

	return ret;
}

template <class TKey>
inline bool cItemStream<TKey>::ReadBuffer()
{
	bool ret = true;
	if (!mBuffer->Read(mStream, BUFFER_LENGTH))
	{
		printf("cItemStream::Next - error during the reading of the result set data file\n");
		exit(0);
	}
	return ret;
}

/**
* Prepare the item stream for writting
*/
template <class TKey>
void cItemStream<TKey>::Clear()
{
	mReadMode = false;
	mItemCount = 0;
	SetFirstItem();
}

template <class TKey>
void cItemStream<TKey>::SetFirstItem()
{
	mFirstSegment = true;
	mBuffer->Seek(0);
	mStream->Seek(0);
	mLastItemSize = 0;
	mItemCounter = 0;
}

/**
* \return Size of the item
*/
template <class TKey>
unsigned int cItemStream<TKey>::GetItemSize(const char* item)
{
	return TKey::GetSize(item, mDTDescriptor);
}

template <class TKey>
void cItemStream<TKey>::SetDTDescriptor(cDTDescriptor* dtDesc)
{
	mDTDescriptor = dtDesc;
}

template <class TKey>
cDTDescriptor* cItemStream<TKey>::GetDTDescriptor()
{
	return mDTDescriptor;
}

/**
* Add the data into the stream. Stream must be in a write mode.
* \param data Represents one item.
*/
template <class TKey>
bool cItemStream<TKey>::Add(const char* item)
{
	assert(!mReadMode);
	unsigned int freeSize = BUFFER_LENGTH - mBuffer->GetOffset();
	uint itemSize = GetItemSize(item);

	if (freeSize < (itemSize + sizeof(EOB)))
	{
		mFirstSegment = false;
		mBuffer->Write((char*)&EOB, sizeof(EOB));
		mBuffer->Write(mStream, BUFFER_LENGTH);
		mBuffer->Seek(0);
	}
	mItemCount++;
	return mBuffer->Write((char*)item, itemSize);
}

/**
* Finish writing into the stream and prepare for reading.
*/
template <class TKey>
void cItemStream<TKey>::FinishWrite()
{
	mReadMode = true;
	mEndOfFile = mItemCount == 0;
	mItemCounter = 0;
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
template <class TKey>
const char* cItemStream<TKey>::GetItem()
{
	return mBuffer->GetCharArray();
}

/*
char* cItemStream::GetBuffer()
{
	return mBuffer->GetCharArray();
}*/

template <class TKey>
bool cItemStream<TKey>::Eof()
{
	return mEndOfFile;
}

template <class TKey>
unsigned int cItemStream<TKey>::GetItemCount() const
{
	return mItemCount;
}

template <class TKey>
void cItemStream<TKey>::CloseResultSet()
{
	mEndOfFile = true;
	Clear();
	mQuickDB->AddResultSet((cItemStream<void>*)this);
}

}}}
#endif
