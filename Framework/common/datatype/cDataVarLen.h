/**
 *	\file cDataVarlen.h
 *	\author Radim Baca
 *	\version 0.1
 *	\date oct 2011
 *	\brief Contains static methods for a manipulation with a variable length data.
 */

#ifndef __cDataVarlen_h__
#define __cDataVarlen_h__

#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/stream/cStream.h"
#include "common/cString.h"

/**
* Contains static methods for a manipulation with a variable length data which has 
* the first byte reserved for the information about the data size.
*
*
* \author Radim Baca
* \version 2.2
* \date oct 2011
**/
namespace common {
	namespace datatype {

class cDataVarlen
{
public:
	static inline unsigned int Copy(char* dest, const char* src);
	static inline void Copy(char* dest, const char* raw_src, unsigned int len);
	static inline unsigned int GetSize(const char *data);
	static inline bool Compare(const char* data1, const char* data2, unsigned int size);
	static inline void Print(const char *data, const char* delim);

};

/**
* \return Size of the block that was copied from src into dest.
*/
unsigned int cDataVarlen::Copy(char* dest, const char* src)
{
	memcpy(dest, src, GetSize(src));
	return GetSize(src);
}

/**
* \param raw_src Source data which does not contain information about its length
* \param len Length (in bytes) of the block that should be copied from the raw_src.
*/
void cDataVarlen::Copy(char* dest, const char* raw_src, unsigned int bytelen)
{
	assert(bytelen < 255);
	*dest = (char)(bytelen + 1);
	memcpy(dest + 1, raw_src, bytelen);
}

/**
* \return The in memory size occupied by the data (including the first byte reserved for the size itself)
*/
unsigned int cDataVarlen::GetSize(const char *data)
{
	//assert((unsigned int)*data < 1024);
	return (unsigned int)*((const unsigned char*)data);
}

/**
* Byte comparison between data using the memcmp function.
* \param data1
* \param data2
* \param size Specifies the size of block which should be compared.
* \return true if the data blocks have the same value.
*/
bool cDataVarlen::Compare(const char* data1, const char* data2, unsigned int size)
{
	return memcmp(data1, data2, size) == 0;
}

void cDataVarlen::Print(const char *data, const char* delim)
{
	for (unsigned int i = 1; i < GetSize(data); i++)
	{
		printf("%c", data[i]);
	}
	printf("%s", delim);
}


}}
#endif