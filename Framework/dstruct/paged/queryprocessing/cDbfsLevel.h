/*!
* \class cDbfsLevel
*
* \brief Level buffer used for traversing the tree in the Breadth First Search manner.
*
* \author Pavel Bednar
* \date 2015-02-05
*/

#ifndef __cDbfsLevel_h__
#define __cDbfsLevel_h__

#include "dstruct/paged/queryprocessing/cRangeQueryConfig.h"
using namespace common::memdatstruct;
using namespace dstruct::paged::core;
using namespace dstruct::paged;

namespace dstruct {
	namespace paged {

class cDbfsLevel
{
private:
	bool mIsArray;
	cArray<uint>* mArray;
	cHashTable<cUInt, cUInt>* mHash;
public:
	static inline uint GetSize(int structType, uint capacity, uint noLevels);
	inline void Init(int structType, char* buffer, uint capacity);
	inline uint Count();
	inline void Add(uint index);
	inline void ClearCount();
	inline void Sort();
	inline uint& GetRefItem(uint order) const;
	inline cArray<uint>* ToArray();
};
/*!
* Returns required memory size.
*/
uint cDbfsLevel::GetSize(int structType, uint capacity, uint noLevels)
{
	uint size;
	if (structType == cRangeQueryConfig::SEARCH_STRUCT_ARRAY)
	{
		return	sizeof(cArray<uint>*) * noLevels +
			noLevels * (sizeof(cArray<uint>) + capacity * sizeof(uint)) +
			capacity * sizeof(uint);
	}
	else
	{
		return cHashTable<cUInt, cUInt>::GetSize(capacity);
	}
}
/*!
* Initializes the level buffer.
*/
void cDbfsLevel::Init(int structType, char* buffer, uint capacity)
{
	mIsArray = structType == cRangeQueryConfig::SEARCH_STRUCT_ARRAY;
	if (mIsArray)
	{
		mArray = (cArray<uint>*)buffer;
		buffer += sizeof(cArray<uint>);
		mArray->Init(buffer,capacity);
		//buffer += capacity * sizeof(uint);
	}
	else
	{
		mHash = new (buffer)cHashTable<cUInt, cUInt>(capacity, buffer);
	}
}
/*!
* Returns number of elements in the level buffer
*/
uint cDbfsLevel::Count()
{
	return (mIsArray) ? mArray->Count() : mHash->GetItemCount();
}
void cDbfsLevel::Add(uint index)
{
	if (mIsArray)
	{
		mArray->Add(index);
	}
	else
	{
		mHash->Add(index, 0);
	}
}
/*!
* Clears the buffer.
*/
void cDbfsLevel::ClearCount()
{
	(mIsArray) ? mArray->ClearCount() : mHash->Clear();
}
/*!
* Returns reference to item on specified position.
*/
inline uint& cDbfsLevel::GetRefItem(unsigned int order) const
{
	uint idx = (mIsArray) ? mArray->GetRefItem(order) : mHash->GetNode(order)->Key;
	return idx;
}
/*!
* Sorts the level buffer (not working for hashtable mode).
*/
inline void cDbfsLevel::Sort()
{
	if (mIsArray) 
		mArray->BubbleSort();
}
/*!
* Returns the level buffer as array (now working for hashtable mode).
*/
inline cArray<uint>* cDbfsLevel::ToArray()
{
	assert(mIsArray); //until hash table has no iterator
	if (mIsArray)
	{
		return mArray;
	}
	else
	{
		//problem: where to alocate uint* based on hashtable items count.
	}
}
}}
#endif