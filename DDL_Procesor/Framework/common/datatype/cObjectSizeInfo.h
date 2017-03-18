/**
*	\file cObjectSizeInfo.h
*	\author Radim Baca
*	\version 0.1
*	\date apr 2008
*	\brief Contains information needed for a successful resize and size estimation of an item.
*/

#include "cMemory.h"
#include "cStream.h"
#include "cSizeInfo.h"

#ifndef __cObjectSizeInfo_h__
#define __cObjectSizeInfo_h__


/**
*	Contains information needed for a successful resize and size estimation of an item. 
* This class is an extension of a cSizeInfo for classes.
*
*	Type - must be a class with a Resize method.
*
*
*	\author Radim Baca
*	\version 0.1
*	\date apr 2008
**/
template<class Type>
class cObjectSizeInfo: public cSizeInfo<Type>
{

public:
	static const char CODE			= 'o';

	cObjectSizeInfo()	{}
	~cObjectSizeInfo() {}

	/// \return The size of the type
	inline virtual void Resize(Type& type)							{ type.Resize(); }
	inline virtual void Resize(Type& type, cMemory* memory)			{ type.Resize(memory); }

	// methods for serialization of this class

	/// Write the type information into the stream. For simple type nothing is serialized
	inline virtual bool WriteSizeInfo(cStream *stream)				{ UNREFERENCED_PARAMETER(stream); return true; }
	/// Read the type information into the stream. For simple type nothing is serialized
	inline virtual bool ReadSizeInfo(cStream *stream)				{ UNREFERENCED_PARAMETER(stream); return true; }
	/// \return Serialized size of the size info object
	inline virtual unsigned int GetSerializeSize()					{ return 0; }

	inline virtual char GetCode()									{ return CODE; }
};

#endif