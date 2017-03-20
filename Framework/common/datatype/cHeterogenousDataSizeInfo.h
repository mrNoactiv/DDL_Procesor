/**
 * \brief It contains information needed for a successful resize and size estimation of a 
 *   \see cVarLengthTermItem instance.
 *
 * \file cHeterogenousDataSizeInfo.h
 * \author Michal Kratky
 * \version 0.1
 * \date jul 2009
 */

#ifndef __cHeterogenousDataSizeInfo_h__
#define __cHeterogenousDataSizeInfo_h__

#include "cSizeInfo.h"
#include "cHeterogenousData.h"
#include "cMemory.h"

namespace common {
	namespace datatype {
class cHeterogenousDataSizeInfo : public cSizeInfo<cHeterogenousData>
{
public:
	static const char CODE = 'h';

	cHeterogenousDataSizeInfo();
	~cHeterogenousDataSizeInfo();

	inline virtual int GetSize() const;
	inline virtual void Resize(cHeterogenousData& type);
	inline virtual void Resize(cHeterogenousData& type, cMemory* memory);

	// methods for serialization of this class
	inline virtual bool WriteSizeInfo(cStream *stream);
	inline virtual bool ReadSizeInfo(cStream *stream);
	inline virtual unsigned int GetSerializeSize();
	inline virtual char GetCode();
};

/**
 * Returns the size of the serialize object
 */
inline int cHeterogenousDataSizeInfo::GetSize() const
{
	return sizeof(char) + cHeterogenousData::VALUE_LENGTH;
}

/**
 * Resize the item using the heap.
 */
inline void cHeterogenousDataSizeInfo::Resize(cHeterogenousData& type)
{ 
	type.Resize(); 
}

/**
 * Resize the item using the memory object.
 */
inline void cHeterogenousDataSizeInfo::Resize(cHeterogenousData& type, cMemory* memory)
{ 
	type.Resize(memory); 
}

/**
 * Write the type information into the stream. For this simple type (the size is constant) nothing is serialized.
 */
inline bool cHeterogenousDataSizeInfo::WriteSizeInfo(cStream *stream)
{ 
	return true;
}

/**
 * Read the type information into the stream. For this simple type (the size is constant) nothing is serialized.
 */
inline bool cHeterogenousDataSizeInfo::ReadSizeInfo(cStream *stream)						
{ 
	return true;
}

/**
 * Serialized size of the size info object
 */
inline unsigned int cHeterogenousDataSizeInfo::GetSerializeSize()
{
	return 0;
}

inline char cHeterogenousDataSizeInfo::GetCode()
{ 
	return CODE; 
}
}}
#endif