/**************************************************************************}
{                                                                          }
{    cSignature.h                                                          }
{                                                                          }
{                                                                          }
{    Copyright (c) 2003                      Michal Kratky                 }
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

#include "cSignature.h"

const unsigned int cSignature::GeneratorType [] = { cSignature::GENTYPE_MOD, cSignature::GENTYPE_FASTUNI, cSignature::GENTYPE_GAUS, cSignature::GENTYPE_EXP };

cSignature::cSignature(): cBitString()
{
}

cSignature::cSignature(int size): cBitString(size)
{
}

cSignature::~cSignature(void)
{
}

void cSignature::Add(char *string)
{
	int hash = 0;

	for (unsigned int i = 0 ; i < strlen(string) ; i++)
	{
		hash += string[i];
	}

	cUniformRandomGenerator gen((const int)hash);    // init the generator according to value

	for (unsigned int i = 0 ; i < strlen(string) ; i++)
	{
		int index = (unsigned int)(gen.GetNext() * mLength);
		SetBit(index, true);
	}
}

void cSignature::GenerateSignature(unsigned int value, char* signature, unsigned int generatorType, unsigned int sigLength, unsigned int bitCount,
	cUniformRandomGenerator *genUni, cGaussRandomGenerator *genGaus, cExponentialRandomGenerator *genExp)
{
	for (int i = 0 ; i < bitCount ; i++)
	{
		unsigned short order = GetTrueBitOrder(value, sigLength, i, generatorType, genUni, genGaus, genExp);
		cBitString::SetBit(signature, order, true);
	}
}

void cSignature::GenerateSignature(uint value, cArray<ullong>* trueBitOrders, uint generatorType, uint sigLength, uint bitCount,
	cUniformRandomGenerator *genUni, cGaussRandomGenerator *genGaus, cExponentialRandomGenerator *genExp)
{
	for (int i = 0 ; i < bitCount ; i++)
	{
		ullong order = GetTrueBitOrder(value, sigLength, i, generatorType, genUni, genGaus, genExp);
		trueBitOrders->Add(order);
	}
}

void cSignature::GenerateSignature(uint value, cArray<ullong>* trueBitOrders, uint sigLength, uint bitCount)
{
	for (int i = 0; i < bitCount; i++)
	{
		ullong order = GetTrueBitOrder(value, sigLength, i);
		trueBitOrders->Add(order);
	}
}

uint cSignature::GetTrueBitOrder(ullong value, uint sigLength, uint bitOrder)
{
	return ((value % sigLength) + (bitOrder * (1 + (value % (sigLength - 1))))) % sigLength;
}

uint cSignature::GetTrueBitOrder(ullong value, uint sigLength, uint bitOrder, uint generatorType,
	cUniformRandomGenerator *genUni, cGaussRandomGenerator *genGaus, cExponentialRandomGenerator *genExp)
{
	uint index;

	if (generatorType == GENTYPE_MOD)
	{
		//index = cNumber::FastRand(value + bitOrder) % sigLength;
		//index = value % sigLength + bitOrder;
		index = ((value % sigLength) + (bitOrder * (1 + (value % (sigLength - 1))))) % sigLength;
		//index = 1 + (value % (sigLength - 1));

		//index = ((value % sigLength) + (bitOrder * (1 + (value % (sigLength - 1)))));
	}
	else // if (generatorType == GENTYPE_FASTUNI)
	{
		//index = cNumber::FastRand(value + bitOrder) % sigLength;

		index = 1 + (value % (sigLength - 1));
		//double MAX = 168420;
		//index = (uint)((value / MAX) * sigLength);
	} 
	
	//else 
	//{
	//	double rnd = 1.0;

	//	if (generatorType == GENTYPE_GAUS)
	//	{
	//		rnd = genGaus->GetNext();
	//		//rnd = genGaus->GetNextUInt(sigLength);
	//	}
	//	else if (generatorType == GENTYPE_EXP)
	//	{
	//		rnd = genExp->GetNext();
	//	}
	//	else if (generatorType == GENTYPE_UNI)
	//	{
	//		rnd = genUni->GetNext();
	//	}

	//	if (rnd < 0.0)
	//	{
	//		rnd = -rnd;
	//	}

	//	index = (uint) (rnd * value) % sigLength; // rnd;
	//}

	return index;
}

/**
 * Generate a signature of the string.
 */
void cSignature::GenerateSignature(wchar_t* string, unsigned int &signature)
{
	signature = 0;
	for (unsigned int i = 0 ; i < wcslen(string) ; i++)
	{
		signature += string[i];
	}
}