/**
*	\file cLeafItemSizeInfo.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2008
*	\brief Contains cTreeHeader needed for a successful resize and size estimation of an leaf item.
*/


#ifndef __cLeafItemSizeInfo_h__
#define __cLeafItemSizeInfo_h__

#include "cTreeHeader.h"
#include "cSizeInfo.h"
#include "cTreeTuple.h"

/**
*	Contains cTreeHeader needed for a successful resize and size estimation of an leaf item.
*
*	\author Radim Baca
*	\version 0.1
*	\date jun 2008
**/
template<class NodeItem>
class cLeafItemSizeInfo : public cSizeInfo<NodeItem>
{
	cTreeHeader *mHeader;
public:
	cLeafItemSizeInfo(cTreeHeader *header)					{ mHeader = header; }
	~cLeafItemSizeInfo()									{}

	inline cTreeHeader *GetTreeHeader()						{ return mHeader; }
	inline void SetTreeHeader(cTreeHeader* header)			{ mHeader = header; }

	inline virtual int GetSize() const						{ return mHeader->GetLeafNodeItemSize(); }
	inline virtual void Resize(NodeItem& type)				{ type.Resize(mHeader); }
};
#endif