/**
 * \brief DateTime data type
 * 
 * \file cDateTime.h
 * \author Michal Kratky
 * \version 0.1
 * \date jul 2009
 */

#ifndef __cDateTime_h__
#define __cDateTime_h__

#include "assert.h"
#include "string.h"
#include "stdio.h"

struct tDateTime
{
	short mYear;
	unsigned char mMonth;
	unsigned char mDay;
	unsigned char mHour;
	unsigned char mMinute;
	unsigned char mSecond;
};

namespace common {
	namespace datatype {
class cDateTime
{
protected:
	tDateTime mDateTime;

public:

	inline void Set(int year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minute, unsigned char second);

	inline short GetYear() const;
	inline unsigned char GetMonth() const;
	inline unsigned char GetHour() const;
	inline unsigned char GetMinute() const;
	inline unsigned char GetSecond() const;

	unsigned int Encode(cDateTime* referenceItem, char *memory, unsigned int mem_size) const;
	unsigned int Decode(cDateTime* referenceItem, char *memory, unsigned int mem_size);

	// void SetCurrentDateTime();

	void Print(const char* delim = NULL);
};

void cDateTime::Set(int year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minute, unsigned char second)
{
	mDateTime.mYear = year;
	mDateTime.mMonth = month;
	mDateTime.mDay = day;
	mDateTime.mHour = hour;
	mDateTime.mMinute = minute;
	mDateTime.mSecond = second;
}

short cDateTime::GetYear() const
{
	return mDateTime.mYear;
}

unsigned char cDateTime::GetMonth() const
{
	return mDateTime.mMonth;
}

unsigned char cDateTime::GetHour() const
{
	return mDateTime.mHour;
}

unsigned char cDateTime::GetMinute() const
{
	return mDateTime.mMinute;
}

unsigned char cDateTime::GetSecond() const
{
	return mDateTime.mSecond;
}

}}
#endif
