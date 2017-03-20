/**
*	\file cTupleSizeInfo.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2008
*	\brief Contains information needed for a successful resize and size estimation of an tuple item.
*/


#ifndef __cTupleSizeInfo_h__
#define __cTupleSizeInfo_h__

#include "cSpaceDescriptor.h"
#include "cSizeInfo.h"
#include "cTuple.h"

/**
*	Contains cSpaceDescriptor needed for a successful resize and size estimation of an cTuple item.
*
*	\author Radim Baca
*	\version 0.1
*	\date jun 2008
**/
namespace common {
	namespace datatype {
		namespace tuple {

class cTupleSizeInfo : public cSizeInfo<cTuple>
{
	cSpaceDescriptor *mTreeSpaceDescriptor;
public:
	static const char CODE			= 'b';

	cTupleSizeInfo(cSpaceDescriptor *spaceDescriptor)					{ mTreeSpaceDescriptor = spaceDescriptor; }
	~cTupleSizeInfo()														{}

	inline cSpaceDescriptor *GetSpaceDescriptor()						{ return mTreeSpaceDescriptor; }
	inline void SetSpaceDescriptor(cSpaceDescriptor* spaceDescriptor)	{ mTreeSpaceDescriptor = spaceDescriptor; }

	inline virtual int GetSize() const										{ return mTreeSpaceDescriptor->GetByteSize(); }
	inline virtual void Resize(cTuple& type)							{ type.Resize(mTreeSpaceDescriptor); }
	inline virtual void Resize(cTuple& type, cMemory* memory)			{ type.Resize(mTreeSpaceDescriptor, memory); }
	inline virtual void Format(cTuple& type, cMemoryBlock* memory)		{ type.Format(mTreeSpaceDescriptor, memory); }

	// methods for serialization of this class

	/// Write the type information into the stream. For simple type nothing is serialized
	inline virtual bool WriteSizeInfo(cStream *stream)					{ return mTreeSpaceDescriptor->Write(stream); }
	/// Read the type information into the stream. For simple type nothing is serialized
	inline virtual bool ReadSizeInfo(cStream *stream)					{ return mTreeSpaceDescriptor->Read(stream); }
	/// \return Serialized size of the size info object
	inline virtual unsigned int GetSerializeSize()						{ return mTreeSpaceDescriptor->GetSerialSize(); }
	inline virtual char GetCode()										{ return CODE; }
};
}}}
#endif