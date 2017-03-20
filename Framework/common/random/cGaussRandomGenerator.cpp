/**************************************************************************}
{                                                                          }
{    cGaussRandomGenerator.cpp		                       		      		}
{                                                                          }
{                                                                          }
{                 Copyright (c) 1998, 2003					Jiri Dvorsky		}
{                                                                          }
{    VERSION: 2.0														DATE 27/2/2003    }
{                                                                          }
{             following functionality:													}
{    Generator nahodnych cisel s normalnim (Gaussovskym) rozlozenim.       }
{    Zalozeno na kodu z knihy Numerical Recipes.                           }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{                                                                          }
{                                                                          }
{**************************************************************************/
#include "common/random/cGaussRandomGenerator.h"

namespace common {
  namespace random {

/*
 * In this case, you must call Init().
 */
cGaussRandomGenerator::cGaussRandomGenerator():
	cAbstractRandomGenerator(), m_UnifRandom()
{
}

cGaussRandomGenerator::cGaussRandomGenerator(const bool Randomize):
	cAbstractRandomGenerator(), m_UnifRandom()
{
	Init(Randomize);
}

/*
 * You must call it after the empty constructor.
 */
void cGaussRandomGenerator::Init(const bool Randomize)
{
	iset = 0;
	m_UnifRandom.Init(Randomize);
}

cGaussRandomGenerator::cGaussRandomGenerator(const int ARandSeed):
	cAbstractRandomGenerator(), m_UnifRandom(ARandSeed), iset(0)
{
}


cGaussRandomGenerator::~cGaussRandomGenerator()
{
}


double cGaussRandomGenerator::GetNext()
{
	double fac, rsq, v1, v2;
	if (iset == 0)
	{
		do
		{
			v1 = 2.0*m_UnifRandom.GetNext() - 1.0;
			v2 = 2.0*m_UnifRandom.GetNext() - 1.0;
			rsq = v1*v1 + v2*v2;
		} while (rsq >= 1.0 || rsq == 0.0);
		fac = sqrt(-2.0*log(rsq)/rsq);
		gset = v1*fac;
		iset = 1;
		return v2*fac;
	}
	iset = 0;
	return gset;
}

/*
 * Return random unsigned int with the maximal value.
 */
unsigned int cGaussRandomGenerator::GetNextUInt(unsigned int maxValue)
{
	double rn = GetNext();
	if (rn < 0)
		{ rn = -rn; }
	if (rn > 1.0)
		{ rn = 1/rn; }
	
	return (unsigned int)(rn * maxValue);
}
}}