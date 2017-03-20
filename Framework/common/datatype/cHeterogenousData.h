/**
 * \brief This data type contains one value of various data types. The main issue is as follows:
 *    there is an array with the maximal size (VALUE_LENGTH), it means all values are aligned to this size.
 * In the case of \see cPersistentArray_VarLength this problem occurs only in the main memory.
 * For example, this class is utilized in the case of \see cMetadaStorage as the value storage.
 * Supported data types are as follows: float, unsigned int, cDateTime. Feel free to add a support
 * for other data types.
 *
 * \author Michal Kratky
 * \version 0.1
 * \date jul 2009
 */

#ifndef __cHeterogenousData_h__
#define __cHeterogenousData_h__

#include "cMemory.h"
#include "cBasicType.h"
#include "cDateTime.h"
#include "cDateTimeType.h"

#define assert_size(size) \
  assert(VALUE_LENGTH >= sizeof(char) + size);

namespace common {
	namespace datatype {
class cHeterogenousData
{
private:
	char* mValue;

public:
	static const int VALUE_LENGTH = 10;

private:
	static inline const char* SetCode(char code, char* memory);
	static inline const char* CheckCode(char code, char* memory);

public:
	cHeterogenousData();

	inline void SetValue(float value);
	inline void SetValue(unsigned int value);
	inline void SetValue(const cDateTime &value);

	inline float GetFloat() const;
	inline unsigned int GetUInt() const;
	inline void GetDateTime(cDateTime &value) const;

	void Resize();
	void Resize(cMemory *memory);

	unsigned int Encode(char *memory, unsigned int max_size) const;
	unsigned int Decode(char *memory, unsigned int max_size);
};

/**
 * Set the float value.
 */
void cHeterogenousData::SetValue(float value)
{
	assert_size(sizeof(float));
	const char* mem = SetCode(cFloatType::CODE, mValue);
	*((float*)mem) = value;
}

/**
 * Set the unsigned int value.
 */
void cHeterogenousData::SetValue(unsigned int value)
{
	assert_size(sizeof(unsigned int));
	const char* mem = SetCode(cUIntType::CODE, mValue);
	*((unsigned int*)mem) = value;
}

/**
 * Set the cDateTime instance.
 */
void cHeterogenousData::SetValue(const cDateTime &value)
{
	int size = cDateTimeType::GetSerSize();
	assert_size(cDateTimeType::GetSerSize());
	const char* mem = SetCode(cDateTimeType::CODE, mValue);
	value.Encode(NULL, (char*)mem, VALUE_LENGTH-sizeof(char));
}

/**
 * Set code into the memory and return pointer to the blank memory.
 *
 * \param code The code of the data type (\see cBasicType)
 * \param memory Memory to be filled
 * \return Pointer to the new memory
 */
const char* cHeterogenousData::SetCode(char code, char* memory)
{
	*memory = code;
	return memory + sizeof(char);
}

/**
 * Get the float value.
 */
float cHeterogenousData::GetFloat() const
{
	assert_size(sizeof(float));
	const char* mem = CheckCode(cFloatType::CODE, mValue);
	return *((float*)mem);
}

/**
 * Get the unsigned int value.
 */
unsigned int cHeterogenousData::GetUInt() const
{
	assert_size(sizeof(unsigned int));
	const char* mem = CheckCode(cUIntType::CODE, mValue);
	return *((unsigned int*)mem);
}

/**
 * Get the cDateTime instance.
 */
void cHeterogenousData::GetDateTime(cDateTime &value) const
{
	assert_size(cDateTimeType::GetSerSize());
	const char* mem = CheckCode(cDateTimeType::CODE, mValue);
	value.Decode(NULL, (char*)mem, VALUE_LENGTH-1);
}

/**
 * Check code in the memory and return pointer to the rest of memory.
 *
 * \param code The code of the data type (\see cBasicType)
 * \param memory Memory containing the code in the first byte
 * \return Pointer to the new memory
 */
const char* cHeterogenousData::CheckCode(char code, char* memory)
{
	assert(*memory == code);
	return memory + sizeof(code);
}
}}
#endif