/**
 *	\file cTreeItemStream.h
 *	\author Radim Baca
 *	\version 0.1
 *	\date oct 2011
 *	\brief Stream of pairs (key, value)
 */

#ifndef __cTreeItemStream_h__
#define __cTreeItemStream_h__

#include "dstruct/paged/core/cItemStream.h"
#include "dstruct/paged/core/cTreeNodeHeader.h"
#include "common/datatype/cDataVarLen.h"


/**
* Stream of items. Item is a pair (key, data); therefore the current version is written just for tree data structures.
* 
*
* \author Radim Baca 
* \version 0.1
* \date apr 2011
**/
namespace dstruct {
  namespace paged {
	namespace core {

template <class TKey> 
class cTreeItemStream: public cItemStream<TKey>
{
	typedef cItemStream<TKey> parent;

protected:

	inline unsigned int GetSizeOfItem(char* item);

private:
	//static const unsigned int BUFFER_LENGTH = 65536;	/// Size of one buffer page
	static const unsigned int BUFFER_LENGTH = 10000000;	/// Size of one buffer page //val644 - aby se neukladal result set
	static const int EOB;							/// End Of Buffer

public:
	cTreeItemStream(cTreeNodeHeader* header, unsigned int bufferLength64k = 1, bool readMode = true);
	~cTreeItemStream();

	inline void SetNodeHeader(cNodeHeader* nodeHeader);
	inline cNodeHeader* GetNodeHeader();

	bool Next();
	bool Add(const char*item);
	bool Add(const TKey &key, const char* data);
	inline bool Close();
	void CloseResultSet();
};

template <class TKey>
const int cTreeItemStream<TKey>::EOB = -1; /// End Of Buffer

template <class TKey>
cTreeItemStream<TKey>::cTreeItemStream(cTreeNodeHeader* header, unsigned int bufferLength64k, bool readMode)
	:cItemStream<TKey>(header, bufferLength64k, readMode)
{
	
}

template <class TKey>
cTreeItemStream<TKey>::~cTreeItemStream() 
{
	
}

template <class TKey>
void cTreeItemStream<TKey>::SetNodeHeader(cNodeHeader* nodeHeader)
{
	cItemStream<TKey>::mNodeHeader = nodeHeader;
	cItemStream<TKey>::SetDTDescriptor((cDTDescriptor*)nodeHeader->GetKeyDescriptor());
}

template <class TKey>
cNodeHeader* cTreeItemStream<TKey>::GetNodeHeader()
{
	return cItemStream<TKey>::mNodeHeader;
}

/**
* \return Size of the whole item (key + data).
*/
template <class TKey>
unsigned int cTreeItemStream<TKey>::GetSizeOfItem(char* item)
{
	unsigned int size = TKey::GetSize(item, parent::mNodeHeader->GetKeyDescriptor());
	if (((cTreeNodeHeader*)parent::mNodeHeader)->VariableLenDataEnabled())
	{
		return size + cDataVarlen::GetSize(item + size);
	} else
	{
		return size + ((cTreeNodeHeader*)parent::mNodeHeader)->GetDataSize();
	}
}



template <class TKey>
bool cTreeItemStream<TKey>::Close()
{
	return cItemStream<TKey>::Close();
}

template <class TKey>
bool cTreeItemStream<TKey>::Add(const TKey &key, const char* data)
{
	unsigned int freeSize = BUFFER_LENGTH - cItemStream<TKey>::mBuffer->GetOffset();
	unsigned int addSize, dataSize, keySize;
	
	assert(!parent::mReadMode);
	keySize = key.GetSize(parent::mNodeHeader->GetKeyDescriptor());
	if (((cTreeNodeHeader*)parent::mNodeHeader)->VariableLenDataEnabled())
	{
		dataSize = cDataVarlen::GetSize(data);
	} 
	else
	{
		dataSize = ((cTreeNodeHeader*)parent::mNodeHeader)->GetDataSize();
	}
	addSize = keySize + dataSize;
	bool ret;
	if (freeSize < (addSize + sizeof(EOB)))
	{
		parent::mFirstSegment = false;
		parent::mBuffer->Write((char*)&EOB, sizeof(EOB));
		parent::mBuffer->Write(parent::mStream, BUFFER_LENGTH);
		parent::mBuffer->Seek(0);
	}
	parent::mItemCount++;
	ret &= parent::mBuffer->Write(key.GetData(), keySize);
	ret &= parent::mBuffer->Write((char*)data, dataSize);
	return ret;

}

/**
* Add one (key,data) pair into the stream. Stream must be in a write mode.
* \param data Represents one item.
*/
template <class TKey>
bool cTreeItemStream<TKey>::Add(const char* item)
{
	unsigned int freeSize = BUFFER_LENGTH - cItemStream<TKey>::mBuffer->GetOffset();
	unsigned int addSize, dataSize, keySize;
	const char* data;
	
	assert(!parent::mReadMode);
	keySize = TKey::GetSize(item, parent::mNodeHeader->GetKeyDescriptor());
	data = item + keySize;
	if (((cTreeNodeHeader*)parent::mNodeHeader)->VariableLenDataEnabled())
	{
		dataSize = cDataVarlen::GetSize(data);
	} else
	{
		dataSize = ((cTreeNodeHeader*)parent::mNodeHeader)->GetDataSize();
	}
	addSize = keySize + dataSize;
	bool ret;
	if (freeSize < (addSize + sizeof(EOB)))
	{
		parent::mFirstSegment = false;
		parent::mBuffer->Write((char*)&EOB, sizeof(EOB));
		parent::mBuffer->Write(parent::mStream, BUFFER_LENGTH);
		parent::mBuffer->Seek(0);
	}
	parent::mItemCount++;
	ret &= parent::mBuffer->Write((char*)item, addSize);
	return ret;
}


/**
* Read the next item from this item stream
*/
template <class TKey>
bool cTreeItemStream<TKey>::Next()
{
	bool ret = true;

	if (parent::mEndOfFile)
	{
		return false;
	}

	if (parent::mItemCounter == parent::mItemCount)
	{
		//CloseResultSet();
		return false;
	}

	assert(parent::mItemCounter <= parent::mItemCount);

	parent::mBuffer->SeekAdd(parent::mLastItemSize);  // seek to the next item

	// try to read EOB
	if (*((int*)parent::mBuffer->GetCharArray()) != EOB)
	{
		// read item size
		parent::mLastItemSize = GetSizeOfItem(parent::mBuffer->GetCharArray());
		parent::mItemCounter++;
	}
	else
	{
		// try to read new page
		if (!parent::mBuffer->Read(parent::mStream, BUFFER_LENGTH))
		{
			printf("cItemStream::Next - error during the reading of the result ser data file\n");
			exit(0);
		}
		else
		{
			parent::mLastItemSize = 0;
			ret = Next();
		}
	}

	return ret;
}

template <class TKey>
void cTreeItemStream<TKey>::CloseResultSet()
{
	//parent::CloseResultSet();
	parent::mEndOfFile = true;
	parent::Clear();
	parent::mQuickDB->AddResultSet((cItemStream<void>*)this);
}
}}}
#endif