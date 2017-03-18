/**************************************************************************}
{                                                                          }
{    cExponentialRandomGenerator.h	                       		      		}
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
{    Poznamky:																					}
{    cExponentialRandomGenerator(const bool Randomize = true)              }
{    Randomize == true                                                     }
{      Pri kazdem vytvoreni tridy se bude generovat jina sekvence cisel.   }
{      Pocatecni hodnota (seminko) je odvozeno od casu.                    }
{    Randomize == false												                  }
{      Pokazde se generuje stejna posloupnost.                             }
{                                                                          }
{    cExponentialRandomGenerator(const int ARandSeed)                      }
{      Pocatecni hodnotu (seminko) je mozno explicitne uvest.              }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{                                                                          }
{                                                                          }
{**************************************************************************/
#ifndef __cExponentialRandomGenerator_h__
#define __cExponentialRandomGenerator_h__

#include <math.h>
#include "common/random/cAbstractRandomGenerator.h"
#include "common/random/cUniformRandomGenerator.h"

namespace common {
  namespace random {

class cExponentialRandomGenerator: public cAbstractRandomGenerator
{
public:
	cExponentialRandomGenerator(const bool Randomize = true);
	cExponentialRandomGenerator(const int ARandSeed);
	virtual ~cExponentialRandomGenerator();

	virtual double GetNext();
	unsigned int GetNextUInt(unsigned int maxValue);

protected:
	cUniformRandomGenerator m_UnifRandom;
};  // cExponentialRandomGenerator
}}
#endif  // __cExponentialRandomGenerator_h__
