/**************************************************************************}
{                                                                          }
{    cGaussRandomGenerator.h			                       		      		}
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
{    Poznamky:																					}
{    cGaussRandomGenerator(const bool Randomize = true)							}
{    Randomize == true                                                     }
{      Pri kazdem vytvoreni tridy se bude generovat jina sekvence cisel.   }
{      Pocatecni hodnota (seminko) je odvozeno od casu.                    }
{    Randomize == false												                  }
{      Pokazde se generuje stejna posloupnost.                             }
{                                                                          }
{    cGaussRandomGenerator(const int ARandSeed)										}
{      Pocatecni hodnotu (seminko) je mozno explicitne uvest.              }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{                                                                          }
{                                                                          }
{**************************************************************************/
#ifndef __cGaussRandomGenerator_h__
#define __cGaussRandomGenerator_h__

#include <math.h>
#include "common/random/cAbstractRandomGenerator.h"
#include "common/random/cUniformRandomGenerator.h"

namespace common {
  namespace random {

class cGaussRandomGenerator: public cAbstractRandomGenerator
{
public:
	cGaussRandomGenerator();
	cGaussRandomGenerator(const bool Randomize);
	cGaussRandomGenerator(const int ARandSeed);
	virtual ~cGaussRandomGenerator();
	void Init(const bool Randomize = true);

	virtual double GetNext();
	unsigned int GetNextUInt(unsigned int maxValue);

protected:
	cUniformRandomGenerator m_UnifRandom;
	int iset;
	double gset;
};  // cGaussRandomGenerator
}}
#endif  // __cGaussRandomGenerator_h__
