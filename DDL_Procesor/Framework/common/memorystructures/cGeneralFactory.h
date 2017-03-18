/**
*	\file cGeneralFactory.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
*	\brief 
*/


#ifndef __cGeneralFactory_h__
#define __cGeneralFactory_h__

#include "cSizeInfo.h"
#include "cMemory.h"

/**
*	Interface for the factory class producing items of specified type. 
* Template must be parametrized by the class inherited from cBasicType.
*
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
**/
template<class TItemType>
class cGeneralFactory
{
protected:
	typedef typename TItemType::Type ItemType;

	cMemory*					mMemory;
	cSizeInfo<ItemType>*		mSizeInfo;

	inline void GetNext(ItemType** item);

	static const unsigned int DEFAULT_MEMORY_BLOCK_SIZE = 131072; // Default memory block size. Do not change in any circumstances, use the constructor parameter instead.
public:
	cGeneralFactory(cSizeInfo<ItemType>* sizeInfo, unsigned int memoryBlockSize = DEFAULT_MEMORY_BLOCK_SIZE);
	~cGeneralFactory();

	inline ItemType* GetNext() 
	{ 
		ItemType* item; 
		GetNext(&item);
		return item;
	}

	inline void Clear()							{ mMemory->Clear(); };
	inline void UndoLast()						{ printf("cGeneralFactory::UndoLast() - not implented yet!\n"); }
	inline unsigned int MemoryUsed()			{ mMemory->GetMemoryUsage(); }
	inline cSizeInfo<ItemType>* GetSizeInfo()	{ return mSizeInfo; }
};

template<class TItemType>
cGeneralFactory<TItemType>::cGeneralFactory(
	cSizeInfo<ItemType>* sizeInfo, 
	unsigned int memoryBlockSize = DEFAULT_MEMORY_BLOCK_SIZE)
	:mSizeInfo(sizeInfo), mMemory(NULL)
{
	mMemory = new cMemory(memoryBlockSize);
}

template<class TItemType>
cGeneralFactory<TItemType>::~cGeneralFactory()
{
	delete mMemory;
}

template<class TItemType>
void cGeneralFactory<TItemType>::GetNext(ItemType** item)
{
	*item = (ItemType*)mMemory->GetMemory(sizeof(ItemType));
	TItemType::Resize(*mSizeInfo, mMemory, **item);
}

#endif