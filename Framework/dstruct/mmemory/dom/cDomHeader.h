/**
*	\file cDomHeader.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2008
*	\brief Header of the cDomTree tree 
*/


#ifndef __cDomHeader_h__
#define __cDomHeader_h__

#include "common/cMemory.h"
#include "common/datatype/cSizeInfo.h"

/**
*  Header of the cDomTree tree.
* Template parameters:
*	- TKeyItem - Have to be inherited from cBasicType. Represent type of the key value.
*	- TLeafItem - Have to be inherited from cBasicType. Represent type of the leaf value. Must implement Inc() method, which increment item of this type.
*
*
*	\author Radim Baca
*	\version 0.1
*	\date dec 2008
**/
template<class TKeyItem, class TLeafItem>
class cDomHeader
{
	typedef typename TKeyItem::Type KeyType;
	typedef typename TLeafItem::Type LeafType;

	cMemory*				mMemory;
	cSizeInfo<KeyType>*		mKeySizeInfo;
	cSizeInfo<LeafType>*	mLeafSizeInfo;			
	unsigned int			mRootIndex;				/// Node index of the root node.

	unsigned int mInnerNodeArraySize;				/// Size of the cDomTree::mNodeCache_Inner.
	unsigned int mTopInnerNodeIndex;				/// Last inner node index.
	unsigned int mMaximalAllocatedInnerNodeIndex;	/// Maximal inner node index, which has been already allocated.
	unsigned int mLeafNodeArraySize;				/// Size of the cDomTree::mNodeCache_leaf.
	unsigned int mTopLeafNodeIndex;					/// Last leaf node index.
	unsigned int mMaximalAllocatedLeafNodeIndex;	/// Maximal leaf node index, which has been already allocated.
	
	LeafType mTopLeafNumber;						/// Maximal leaf number.
	LeafType mStartLeafNumber;						/// Initial leaf number.

	unsigned int mInnerNodeCapacity;				/// Capacity of a inner node in a sub-tree.
	unsigned int mLeafNodeCapacity;					/// Capacity of a leaf node in a sub-tree.

	// Do not change this attribute in any circumstances. Use the constructor parameter instead.
	static const unsigned int DEFAULT_CAPACITY = 10; /// Default capacity of a inner node and leaf node.
	static const unsigned int DEFAULT_MEMORY_CAPACITY = 102400; /// Default capacity of a inner node and leaf node.
public:
	cDomHeader(cSizeInfo<KeyType>* keySizeInfo, cSizeInfo<LeafType> *LPSizeInfo, LeafType startLeafNumber, unsigned int capacity = DEFAULT_CAPACITY, unsigned int memoryBlockCapacity = DEFAULT_MEMORY_CAPACITY);
	~cDomHeader();

	void Delete();
	void Init( LeafType startLeafNumber, unsigned int capacity, unsigned int memoryBlockCapacity);
	void Clear();

	inline cMemory* GetMemory()									{ return mMemory; }
	inline const cSizeInfo<KeyType>& GetKeySizeInfo() const		{ return *mKeySizeInfo; }
	inline const cSizeInfo<LeafType>& GetLeafSizeInfo() const	{ return *mLeafSizeInfo; }

	inline unsigned int GetNextInnerNodeIndex();
	inline unsigned int GetNextLeafNodeIndex();
	inline unsigned int GetInnerArraySize() const				{ return mInnerNodeArraySize; }
	inline unsigned int GetLeafArraySize() const				{ return mLeafNodeArraySize; }
	inline unsigned int GetTopInnerNodeIndex() const			{ return mTopInnerNodeIndex; }
	inline unsigned int GetTopLeafNodeIndex() const				{ return mTopLeafNodeIndex; }
	inline unsigned int GetMaximalAllocatedInnerNodeIndex()		{ return mMaximalAllocatedInnerNodeIndex; }
	inline unsigned int GetMaximalAllocatedLeafNodeIndex()		{ return mMaximalAllocatedLeafNodeIndex; }
	inline unsigned int GetRootIndex() const					{ return mRootIndex; }
	inline LeafType& GetNextLeaf()								{ TLeafItem::Inc(mTopLeafNumber); return mTopLeafNumber; }
	inline LeafType& GetTopLeaf()								{ return mTopLeafNumber; }
	inline unsigned int GetInnerNodeCapacity() const			{ return mInnerNodeCapacity; }
	inline unsigned int GetLeafNodeCapacity() const				{ return mLeafNodeCapacity; }

