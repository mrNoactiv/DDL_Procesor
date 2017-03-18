/**
*	\file cDeweyLeafSizeInfo.h
*	\author Radim Baca
*	\version 0.1
*	\date feb 2008
*	\brief Contains information needed for a successful resize and size estimation of an cDeweyLeaf item.
*/


#ifndef __cDeweyLeafSizeInfo_h__
#define __cDeweyLeafSizeInfo_h__

#include "cDeweyLeafSpaceDescriptor.h"
#include "cSizeInfo.h"
#include "cDeweyLeaf.h"

/**
*	Contains cDeweyLeafSpaceDescriptor needed for a successful resize and size estimation of an cDeweyLeaf item.
*
*	\author Radim Baca
*	\version 0.1
*	\date feb 2008
**/
class cDeweyLeafSizeInfo : public cSizeInfo<cDeweyLeaf>
{
	cDeweyLeafSpaceDescriptor *mTreeSpaceDescriptor;
public:
	cDeweyLeafSizeInfo(cDeweyLeafSpaceDescriptor *spaceDescriptor)				{ mTreeSpaceDescriptor = spaceDescriptor; }
	~cDeweyLeafSizeInfo()														{}

	inline cDeweyLeafSpaceDescriptor *GetSpaceDescriptor()						{ return mTreeSpaceDescriptor; }
	inline void SetSpaceDescriptor(cDeweyLeafSpaceDescriptor* spaceDescriptor)	{ mTreeSpaceDescriptor = spaceDescriptor; }

	inline virtual int GetSize() const											{ return mTreeSpaceDescriptor->GetByteSize(); }
	inline virtual void Resize(cDeweyLeaf& type)								{ type.Resize(mTreeSpaceDescriptor); }

};
#endif