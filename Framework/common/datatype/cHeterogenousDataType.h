/**
 * \brief It represents heterogenous data type (\see cHeterogenousData)
 *
 * \file cHeterogenousDataType.h
 * \author Michal Kratky
 * \version 0.1
 * \date jul 2009
 */

#ifndef __cHeterogenousDataType_h__
#define __cHeterogenousDataType_h__

#include "cHeterogenousData.h"
#include "cBasicType.h"

namespace common {
	namespace datatype {
class cHeterogenousDataType: public cBasicType<cHeterogenousData>
{
public:
	cHeterogenousDataType();
	~cHeterogenousDataType();

	static unsigned int Decode(Type* item, Type* referenceItem, char *memory, unsigned int mem_size);
	static unsigned int Encode(const Type* item, Type* referenceItem, char *memory, unsigned int mem_size);
};
}}
#endif