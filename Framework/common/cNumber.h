/**************************************************************************}
{                                                                          }
{    cNumber.h                                                             }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001, 2003                Michal Kratky                 }
{                                                                          }
{    VERSION: 0.2                            DATE 17/4/2002                }
{                                                                          }
{    following functionality:                                              }
{       general math class                                                 }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cNumber_h__
#define __cNumber_h__

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#include "common/cCommon.h"
// #include "common/utils/cSSEUtils.h"

using namespace common;
// using namespace common::utils;

static const char LogTable256_1[256] = 
{
#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
    0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    LT(5), LT(6), LT(6), LT(7), LT(7), LT(7), LT(7),
    LT(8), LT(8), LT(8), LT(8), LT(8), LT(8), LT(8), LT(8)
};

static const char LogTable256_0[256] = 
{
#define LT0(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
    -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
    LT0(5), LT0(6), LT0(6), LT0(7), LT0(7), LT0(7), LT0(7),
    LT0(8), LT0(8), LT0(8), LT0(8), LT0(8), LT0(8), LT0(8), LT0(8)
};

class cNumber
{
public:
	static const unsigned int BYTE_LENGTH = 8;
	static const unsigned int UINT_LENGTH = 32;

	cNumber(void);
	~cNumber(void);

	static unsigned int power2(unsigned int x);
	static unsigned int log2(unsigned int x);
	static void Srand(unsigned int srandv);
	static void Srand();
	static double Rnd(unsigned int maxValue);
	static double Rnd();
	inline static ullong FastRand(ullong seed);
	static unsigned int Random(unsigned int maxValue);
	static unsigned int Random(unsigned int srandv, unsigned int maxValue);
	inline static unsigned int DivMul2(unsigned int n, unsigned int i);
	inline static int Abs(int value);
	inline static float Abs(float value);
	inline static unsigned int BitsToBytes(unsigned int bitLength);
	inline static unsigned int BitsToChunks(unsigned int bitLength, uint chunkLength);

	inline static int atoi_fast(const char *str);
	inline static int atoi_ffast(const char *str, int length = -1);
	inline static uint atoui_ffast(const char *str, int length = -1);
	inline static ullong atoull_ffast(const char *str, int length = -1);

	inline static int Length(uint value);
};

inline unsigned int cNumber::DivMul2(unsigned int n, unsigned int i)
{
	int number = n >> i;
	if (((1 << (i-1)) & n) != 0)
		number++;
	return number;
}

// Return absolute value of int number.
inline int cNumber::Abs(int number)
{
	if (number >= 0)
	{
		return number;
	}
	else
	{
		return -number;
	}
}

// Return absolute value of float number
inline float cNumber::Abs(float number)
{
	if (number >= 0)
	{
		return number;
	}
	else
	{
		return -number;
	}
}

unsigned int cNumber::BitsToBytes(unsigned int bitLength)
{
	unsigned int byteLen = bitLength / 8;
	if (bitLength % 8 > 0)
	{
		byteLen++;
	}
	return byteLen;
}

unsigned int cNumber::BitsToChunks(uint bitLength, uint chunkLength)
{
	unsigned int chunkLen = bitLength / chunkLength;
	if (bitLength % chunkLength != 0)
	{
		chunkLen++;
	}
	return chunkLen;
}

/**
 * http://software.intel.com/en-us/articles/fast-random-number-generator-on-the-intel-pentiumr-4-processor
 * This article includes the SSE-version as well.
 */
inline ullong cNumber::FastRand(ullong seed)
{
	ullong tmpSeed = (214013 * seed + 2531011);
	return (tmpSeed>>16)&0x7FFF;
}

int cNumber::atoi_fast(const char * str)
{
    int val = 0;
    while( *str ) {
        val = val*10 + (*str++ - '0');
    }
    return val;
}

