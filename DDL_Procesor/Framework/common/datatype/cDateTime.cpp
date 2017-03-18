#include "cDateTime.h"

using namespace common::datatype;

/**
 * Encode item into the memory (for deserialization or decompression purpose).
 *
 * \param referenceItem Reference item, which can be used during the encoding (not in this type of class).
 * \param memory Source memory where the item is encoded.
 * \param mem_size Size of the memory.
 * \return Number of bytes written into the memory
 * \see cDateTimeType::Decode()
 */
unsigned int cDateTime::Encode(cDateTime* referenceItem, char *memory, unsigned int mem_size) const
{
	// UNREFERENCED_PARAMETER(referenceItem); 
	assert(mem_size >= sizeof(tDateTime));

	memcpy(memory, (char*)&mDateTime, sizeof(tDateTime));
	return sizeof(tDateTime);
}

/**
 * Decode item from the memory (for serialization or compression purpose).
 *
 * \param referenceItem Reference item, which can be used during the decoding (not in this type of class).
 * \param memory Source memory from which the item is decoded.
 * \param mem_size Size of the memory.
 * \return Number of bytes read from memory.
 * \see cDateTimeType::Decode()
 */

unsigned int cDateTime::Decode(cDateTime* referenceItem, char *memory, unsigned int mem_size)
{
	// UNREFERENCED_PARAMETER(referenceItem); 
	assert(mem_size >= sizeof(tDateTime));

	memcpy((char*)&mDateTime, memory, sizeof(tDateTime));
	return sizeof(tDateTime);
}

/*void cDateTime::SetCurrentDateTime()
{
}*/

/**
 * Print date time.
 * \param delim Delimiter inserted in the end of the last line (may be NULL)
 */
void cDateTime::Print(const char* delim)
{
	printf("%d.%d.%d %d:%d:%d%s", mDateTime.mDay, mDateTime.mMonth, mDateTime.mYear, mDateTime.mHour, 
		mDateTime.mMinute, mDateTime.mSecond, delim);
}