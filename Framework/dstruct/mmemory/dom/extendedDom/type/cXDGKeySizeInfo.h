/**
*	\file cXDGKeySizeInfo.h
*	\author Radim Baca
*	\version 0.1
*	\date feb 2008
*	\brief Contains information needed for a successful resize and size estimation of an cXDGKey item.
*/


#ifndef __cXDGKeySizeInfo_h__
#define __cXDGKeySizeInfo_h__

#include "dstruct/mmemory/dom/extendedDom/type/cXDGKeySpaceDescriptor.h"
#include "cSizeInfo.h"
#include "dstruct/mmemory/dom/extendedDom/type/cXDGKey.h"

/**
*	Contains cXDGKeySpaceDescriptor needed for a successful resize and size estimation of an cXDGKey item.
*
*	\author Radim Baca
*	\version 0.1
*	\date feb 2008
**/
class cXDGKeySizeInfo : public cSizeInfo<cXDGKey>
{
	cXDGKeySpaceDescriptor *mTreeSpaceDescriptor;
public:
	cXDGKeySizeInfo(cXDGKeySpaceDescriptor *spaceDescriptor)				{ mTreeSpaceDescriptor = spaceDescriptor; }
	~cXDGKeySizeInfo()														{}

	inline cXDGKeySpaceDescriptor *GetSpaceDescriptor()						{ return mTreeSpaceDescriptor; }
	inline void SetSpaceDescriptor(cXDGKeySpaceDescriptor* spaceDescriptor)	{ mTreeSpaceDescriptor = spaceDescriptor; }

	inline virtual int GetSize() const										{ return mTreeSpaceDescriptor->GetByteSize(); }
	inline virtual void Resize(cXDGKey& type)								{ type.Resize(mTreeSpaceDescriptor); }
	inline virtual void Resize(cXDGKey& type, cMemory* memory)				{ type.Resize(mTreeSpaceDescriptor, memory); }

};
#endif