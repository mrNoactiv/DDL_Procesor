/**
*	\file cDomStackItem.h
*	\author Radim Baca
*	\version 0.1
*	\date dec 2008
*	\brief Represents one item on the DOM tree stack. Used during the cDomTree traversal.
*/

#ifndef __cDomStackItem_h__
#define __cDomStackItem_h__

#include "cStream.h"
#include "cTreeSpaceDescriptor.h"
#include "cTreeTuple.h"

/**
* Represents one item on the DOM tree stack. Used during the cDomTree traversal.
* Extend the cTreeTuple with a dimension semantic.
* Consists from level, node index, right node index, node order, and right node order.
*
*	\author Radim Baca
*	\version 0.1
*	\date dec 2008
**/
class cDomStackItem: public cTreeTuple
{
	static const unsigned int ITEM_SIZE = 3;

	static const unsigned int LEVEL = 0;
	static const unsigned int NODE_INDEX = 1;
	static const unsigned int NODE_ORDER_IN_PARENT = 2;
	static const unsigned int RIGHT_NODE_INDEX = 3;
public:
	cDomStackItem(cTreeSpaceDescriptor* descriptor);

	inline void operator = (const cTreeTuple& tuple)	{ cTreeTuple::operator =(tuple); }

	inline void SetLevel(unsigned char level)			{ SetValue(LEVEL, (char)level); }
	inline unsigned char GetLevel() const				{ return GetUChar(LEVEL); }
	inline void SetIndex(unsigned int index)			{ SetValue(NODE_INDEX, index); }
	inline unsigned int GetIndex() const				{ return GetUInt(NODE_INDEX); }
	inline void SetRightIndex(unsigned int index)		{ SetValue(RIGHT_NODE_INDEX, index); }
	inline unsigned int GetRightIndex() const			{ return GetUInt(RIGHT_NODE_INDEX); }
	inline void SetNodeOrder(unsigned int order)		{ SetValue(NODE_ORDER_IN_PARENT, order); }
	inline unsigned int GetNodeOrder() const			{ return GetUInt(NODE_ORDER_IN_PARENT); }
};

#endif