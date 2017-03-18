/**
*	\file cXDGKeySpaceDescriptor.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
*	\brief Meta information about vector in cXDGKey
*/


#ifndef __cXDGKeySpaceDescriptor_h__
#define __cXDGKeySpaceDescriptor_h__

#include "cStream.h"
#include "cTreeSpaceDescriptor.h"

/**
*	Meta information about vector in cXDGKey. (DocId, LeftPos, RightPos)
*
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
**/
class cXDGKeySpaceDescriptor: public cTreeSpaceDescriptor
{
public:
	static const unsigned int DIMENSION = 2;

	cXDGKeySpaceDescriptor();
	cXDGKeySpaceDescriptor(const cXDGKeySpaceDescriptor &sd);
	~cXDGKeySpaceDescriptor();

	void Init();
	void Init(const cXDGKeySpaceDescriptor &sd);

	void Print(char *string) const;
};

#endif