/**
*	\file cArrayFactory.h
*	\author Radim Baca
*	\version 0.1
*	\date jun 2008
*	\brief Produce cArray<ItemType> objects.
*/


#ifndef __cArrayFactory_h__
#define __cArrayFactory_h__

#include "cArray.h"

/**
*	 Produce cArray<ItemType> objects. ItemType must be some elementar type.
*
*	\author Radim Baca
*	\version 0.1
*	\date jun 2008
**/
template<class ItemType>
class cArrayFactory
{
	unsigned int			mInitialAllocation;		/// Initial allocation of one array.
	unsigned int			mSize;					/// Maximal size of the array.
	unsigned int			mPosition;				/// Last unassigned array.
	cArray<ItemType>**		mArrays;				/// Set of arrays.
public:
	const static unsigned int DEFAULT_SIZE = 50;

	cArrayFactory(unsigned int initialAllocation, unsigned int size = DEFAULT_SIZE);
	~cArrayFactory();

	void Delete();
	void Init(unsigned int size);
	void Clear();
	void Resize(unsigned int size);

	/// \return Next unassigned array. Resize the array if necessary.
	inline cArray<ItemType>* GetNext() 
	{ 
		if (mPosition == mSize)
		{
			Resize(mSize + DEFAULT_SIZE);
		}
		return mArrays[mPosition++];
	}
};

/// Constructor.
/// \param initialAllocation Parameter specify inital size of each array created by this factory.
template<class ItemType>
cArrayFactory<ItemType>::cArrayFactory(unsigned int initialAllocation, unsigned int size)
	:mArrays(NULL), mInitialAllocation(initialAllocation), 
	mSize(0)
{
	Init(size);
}

/// Destructor
template<class ItemType>
cArrayFactory<ItemType>::~cArrayFactory()
{
	Delete();
}

/// Delete all objects created by this object.
template<class ItemType>
void cArrayFactory<ItemType>::Delete()
{
	if (mArrays != NULL)
	{
		for (unsigned int i = 0; i < mSize; i++)
		{
			delete mArrays[i];
		}
		delete[] mArrays;
	}
}

/// Create array of arrays
/// \param size Size of array
template<class ItemType>
void cArrayFactory<ItemType>::Init(unsigned int size)
{
	Delete();

	Resize(size);
	mPosition = 0;
}

/// Start assign from the first array again.
template<class ItemType>
void cArrayFactory<ItemType>::Clear()
{
	mPosition = 0;	
}

/// Increase the size of the array of arrays
/// \param size New size of array
template<class ItemType>
void cArrayFactory<ItemType>::Resize(unsigned int size)
{
	if (size > mSize)
	{
		cArray<ItemType>** mAuxArray = new cArray<ItemType>*[size];
		for (unsigned int i = 0; i < mSize; i++)
		{
			mAuxArray[i] = mArrays[i];
		}
		for (unsigned int i = mSize; i < size; i++)
		{
			mAuxArray[i] = new cArray<ItemType>();
			mAuxArray[i]->Resize(mInitialAllocation);
		}
		delete[] mArrays;
		mArrays = mAuxArray;
		mSize = size;
	}
}

#endif