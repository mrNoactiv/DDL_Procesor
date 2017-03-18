/**
*	\file cDataType.h
*	\author Michal Kratky
*	\version 0.1
*	\date jun 2003
*	\brief Abstraction of a data type
*/

#ifndef __cDataType_h__
#define __cDataType_h__

#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "common/datatype/cDTDescriptor.h"

using namespace common::datatype;

/**
*	Abstraction of a data type
*
*	\author Michal Kratky
*	\version 0.1
*	\date jun 2006
**/
class cDataType
{
public:
	static const char SER_SIZE					= 0;
	static const unsigned int CHAR_BIT_LENGTH 	= 8;
	static const char CODE						= 'x';

	static const unsigned int LENGTH_VARLEN = 0;
	static const unsigned int LENGTH_FIXLEN = 1;

	static const unsigned int NOT_DEFINED = UINT_MAX;
	
	static const char CODE_NTUPLE = 'n';
	static const char CODE_LNTUPLE = 'l';

	cDataType(void);
	~cDataType(void);
	
	inline virtual char GetCode()							{ return CODE; }
	inline virtual int CompareArray(const char* array1, const char* array2, unsigned int length) = 0;
	inline virtual unsigned int HashValue(const char *array, unsigned int length, unsigned int hashTableSize) = 0;
	inline virtual unsigned int GetSize_instance(const char* mem, const cDTDescriptor *dTd = NULL) const = 0;
	inline virtual unsigned int GetSize(const cDTDescriptor *dTd = NULL) const = 0;
	inline virtual unsigned int GetSize(unsigned int itemSize) const = 0;
	inline virtual unsigned int GetMaxSize(const cDTDescriptor *dTd) const = 0;
};
#endif
