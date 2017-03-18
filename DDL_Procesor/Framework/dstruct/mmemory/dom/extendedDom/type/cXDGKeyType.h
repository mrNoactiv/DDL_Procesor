/**
*	\file cXDGKeyType.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
*	\brief Type for cXDGKey
*/


#ifndef __cXDGKeyType_h__
#define __cXDGKeyType_h__

#include "cDataType.h"
#include "dstruct/mmemory/dom/extendedDom/type/cXDGKey.h"	
#include "dstruct/mmemory/dom/extendedDom/type/cXDGKeySpaceDescriptor.h"	

/**
*	Type for cXDGKey
*
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
**/
class cXDGKeyType : public cBasicType<cXDGKey> 
{
public:
	static Type MAX;
	static const char CODE			 = 't';

	inline virtual char GetCode()									{return CODE; }
	inline virtual const char* GetStringFormat(int mode=0)			{ UNREFERENCED_PARAMETER(mode); return "%X"; }
	inline virtual void Print(const Type &it, int mode=0)			{ UNREFERENCED_PARAMETER(mode); it.Print(""); }

	inline static int GetSize(const Type& item)						{ return item.GetSerialSize(); }

	/// This method copy only pointers! Can lead to heap corruption during the delete!
	/// \see cTreeTuple::Copy
	inline static void CopyPointers(Type& to, const Type &from)		{ to.Copy(from); }
	inline static void Clear(Type& it)								{ it.Clear(); }
	inline static void CopyBlock(Type* to, const Type* from, unsigned int count, const cSizeInfo<cXDGKey> &sizeInfo);
	inline static void MoveBlock(Type* to, const Type* from, unsigned int count, const cSizeInfo<cXDGKey> &sizeInfo);	

	inline static int Compare(const Type &a, const Type &b)			{ return a.Equal(b); }

	inline static bool Write(cStream *out, const Type & it)			{ return it.Write(out); }
	inline static bool Read (cStream *inp, Type & it)				{ return it.Read(inp); }

	inline static char* ToString(char*str,const Type &it)			{ sprintf(str, "%d-%d", it.GetKey(), it.GetOrder()); return str; }
	inline static void Print(const char *str, const Type& it)		{ it.Print(str); }

	static void SetMax(cTreeSpaceDescriptor *sd);
	static void FreeMax();
};

void cXDGKeyType::CopyBlock(Type* to, const Type* from, unsigned int count, const cSizeInfo<cXDGKey> &sizeInfo)
{
	UNREFERENCED_PARAMETER(sizeInfo);
	for (unsigned int i = 0; i < count; i++)
	{
		to[i] = from[i];
	}
}

void cXDGKeyType::MoveBlock(Type* to, const Type* from, unsigned int count, const cSizeInfo<cXDGKey> &sizeInfo)
{
	UNREFERENCED_PARAMETER(sizeInfo);
	if (to < from)
	{
		for (int i = 0; i < (int)count; i++)
		{
			to[i] = from[i];
		}
	} else
	{
		for (int i = count - 1; i >= 0; i--)
		{
			to[i] = from[i];
		}
	}
}

#endif