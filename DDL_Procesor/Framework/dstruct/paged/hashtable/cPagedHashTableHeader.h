/**
*	\file cPagedHashTableHeader.h
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.1
*	\date jul 2002
*	\version 0.2
*	\date jul 2011
*	\brief Header of hash table
*/

#ifndef __cPagedHashTableHeader_h__
#define __cPagedHashTableHeader_h__

#include "common/stream/cStream.h"
#include "dstruct/paged/core/cNode.h"
#include "dstruct/paged/core/cDStructHeader.h"
#include "dstruct/paged/hashtable/cPagedHashTableNodeHeader.h"

using namespace common;
using namespace common::datatype::tuple;
using namespace dstruct::paged::core;

// #define TITLE_SIZE 128

namespace dstruct {
  namespace paged {
	namespace hashtable {

/**
*	Header of hash table
*
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.2
*	\date jul 2011
**/

template <class TKey>
class cPagedHashTableHeader: public cDStructHeader
{
private:
	unsigned int mHashTableSize;
	unsigned int mMinStringLength;
	unsigned int mMaxStringLength;

	// for extendible and linear hashing
	tNodeIndex mUnusedNodesCount;	// Count of stored unused nodes
	tNodeIndex mUnusedNodesSize;	// Size of unused nodes cache
	tNodeIndex* mUnusedNodes;		// Unused nodes cache array
public:
	cPagedHashTableHeader(const char* dsName, cDTDescriptor* pSd, const uint keySize, const uint dataSize, bool varlenData);
	cPagedHashTableHeader();
	cPagedHashTableHeader(const cPagedHashTableHeader &header);
	~cPagedHashTableHeader();

	bool Write(cStream *stream);
	bool Read(cStream *stream);

	inline void Init(const char* dsName, cDTDescriptor* pSd, const uint keySize, const uint dataSize, bool varlenData);
	void HeaderSetup(/*unsigned int maxNodeInMemSize,*/ unsigned int blockSize);
	inline unsigned int GetNodeCount() const;
	inline unsigned int GetItemCount() const;

	inline cPagedHashTableNodeHeader<TKey>* GetNodeHeader();

	inline void SetHashTableSize(unsigned int size);
	inline unsigned int GetHashTableSize() const;

	inline void SetMaxStringLength(unsigned int stringLength);
	inline unsigned int GetMaxStringLength() const;

