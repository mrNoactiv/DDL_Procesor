/**
*	\file cObjectType.h
*	\author Radim Baca
*	\version 0.1
*	\date may 2008
*	
*/


#ifndef __cObjectType_h__
#define __cObjectType_h__

#include "cBasicType.h"

/**
*	
*
*	\author Radim Baca
*	\version 0.1
*	\date may 2008
**/
template<class TItemType>
class cObjectType : public cBasicType<TItemType> 
{
public:
	static Type MAX;
	static const char CODE			 = 's';

	inline static bool Write(cStream *out, const Type & it)			{ return it.Write(out); }
	inline static bool Read (cStream *inp, Type & it)				{ return it.Read(inp); }

	inline static void Print(const char *str, const Type& it)		{ it.Print(str); }

};
#endif