/**************************************************************************}
{                                                                          }
{    cSignature.cpp                                                        }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001                      Michal Kratky                 }
{                                                                          }
{    VERSION: 0.01                           DATE 04/11/2003               }
{                                                                          }
{    following functionality:                                              }
{       signature                                                          }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cSignature_h__
#define __cSignature_h__

#include "common/cCommon.h"
#include "common/cBitString.h"
#include "common/random/cUniformRandomGenerator.h"
#include "common/random/cExponentialRandomGenerator.h"
#include "common/random/cGaussRandomGenerator.h"

using namespace common;
using namespace common::random;

class cSignature: public cBitString
{
public:
	static const int unsigned GENTYPE_UNI = 0;
	static const int unsigned GENTYPE_MOD = 1;
	static const int unsigned GENTYPE_GAUS = 2;
	static const int unsigned GENTYPE_EXP = 3;
	static const int unsigned GENTYPE_FASTUNI = 4;

	static const unsigned int GeneratorType[];

	cSignature();
	cSignature(int size);
	~cSignature(void);

	void Add(char *string);

	static void GenerateSignature(unsigned int value, char* signature, unsigned int generatorType, unsigned int sigLength, unsigned int bitCount, cUniformRandomGenerator *genUni = NULL, cGaussRandomGenerator *genGaus = NULL, cExponentialRandomGenerator *genExp = NULL);
	static void GenerateSignature(uint value, cArray<ullong>* trueBitOrders, uint generatorType, uint sigLength, uint bitCount, cUniformRandomGenerator *genUni = NULL, cGaussRandomGenerator *genGaus = NULL, cExponentialRandomGenerator *genExp = NULL);
	static uint GetTrueBitOrder(ullong value, uint sigLength, uint bitOrder, uint generatorType, cUniformRandomGenerator *genUni = NULL, cGaussRandomGenerator *genGaus = NULL, cExponentialRandomGenerator *genExp = NULL);

	static void GenerateSignature(uint value, cArray<ullong>* trueBitOrders, uint sigLength, uint bitCount);
	static uint GetTrueBitOrder(ullong value, uint sigLength, uint bitOrder);

	void static GenerateSignature(wchar_t* string, unsigned int &signature);
};
#endif