	inline void SetRootIndex(unsigned int rootIndex)			{ mRootIndex = rootIndex; }
	inline void SetLeafNumber(const LeafType& leafNumber)		{ TLeafItem::Copy(mTopLeafNumber, lpNumber); }
	inline void SetInnerArraySize(unsigned int innerSize)		{ mInnerNodeArraySize = innerSize; }
	inline void SetLeafArraySize(unsigned int leafSize)			{ mLeafNodeArraySize = leafSize; }
	inline void SetPointerNodeCapacity(unsigned int capacity)	{ mInnerNodeCapacity = capacity; }
	inline void SetInnerNodeCapacity(unsigned int capacity)		{ mLeafNodeCapacity = capacity; }
	inline void DecrementTopLeafNodeIndex()						{ assert(mTopLeafNodeIndex  > 0); mTopLeafNodeIndex -= 1; }
	inline void DecrementTopInnerNodeIndex()					
	{ 
		assert(mTopInnerNodeIndex  > 0); 
		mTopInnerNodeIndex -= 1; 
	}
};

/**
* Constructor
* \param keySizeInfo Size info related to the TKeyItem.
* \param leafSizeInfo Size info related to the TLeafItem.
* \param startLeafNumber Starting labelled path number.
* \param capacity Capacity of all nodes. Each node type capacity can be than changed after the header creation.
* \param memoryBlockCapacity capacity of one block allocated by underlying cMemory object. Specify the allocation granularity of the DOM object.
*/
template<class TKeyItem, class TLeafItem>
cDomHeader<TKeyItem, TLeafItem>
	::cDomHeader(
		cSizeInfo<KeyType>* keySizeInfo, 
		cSizeInfo<LeafType> *leafSizeInfo, 
		LeafType startLeafNumber, 
		unsigned int capacity, 
		unsigned int memoryBlockCapacity)
	:mKeySizeInfo(keySizeInfo), mLeafSizeInfo(leafSizeInfo),
	mMemory(NULL)
{
	Init(startLeafNumber, capacity, memoryBlockCapacity);
}

/**
* Destructor
*/
template<class TKeyItem, class TLeafItem>
cDomHeader<TKeyItem, TLeafItem>::~cDomHeader()
{
	Delete();
}

template<class TKeyItem, class TLeafItem>
void cDomHeader<TKeyItem, TLeafItem>::Delete()
{
	if (mMemory != NULL)
	{
		delete mMemory;
		mMemory = NULL;
	}
}

/**
* Initialize variables and allocate cMemory object.
* \param startLeafNumber Starting labelled path number.
* \param capacity Capacity of all nodes. Each node type capacity can be than changed after the header creation.
* \param memoryBlockCapacity capacity of one block allocated by underlying cMemory object. Specify the allocation granularity of the DOM object.
*/
template<class TKeyItem, class TLeafItem>
void cDomHeader<TKeyItem, TLeafItem>
	::Init( LeafType startLeafNumber, unsigned int capacity, unsigned int memoryBlockCapacity)
{
	Delete();

	mMemory = new cMemory(memoryBlockCapacity);

	mInnerNodeCapacity = capacity;
	mLeafNodeCapacity = capacity;
	mRootIndex = (unsigned int)-1;

	mTopInnerNodeIndex = (unsigned int)-1;
	mTopLeafNodeIndex = (unsigned int)-1;
	mMaximalAllocatedInnerNodeIndex = (unsigned int)-1;
	mMaximalAllocatedLeafNodeIndex = (unsigned int)-1;

	TLeafItem::Copy(mStartLeafNumber, startLeafNumber);
	TLeafItem::Copy(mTopLeafNumber, startLeafNumber);
}

template<class TKeyItem, class TLeafItem>
void cDomHeader<TKeyItem, TLeafItem>::Clear()
{
	mRootIndex = (unsigned int)-1;
	mTopInnerNodeIndex = (unsigned int)-1;
	mTopLeafNodeIndex = (unsigned int)-1;
	TLeafItem::Copy(mTopLeafNumber, mStartLeafNumber);
}

template<class TKeyItem, class TLeafItem>
unsigned int cDomHeader<TKeyItem, TLeafItem>::GetNextInnerNodeIndex()
{ 
	if (++mTopInnerNodeIndex > mMaximalAllocatedInnerNodeIndex || mMaximalAllocatedInnerNodeIndex == (unsigned int)-1)
	{
		mMaximalAllocatedInnerNodeIndex = mTopInnerNodeIndex; 
	}
	return mTopInnerNodeIndex; 
}

template<class TKeyItem, class TLeafItem>
unsigned int cDomHeader<TKeyItem, TLeafItem>::GetNextLeafNodeIndex()
{ 
	if (++mTopLeafNodeIndex > mMaximalAllocatedLeafNodeIndex || mMaximalAllocatedLeafNodeIndex == (unsigned int)-1)
	{
		mMaximalAllocatedLeafNodeIndex = mTopLeafNodeIndex; 
	}
	return mTopLeafNodeIndex; 
}

#endif