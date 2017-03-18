#include "cHeterogenousData.h"

using namespace common::datatype;

cHeterogenousData::cHeterogenousData()
{
}

/**
 * Resize this item, no parameter is passed due to the constant nature of attribute values.
 */
void cHeterogenousData::Resize()
{
	mValue = new char[VALUE_LENGTH];
}

/**
 * Resize this item using the memory object.
 */
void cHeterogenousData::Resize(cMemory *memory)
{
	mValue = memory->GetMemory(VALUE_LENGTH);
}

/**
 * Encode this item into the memory. It does not use any compression.
 *
 * @param memory Destination memory, where the tuple is encoded.
 * @param max_size Size of the memory.
 * @return Number of bytes writen into the memory
 * @see cNTreeTuple::Encode()
 */
unsigned int cHeterogenousData::Encode(char *memory, unsigned int max_size) const
{
	assert(VALUE_LENGTH <= max_size);
	memcpy(memory, mValue, VALUE_LENGTH);
	return VALUE_LENGTH;
}

/**
 * Decode tuple from the memory. It does not use any compression.
 * \param memory Source memory from which the tuple is decoded.
 * \param max_size Size of the memory.
 * \return Number of bytes readed from memory.
 * @see cNTreeTuple::Encode()
 */
unsigned int cHeterogenousData::Decode(char *memory, unsigned int max_size)
{
	assert(VALUE_LENGTH <= max_size);
	memcpy(mValue, memory, VALUE_LENGTH);
	return VALUE_LENGTH;
}