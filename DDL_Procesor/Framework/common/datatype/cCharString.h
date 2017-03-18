#ifndef __cCharString_h__
#define __cCharString_h__

#include "common/datatype/cBasicType.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"

using namespace common::datatype::tuple;

class cCharString : public cBasicType<char*>
{
public:
	static const unsigned char MAX			= 255;
	static const unsigned char ZERO			= 0;
	static const char CODE			= 'r';

	cCharString(char* value) : cBasicType(value) {}
	cCharString() : cBasicType(NULL) {}
	
	inline virtual char GetCode()								{return CODE; }
	inline static bool IsZero(const Type &a)					{ return a.GetValue() == ZERO; }

	/*inline operator char*() const
	{
		return mValue;
	}*/

	static bool ResizeSet(char** str1, const char* str2, cDTDescriptor* pDtd, cMemoryBlock* memBlock)
	{
		cSpaceDescriptor *sd = (cSpaceDescriptor*)pDtd;
		*str1 = memBlock->GetMemory(sd->GetSize());

		if (*str1 != NULL)
		{
			memcpy(*str1, str2, sd->GetSize());
		}
		return *str1 != NULL;
	}

	static inline void SetValue(T& value1, const T& value2, const cDTDescriptor* pDtd)
	{
		cSpaceDescriptor *sd = (cSpaceDescriptor*)pDtd;
		memcpy(value1, value2, sd->GetSize());
	}
};

#endif