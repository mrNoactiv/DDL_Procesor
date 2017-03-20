/**
 * \brief A type for the DateTime class
 *
 * \file cDateTimeType.h
 * \author Michal Kratky
 * \version 0.1
 * \date jul 2009
 */

#ifndef __cDateTimeType_h__
#define __cDateTimeType_h__

#include "cBasicType.h"
#include "cDateTime.h"

namespace common {
	namespace datatype {
class cDateTimeType: public cBasicType<cDateTime>
{
public:
	static const char CODE			= 't';
	static const char SER_SIZE		= sizeof(tDateTime);

public:
	cDateTimeType();
	~cDateTimeType();

	inline static int GetSerSize();

	static unsigned int Decode(Type* item, Type* referenceItem, char *memory, unsigned int mem_size);
	static unsigned int Encode(const Type* item, Type* referenceItem, char *memory, unsigned int mem_size);
};

/**
 * Return serilized size of the instance.
 */
int cDateTimeType::GetSerSize()
{
	return sizeof(tDateTime);
}
}}
#endif