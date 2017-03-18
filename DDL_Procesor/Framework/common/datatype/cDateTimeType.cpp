#include "cDateTimeType.h"

using namespace common::datatype;

/**
 * Decode item from the memory (for serialization or compression purpose).
 *
 * \param item Decoded item is stored here.
 * \param referenceItem Reference item, which can be used during the decoding (not in this type of class).
 * \param memory Source memory from which the item is decoded.
 * \param mem_size Size of the memory.
 * \return Number of bytes readed from memory.
 * \see cBasicType::Decode()
 */

static unsigned int Decode(cDateTime* item, cDateTime* referenceItem, char *memory, unsigned int mem_size)
{
	UNREFERENCED_PARAMETER(referenceItem); 
	return item->Decode(referenceItem, memory, mem_size);
}

/**
 * Encode item into the memory (for deserialization or decompression purpose).
 *
 * \param item Input item.
 * \param referenceItem Reference item, which can be used during the encoding (not in this type of class).
 * \param memory Source memory where the item is encoded.
 * \param mem_size Size of the memory.
 * \return Number of bytes writen into the memory
 * \see cBasicType::Decode()
 */
static unsigned int Encode(const cDateTime* item, cDateTime* referenceItem, char *memory, unsigned int mem_size)
{
	UNREFERENCED_PARAMETER(referenceItem); 
	return item->Encode(referenceItem, memory, mem_size);
}