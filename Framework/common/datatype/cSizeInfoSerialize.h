/**
*	\file cSizeInfoSerialize.h
*	\author Radim Baca
*	\version 0.1
*	\date feb 2008
*	\brief Serialize and deserialize the cSizeInfo object.
*/

#include "cStream.h"

#include "cSizeInfo.h"
#include "cTupleSizeInfo.h"
#include "cNTupleSizeInfo.h"
#include "cBasicType.h"
#include "cTreeTuple.h"
#include "cNTreeTuple.h"

#ifndef __cSizeInfoSerialize_h__
#define __cSizeInfoSerialize_h__

/**
*	Serialize and deserialize the cSizeInfo object. 
* Each type (CODE) of the cSizeInfo object should be implemented in read function.
* Bad on this class is that the project which use it must know all classes included here. 
* This could be solved by another template parameter - precise type of the sizeInfo, 
* but we have to know the precise type which should be readed created. If we would know
* this class would lost its sence.
*
* Parameter type is the type which will be used in the cSizeInfo template (it is not derived from cBasicType).
*
*	\author Radim Baca
*	\version 0.1
*	\date feb 2008
**/
template<class Type>
class cSizeInfoSerialize
{

public:
	static bool Write(cStream *stream, cSizeInfo<Type>* sizeInfo);
	static bool Read(cStream *stream, cSizeInfo<Type>** sizeInfo);

};

/// Method write the sizeInfo into the stream
/// \param stream Stream into which the data are written
/// \param sizeInfo Serialized sizeInfo object
/// \return
///		- true if the object was successfully serialized
///		- false otherwise
template<class Type>
bool cSizeInfoSerialize<Type>::Write(cStream *stream, cSizeInfo<Type>* sizeInfo)
{
	bool ret = true;
	char code = sizeInfo->GetCode();

	ret &= stream->Write(&code, sizeof(char));
	ret &= sizeInfo->WriteSizeInfo(stream);

	return ret;
}

/// Method read the sizeInfo from the stream
/// \param stream Stream from which the data are readed
/// \param sizeInfo New cSizeInfo object is created and stored in this parameter
/// \return
///		- true if the object was successfully deserialized
///		- false otherwise
template<class Type>
bool cSizeInfoSerialize<Type>::Read(cStream *stream, cSizeInfo<Type>** sizeInfo)
{
	bool ret = true;
	char code;

	ret &= stream->Read(&code, sizeof(char));

	// the main part of this class (the main reason why the class was created)
	switch(code)
	{
	case cSizeInfo<Type>::CODE:
		*sizeInfo = new cSizeInfo<Type>();
		break;
	case cTupleSizeInfo::CODE:
		*sizeInfo = (cSizeInfo<Type>*)(new cTupleSizeInfo(new cTreeSpaceDescriptor()));
		break;
	case cNTupleSizeInfo::CODE:
		*sizeInfo = (cSizeInfo<Type>*)(new cNTupleSizeInfo(new cNTreeSpaceDescriptor()));
		break;
	default:
		printf("cSizeInfoSerialize<Type>::Read - size info is not in correct format!\n");
	}
	ret &= (*sizeInfo)->ReadSizeInfo(stream);
	return ret;
}

#endif