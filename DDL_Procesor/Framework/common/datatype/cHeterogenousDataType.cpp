#include "cHeterogenousDataType.h"

using namespace common::datatype;

cHeterogenousDataType::cHeterogenousDataType()			
{
}

cHeterogenousDataType::~cHeterogenousDataType()
{
}

/**
 * Encode item into the memory.
 *
 * @param item Input item.
 * @param referenceItem Reference item, which can be used during the encoding (not in this type of class).
 * @param memory Source memory where the item is encoded.
 * @param mem_size Size of the memory.
 * @return Number of bytes written into the memory
 * @see cBasicType::Encode()
*/
unsigned int cHeterogenousDataType::Encode(const Type* item, Type* referenceItem, char *memory, unsigned int mem_size)
{
	UNREFERENCED_PARAMETER(referenceItem); 
	return item->Encode(memory, mem_size);
}

/**
 * Decode item from the memory.
 *
 * \param item Decoded item is stored here.
 * \param referenceItem Reference item, which can be used during the decoding (not in this type of class).
 * \param memory Source memory from which the item is decoded.
 * \param mem_size Size of the memory.
 * \return Number of bytes read from memory.
 * @see cBasicType::Decode()
 */
unsigned int cHeterogenousDataType::Decode(Type* item, Type* referenceItem, char *memory, unsigned int mem_size)		
{ 
	UNREFERENCED_PARAMETER(referenceItem); 
	return item->Decode(memory, mem_size); 
}