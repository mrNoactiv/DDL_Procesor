/**
*	\file cTupleType.h
*	\author Radim Baca
*	\version 0.1
*	\date aug 2007
*	\brief Encapsulate \see cTuple and define type inherited from \see cBasicType. 
* It can be then used as type in tree nodes.
*/

#ifndef __cTupleType_h__
#define __cTupleType_h__

#include "common/datatype/cBasicType.h"
#include "common/datatype/tuple/cTuple.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cTupleSizeInfo.h"
#include "dstruct/mmemory/cMemoryBlock.h"

/**
*	\class cTupleType Encapsulate \see cTuple and define type inherited from \see cBasicType. 
* It can be then used as type in tree nodes.
*
*	\author Radim Baca
*	\version 0.1
*	\date aug 2007
**/

namespace common {
	namespace datatype {
		namespace tuple {

class cTupleType : public cBasicType<cTuple> 
{
public:
	static Type MAX;
	static const char CODE			 = 't';

	inline virtual char GetCode()									{ return CODE; }
	inline virtual const char* GetStringFormat(int mode=0)			{ UNREFERENCED_PARAMETER(mode); return "%X"; }
	inline virtual void Print(const Type &it, int mode=0)			{ UNREFERENCED_PARAMETER(mode); it.Print("\t"); }

	inline static int GetSize(const Type& item)						{ return item.GetSerialSize(); }
	inline static void Format(cSizeInfo<Type> &sizeInfo, cMemoryBlock* memory, Type& item)	{ item.Format(((cTupleSizeInfo&)sizeInfo).GetSpaceDescriptor(), memory); }

	inline static Type Average(const Type &a, const Type &b) 		{ printf("\n\ncTupleType::AVERAGE - it should not be called on this type!!\n\n"); UNREFERENCED_PARAMETER(b); return a; }
	/// This method copy only pointers! Can lead to heap corruption during the delete!
	/// \see cTuple::Copy
	inline static void CopyPointers(Type& to, const Type &from)		{ to.Copy(from); }
	inline static void Clear(Type& it)								{ it.Clear(); }

	inline static int Compare(const Type &a, const Type &b)			{ return a.Equal(b); }
	inline static bool IsZero(const Type &a)						{ return a.IsZero(); }

	inline static bool Write(cStream *out, const Type & it)			{ return it.Write(out); }
	inline static bool Read (cStream *inp, Type & it)				{ return it.Read(inp); }

	inline static void Print(const char *str, const Type& it)		{ it.Print(str); }

	static void SetMax(cSpaceDescriptor *sd);
	static void FreeMax();
};
}}}
#endif