/**************************************************************************}
{                                                                          }
{    cExponentialRandomGenerator.cpp                       		      		}
{                                                                          }
{                                                                          }
{                 Copyright (c) 1998, 2003					Jiri Dvorsky		}
{                                                                          }
{    VERSION: 2.0														DATE 27/2/2003    }
{                                                                          }
{             following functionality:													}
{    Generator nahodnych cisel s exponencialnim rozlozenim.			         }
{    Zalozeno na kodu z knihy Numerical Recipes.                           }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{                                                                          }
{                                                                          }
{**************************************************************************/
#include "common/random/cExponentialRandomGenerator.h"

namespace common {
  namespace random {

cExponentialRandomGenerator::cExponentialRandomGenerator(const bool Randomize):
	cAbstractRandomGenerator(), m_UnifRandom(Randomize)
{
}


cExponentialRandomGenerator::cExponentialRandomGenerator(const int ARandSeed):
	cAbstractRandomGenerator(), m_UnifRandom(ARandSeed)
{
}


cExponentialRandomGenerator::~cExponentialRandomGenerator()
{
}


double cExponentialRandomGenerator::GetNext()
{
	double dRet;
	do
	{
		dRet = m_UnifRandom.GetNext();
	} while (dRet == 0.0);
	return -log(dRet);
}

/*
 * Return random unsigned int with the maximal value.
 */
unsigned int cExponentialRandomGenerator::GetNextUInt(unsigned int maxValue)
{
	double rn = GetNext();
	if (rn < 0)
		{ rn = -rn; }
	if (rn > 1.0)
		{ rn = 1/rn; }
	
	return (unsigned int)(rn * maxValue);
}
}}
