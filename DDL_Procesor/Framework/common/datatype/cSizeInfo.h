/**
*	\file cSizeInfo.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2008
*	\brief Contains information needed for a successful resize and size estimation of an item.
*/

#include "common/cMemory.h"
//#include "dstruct\mmemory\cMemoryBlock.h"
#include "common/stream/cStream.h"

#ifndef __cSizeInfo_h__
#define __cSizeInfo_h__

/**
*	Contains information needed for a successful resize and size estimation of an item. The elementar types does not 
* any information, however, more complicated types need aditional info, which can be stored in this classess 
* which inherit from this one. Therefore, this class is intended for use with elementar types.
*
* Template has to be parameterized with the specific type (not the cBasicType class)
*
*	\author Radim Baca
*	\version 0.1
*	\date jun 2008
**/
template<class Type>
class cSizeInfo
{
public:
	static const char CODE			= 'a';

	cSizeInfo()	{}
	~cSizeInfo() {}

	inline virtual unsigned int GetInMemSize() const;
	inline virtual unsigned int GetSerialSize() const;

	inline virtual void Resize(Type& type)							{ UNREFERENCED_PARAMETER(type); }
	inline virtual void Resize(Type& type, cMemory* memory)			{ UNREFERENCED_PARAMETER(type); UNREFERENCED_PARAMETER(memory); }
	//inline virtual void Format(Type& type, cMemoryBlock* memory)	{ UNREFERENCED_PARAMETER(type); UNREFERENCED_PARAMETER(memory); }

	// methods for serialization of this class

	/// Write the type information into the stream. For simple type nothing is serialized
	inline virtual bool WriteSizeInfo(cStream *stream)				{ UNREFERENCED_PARAMETER(stream); return true; }
	/// Read the type information into the stream. For simple type nothing is serialized
	inline virtual bool ReadSizeInfo(cStream *stream)				{ UNREFERENCED_PARAMETER(stream); return true; }

	inline virtual char GetCode()									{ return CODE; }
};

/**
* \return The size of the type
*/
template<class Type>
inline unsigned int cSizeInfo<Type>::GetInMemSize() const
{ 
	return Type::GetInMemSize((char*)NULL); 
}

/**
* \return Serialized size of the size info object
*/
template<class Type>
inline unsigned int cSizeInfo<Type>::GetSerialSize() const
{ 
	return Type::GetStaticSerialSize(); 
}
#endif