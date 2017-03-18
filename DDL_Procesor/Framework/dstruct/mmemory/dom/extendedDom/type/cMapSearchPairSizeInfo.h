/**
*	\file cMapSearchPairSizeInfo.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
*	\brief Implement resize of an cMapSearchPair item.
*/


#ifndef __cMapSearchPairSizeInfo_h__
#define __cMapSearchPairSizeInfo_h__

#include "cSizeInfo.h"
#include "dstruct/mmemory/dom/extendedDom/type/cMapSearchPair.h"

/**
*	Implement resize of an cMapSearchPair item.
*
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
**/
class cMapSearchPairSizeInfo : public cSizeInfo<cMapSearchPair>
{
public:
	cMapSearchPairSizeInfo()														{}
	~cMapSearchPairSizeInfo()														{}

	inline virtual void Resize(cMapSearchPair& type, cMemory* memory)				
	{ 
		type.Clear();
	}

	inline virtual void Resize(cMapSearchPair& type)								
	{ 
		type.Clear();
	}

};
#endif