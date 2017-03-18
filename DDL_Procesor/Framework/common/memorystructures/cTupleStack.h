/**
*	\file cTupleStack.h
*	\author R.Baca
*	\version 0.1
*	\date jun 2006 
*	\brief Tuple stack
*/

#ifndef __cTupleStack_h__
#define __cTupleStack_h__

#include "cStack.h"
#include "cTreeTuple.h"
#include "cTreeSpaceDescriptor.h"

/**
*	\class cTupleStack extend cStack for cTuple handling
*
*	\author R.Baca
*	\version 0.1
*	\date jun 2006
**/
class cTupleStack: public cStack<cTreeTuple>
{
	cTreeSpaceDescriptor *mSpaceDescriptor;

	inline void Resize();
public:

	cTupleStack(const int size = 100);
	cTupleStack(const cTreeSpaceDescriptor &sd, const int size = 100);
	~cTupleStack();

	void CreateSpaceDescriptor(unsigned int dimension, cDataType *type);
	void CreateSpaceDescriptor(const cTreeSpaceDescriptor &sd);
	inline cTreeSpaceDescriptor* GetSpaceDescriptor() const;
};

/// Get space descriptor of the tuples in stack
/// \returns Space descriptor class
inline cTreeSpaceDescriptor* cTupleStack::GetSpaceDescriptor() const
{
	return mSpaceDescriptor;
}

/// Resize tuples in stack
inline void cTupleStack::Resize()
{
	for (int i = 0; i < m_Size; i++)
	{
		m_Items[i].Resize(mSpaceDescriptor);
	}
}
#endif