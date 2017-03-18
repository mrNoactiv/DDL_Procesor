/**
*	\file cSequentialArrayContext.h
*	\author Radim Baca
*	\version 0.1
*	\date jul 2011
*	\brief Context information during the work with persistent array having variable length items.
*/


#ifndef __cSequentialArrayContext_h__
#define __cSequentialArrayContext_h__

#include "dstruct/paged/sequentialarray/cSequentialArrayHeader.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayNode.h"
#include "dstruct/paged/core/cNode.h"
#include "dstruct/paged/core/cDataStructureContext.h"

using namespace dstruct::paged::core;

namespace dstruct {
	namespace paged {
		namespace sqarray {

/**
* Context information used during the work with sequential array.
* This context does not have its own memory, it always point directly to the node.
*
* Template parameters:
*	- TItemType - Type of the item which is stored in the persistent array.
*
*	\author Radim Baca
*	\version 0.1
*	\date jul 2011
**/
template<class TItemType>
class cSequentialArrayContext: public cDataStructureContext
{
	cNode*			mNode;
	char*			mItem;				/// Actual item read from the array.
	unsigned int	mStartPosition;		/// Where the mItem start in the actual node (mNode).
	unsigned int	mPosition;	
	bool			mReadFlag;
public:

	cSequentialArrayContext();
	~cSequentialArrayContext();

	void Null();
	void Init();
	void Delete();

	void operator = (cSequentialArrayContext<TItemType>& context);

	inline void SetItem(char* item)					{ mItem = item; }
	inline void SetPosition(unsigned int position);
	inline void SetNode(cNode* node)		{ mNode = node; }
	inline unsigned int IncPosition()				{ return ++mPosition; }
	inline void SetReadFlag(bool readFlag)			{ mReadFlag = readFlag; }

	inline cNode* GetNode() const	{ return mNode; }
	inline cNode* GetRefNode()	{ return mNode; }
	inline unsigned int GetPosition() const			{ return mPosition; };
	inline char* GetItem()							{ return mItem; }
	inline const char* GetCItem() const				{ return mItem; }
	inline unsigned int GetStartPosition() const	{ return mStartPosition; }
	inline bool GetReadFlag() const					{ return mReadFlag; }
};

template<class TItemType>
cSequentialArrayContext<TItemType>::cSequentialArrayContext()
{
	Null();
	Init();
}

template<class TItemType>
cSequentialArrayContext<TItemType>::~cSequentialArrayContext()
{
	Delete();
}

template<class TItemType>
void cSequentialArrayContext<TItemType>::Null()
{
	mItem = NULL;
}

template<class TItemType>
void cSequentialArrayContext<TItemType>::Init()
{
	mPosition = 0;
}

template<class TItemType>
void cSequentialArrayContext<TItemType>::Delete()
{
	//if (mItem != NULL)
	//{
	//	delete mItem;
	//	mItem = NULL;
	//}
}

/**
* Copy the context into this context.
* Increase read locks on the node, therefore, new context has to be closed after it is useless.
*/
template<class TItemType>
void cSequentialArrayContext<TItemType>
	::operator = (cSequentialArrayContext<TItemType>& context)
{
	mNode = context.GetRefNode();
	mPosition = context.GetPosition();
	*mItem = *context.GetItem();
	mStartPosition = context.GetStartPosition();
}

/**
* Set actual position of the context node. We also save the actual position, because we
* sometimes need the start position of the actual mItem (i.e., during the update).
* \param position New position of the cursor.
*/
template<class TItemType>
void cSequentialArrayContext<TItemType>::SetPosition(unsigned int position)
{
	mStartPosition = mPosition;
	mPosition = position;
}

}}}
#endif