/**************************************************************************}
{                                                                          }
{    cNumber.cpp                                                           }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001, 2003                    Michal Kratky             }
{                                                                          }
{    VERSION: 0.2                            DATE 17/3/2002                }
{                                                                          }
{    following functionality:                                              }
{       general math class                                                 }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#include "cNumber.h"
#include "cCommon.h"
using namespace common;

cNumber::cNumber(void)
{
}

cNumber::~cNumber(void)
{
}

/**
 * @author: Tomas Skopal, (c)7.2002
 **/
unsigned int cNumber::power2(unsigned int x)
{
	unsigned int res =1;
	res <<= x;
  return res;
}

/**
 * Very stupid algorithm for log2 calculation.
 * Only for 32bit number.
 */
unsigned int cNumber::log2(unsigned int x)
{
	int ret = -1;
	llong pow = 1;

	for (unsigned int i = 1 ; i <= 32 ; i++)
	{
		if ((pow <<= 1) >= x)
		{
			ret = i;
			break;
		}
	}

  return ret;
}

/**
 * Return pseudo random unsigned int in <0,maxValue>.
 */
unsigned int cNumber::Random(unsigned int maxValue)
{
	return (unsigned int)(cNumber::Rnd()*maxValue);
}

/**
 * Return pseudo random unsigned int in <0,maxValue>. Init it by srandv.
 */
unsigned int cNumber::Random(unsigned int srandv, unsigned int maxValue)
{
	return (unsigned int)(cNumber::Rnd(srandv)*maxValue);
}

/**
 * Return pseudo random float in <0,1>.
 */
double cNumber::Rnd()
{
	// srand((unsigned)time(NULL) * rand());
	double rnd = (double)rand();
	double tmp = rnd / RAND_MAX;
	return tmp;
}

/**
 * Return pseudo random float in <0,1>. Init it by srandv.
 */
double cNumber::Rnd(unsigned int srandv)
{
	srand(srandv);
	double rnd = (double)rand();
	double tmp = rnd / RAND_MAX;
	return tmp;
}

/**
 * Init generator.
 */
void cNumber::Srand(unsigned int srandv)
{
	srand(srandv);
}

/**
 * Init generator.
 */
void cNumber::Srand()
{
	time_t timer;
	srand((unsigned int)time(&timer));
}