inline int cNumber::atoi_ffast(const char *str, int length)
{
	size_t len;

	if (length == -1)
	{
		len = strlen(str);
	}
	else
	{
		len = length;
	}

	int value = 0;

	int sign = 1;
    if (str[0] == '-') // handle negative
	{ 
		sign = -1;
        ++str;
        --len;
    }

    switch (len) { // handle up to 10 digits, assume we're 32-bit
        case 10:    value += (str[len-10] - '0') * 1000000000;
        case  9:    value += (str[len- 9] - '0') * 100000000;
        case  8:    value += (str[len- 8] - '0') * 10000000;
        case  7:    value += (str[len- 7] - '0') * 1000000;
        case  6:    value += (str[len- 6] - '0') * 100000;
        case  5:    value += (str[len- 5] - '0') * 10000;
        case  4:    value += (str[len- 4] - '0') * 1000;
        case  3:    value += (str[len- 3] - '0') * 100;
        case  2:    value += (str[len- 2] - '0') * 10;
        case  1:    value += (str[len- 1] - '0');
    }
    value *= sign;

	return value;
}

inline uint cNumber::atoui_ffast(const char *str, int length)
{
	size_t len;

	if (length == -1)
	{
		len = strlen(str);
	}
	else
	{
		len = length;
	}

	uint value = 0;

    switch (len) { // handle up to 10 digits, assume we're 32-bit
        case 10:    value += (str[len-10] - '0') * 1000000000;
        case  9:    value += (str[len- 9] - '0') * 100000000;
        case  8:    value += (str[len- 8] - '0') * 10000000;
        case  7:    value += (str[len- 7] - '0') * 1000000;
        case  6:    value += (str[len- 6] - '0') * 100000;
        case  5:    value += (str[len- 5] - '0') * 10000;
        case  4:    value += (str[len- 4] - '0') * 1000;
        case  3:    value += (str[len- 3] - '0') * 100;
        case  2:    value += (str[len- 2] - '0') * 10;
        case  1:    value += (str[len- 1] - '0');
    }

	return value;
}

inline ullong cNumber::atoull_ffast(const char *str, int length)
{
	assert(length <= 15);
	size_t len;

	if (length == -1)
	{
		len = strlen(str);
	}
	else
	{
		len = length;
	}

	ullong value = 0ULL;

    switch (len) { // handle up to ?? 15 ?? digits, assume we're 32-bit
        case 15:    value += (ullong)(str[len-15] - '0') * 100000000000000ULL;
        case 14:    value += (ullong)(str[len-14] - '0') * 10000000000000ULL;
        case 13:    value += (ullong)(str[len-13] - '0') * 1000000000000ULL;
        case 12:    value += (ullong)(str[len-12] - '0') * 100000000000ULL;
        case 11:    value += (ullong)(str[len-11] - '0') * 10000000000ULL;
        case 10:    value += (ullong)(str[len-10] - '0') * 1000000000ULL;
        case  9:    value += (str[len- 9] - '0') * 100000000;
        case  8:    value += (str[len- 8] - '0') * 10000000;
        case  7:    value += (str[len- 7] - '0') * 1000000;
        case  6:    value += (str[len- 6] - '0') * 100000;
        case  5:    value += (str[len- 5] - '0') * 10000;
        case  4:    value += (str[len- 4] - '0') * 1000;
        case  3:    value += (str[len- 3] - '0') * 100;
        case  2:    value += (str[len- 2] - '0') * 10;
        case  1:    value += (str[len- 1] - '0');
    }

	return value;
}

/*
 * Not tested.
 */
inline int cNumber::Length(uint value)
{
	int len;
	register unsigned int t, tt; 
	if (tt = value >> 16)
	{
		len = (t = tt >> 8) ? 24 + LogTable256_1[t] : 16 + LogTable256_1[tt];
	}
	else 
	{
		len = (t = value >> 8) ? 8 + LogTable256_1[t] : LogTable256_1[value];
	}
	return len;
}

#endif