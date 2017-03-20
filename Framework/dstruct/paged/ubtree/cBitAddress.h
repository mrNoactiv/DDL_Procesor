/**
 *	\file cBitAddress.h
 *	\author Michal Kratky, Radim Baca
 *	\version 0.1
 *	\date jun 2006
 *	\brief Homogenous tuple for a tree data structure. It contains an array of items of the same type.
 */

#ifndef __cBitAddress_h__
#define __cBitAddress_h__

#include <assert.h>
#include <stdio.h>

#include "common/datatype/cDataType.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cTuple.h"

using namespace common::compression;
using namespace common::datatype;
using namespace common::utils;

/**
* Represents n-dimensional tuple. Homogenous tuple for a tree data structure. It contains an array of items of the same type.
* Tuple does not contain the reference to the space descriptor, therefore, almost no asserts are contained in the tuple!
* Application has to do the asserts by itself!
*
* Written just for integer data types (int, short, char) since the methods like >, Greater, Equal will not return correct answers.
*
* \author Radim Baca, Michal Kratky
* \version 2.2
* \date feb 2011
**/

namespace common {
	namespace datatype {
		namespace tuple {

class cBitAddress
{
public:
	typedef cBitAddress T;

protected:
	char *mData;

public:
	static const unsigned int LengthType = cDataType::LENGTH_FIXLEN;
	static const char CODE = 'a';

public:
	cBitAddress();
	cBitAddress(const cSpaceDescriptor *pSd);
	~cBitAddress();
	cBitAddress(char* buffer);

	// static inline int GetInMemSize(const cSpaceDescriptor *pSd);

	void Free(cMemoryBlock *memBlock = NULL);
	/**********************************************************************************************/
	static void TupleToBitAddress(cSpaceDescriptor *pSd, const cTuple &t, char* adress);
	static void BitAddressToTuple(cSpaceDescriptor *pSd, char* add, cTuple &tuple);
	
	/*********************************************************************************************/
	//bool Resize(const cSpaceDescriptor *pSd, cMemoryBlock *memBlock = NULL);
	//bool Resize(const cDTDescriptor *pSd, uint length);
	//bool Resize(const cDTDescriptor *pSd);

	// inline char* Init(char* mem);
};
}}}

namespace common {
	namespace datatype {
		namespace tuple {

}}}
#endif