	// for extendible and linear hashing
	void SetUnusedNode(tNodeIndex nodeIndex);
	tNodeIndex GetUnusedNode();
};

template <class TKey>
dstruct::paged::hashtable::cPagedHashTableHeader<TKey>::cPagedHashTableHeader()
{

}

template <class TKey>
cPagedHashTableHeader<TKey>::cPagedHashTableHeader(const char* dsName, cDTDescriptor* pSd, const unsigned int keySize, 
	const unsigned int dataSize, bool varlenData)
{
	mHashTableSize = -1;
	mMaxStringLength = -1;
	mMinStringLength = -1;

	// for extendible and linear hashing
	mUnusedNodes = NULL;
	mUnusedNodesCount = 0;
	mUnusedNodesSize = 0;

	Init(dsName, pSd, keySize, dataSize, varlenData);
}

/// !! copy headers !!
template <class TKey>
cPagedHashTableHeader<TKey>::cPagedHashTableHeader(const cPagedHashTableHeader &header): cDStructHeader(header)
{
	mHashTableSize = header.GetHashTableSize();
	mMaxLength = header.GetMaxLength();
	mMinLength = header.GetMinLength();
	AddHeaderSize(3 * sizeof(unsigned int));

	// for extendible and linear hashing
	mUnusedNodes = NULL;
	mUnusedNodesCount = 0;
	mUnusedNodesSize = 0;
}

template <class TKey>
cPagedHashTableHeader<TKey>::~cPagedHashTableHeader()
{ 
}

template <class TKey>
inline void cPagedHashTableHeader<TKey>::SetMaxStringLength(unsigned int stringLength)
{
	mMaxStringLength = stringLength;
}

template <class TKey>
inline unsigned int cPagedHashTableHeader<TKey>::GetMaxStringLength() const
{
	return mMaxStringLength;
}

template <class TKey>
inline void cPagedHashTableHeader<TKey>::SetHashTableSize(unsigned int size)
{
	mHashTableSize = size;
}

template <class TKey>
inline unsigned int cPagedHashTableHeader<TKey>::GetHashTableSize() const
{
	return mHashTableSize;
}

template <class TKey>
bool cPagedHashTableHeader<TKey>::Write(cStream *stream)
{
	bool ok = cDStructHeader::Write(stream);
	
	ok &= stream->Write((char*)&mHashTableSize, sizeof(unsigned int));
	ok &= stream->Write((char*)&mMinStringLength, sizeof(unsigned int));
	ok &= stream->Write((char*)&mMaxStringLength, sizeof(unsigned int));

	// for extendible and linear hashing
	ok &= stream->Write((char*) &mUnusedNodesCount, sizeof(tNodeIndex));
	ok &= stream->Write((char*) mUnusedNodes, mUnusedNodesCount * sizeof(tNodeIndex));
	delete[] mUnusedNodes;
	mUnusedNodes = NULL;

	return ok;
}

template <class TKey>
bool cPagedHashTableHeader<TKey>::Read(cStream *stream) 
{
	bool ok = cDStructHeader::Read(stream);

	ok &= stream->Read((char*)&mHashTableSize, sizeof(unsigned int));
	ok &= stream->Read((char*)&mMinStringLength, sizeof(unsigned int));
	ok &= stream->Read((char*)&mMaxStringLength, sizeof(unsigned int));

	// for extendible and linear hashing
	ok &= stream->Read((char*) &mUnusedNodesCount, sizeof(tNodeIndex));
	if ((mUnusedNodes != NULL) && (mUnusedNodesSize < mUnusedNodesCount))
	{
		delete[] mUnusedNodes;
		mUnusedNodes = NULL;
	}
	if (mUnusedNodes == NULL)
	{
		mUnusedNodesSize = C_UNUSED_NODES_INITIAL_SIZE;
		while (mUnusedNodesSize < mUnusedNodesCount)
			mUnusedNodesSize <<= 1;
		mUnusedNodes = new tNodeIndex[mUnusedNodesSize];
	}
	ok &= stream->Read((char*) mUnusedNodes, mUnusedNodesCount * sizeof(tNodeIndex));

	return ok;
}

template <class TKey>
inline void cPagedHashTableHeader<TKey>::Init(const char* dsName, cDTDescriptor* pSd, const unsigned int keySize,
	const unsigned int dataSize, bool varlenData)
{
	cDStructHeader::Init();
	SetName(dsName);

	mHashTableSize = -1;
	mMinStringLength = -1;
	mMaxStringLength = -1;
	AddHeaderSize(3 * sizeof(unsigned int) + sizeof(bool)); /* MK: ?3? */

	// for extendible and linear hashing
	mUnusedNodesCount = 0;
	mUnusedNodesSize = C_UNUSED_NODES_INITIAL_SIZE;
	mUnusedNodes = new tNodeIndex[mUnusedNodesSize];

	SetNodeHeaderCount(1);

	cPagedHashTableNodeHeader<TKey>* nodeHeader = new cPagedHashTableNodeHeader<TKey>(keySize, dataSize, 
		TKey::LengthType == cDataType::LENGTH_VARLEN, varlenData);
	nodeHeader->SetKeyDescriptor(pSd);

	SetNodeHeader(0, nodeHeader);
}

template<class TKey>
void cPagedHashTableHeader<TKey>::HeaderSetup(/*unsigned int maxNodeInMemSize, */ unsigned int blockSize)
{
	cPagedHashTableNodeHeader<TKey>* nodeHeader = GetNodeHeader();
	GetNodeHeader()->SetNodeSerialSize(blockSize);
	GetNodeHeader()->SetNodeFanoutCapacity(0);
	GetNodeHeader()->SetNodeExtraItemCount(0);
	GetNodeHeader()->SetNodeExtraLinkCount(1);  // pointer to the next node in the string related to a bucket
	SetCacheMeasureTime(true);
	SetCacheMeasureCount(true);

	GetNodeHeader()->ComputeNodeCapacity(blockSize, true);  // suppose nodes as leafs - you need data to keys
}

template <class TKey>
inline unsigned int cPagedHashTableHeader<TKey>::GetNodeCount() const
{ 
	return ((cPagedHashTableNodeHeader<TKey>*)mNodeHeaders[0])->GetNodeCount();
}

template <class TKey>
inline unsigned int cPagedHashTableHeader<TKey>::GetItemCount() const
{ 
	return ((cPagedHashTableNodeHeader<TKey>*)mNodeHeaders[0])->GetItemCount();
}

template <class TKey>
inline cPagedHashTableNodeHeader<TKey>* cPagedHashTableHeader<TKey>::GetNodeHeader()
{ 
	return (cPagedHashTableNodeHeader<TKey>*)mNodeHeaders[0];
}

template <class TKey>
tNodeIndex cPagedHashTableHeader<TKey>::GetUnusedNode()
{
	if (mUnusedNodesCount > 0)
		return mUnusedNodes[--mUnusedNodesCount];

	return C_EMPTY_LINK;
}

template <class TKey>
void cPagedHashTableHeader<TKey>::SetUnusedNode(tNodeIndex nodeIndex)
{
	if (nodeIndex == C_EMPTY_LINK) return;

	if (mUnusedNodesCount >= mUnusedNodesSize)
	{
		tNodeIndex* newCache = new tNodeIndex[mUnusedNodesSize * 2];
		memcpy(newCache, mUnusedNodes, mUnusedNodesSize*sizeof(tNodeIndex));
		delete[] mUnusedNodes;
		mUnusedNodes = newCache;
		mUnusedNodesSize *= 2;
	}
	mUnusedNodes[mUnusedNodesCount++] = nodeIndex;
}

}}}
#endif