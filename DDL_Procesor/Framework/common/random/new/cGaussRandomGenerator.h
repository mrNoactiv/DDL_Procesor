/**
*	\file  cGaussRandomGenerator.h
*	\author Jiri Dvorsky (1998-2003), Jiří Walder (2010-2011)
*	\version 2.1
*	\date dec 2011
*	\brief Generator nahodnych cisel s normalnim (Gaussovskym) rozlozenim. Zalozeno na kodu z knihy Numerical Recipes.
*
*   Poznamky:
*    cGaussRandomGenerator(const bool Randomize = true)
*    Randomize == true
*      Pri kazdem vytvoreni tridy se bude generovat jina sekvence cisel.
*      Pocatecni hodnota (seminko) je odvozeno od casu.
*    Randomize == false
*      Pokazde se generuje stejna posloupnost.
*
*    cGaussRandomGenerator(const int ARandSeed)
*      Pocatecni hodnotu (seminko) je mozno explicitne uvest.
*/

#ifndef __cGaussRandomGenerator_new_h__
#define __cGaussRandomGenerator_new_h__

#include <math.h>
#include <stdlib.h>
#include "cAbstractRandomGenerator.h"
#include "cUniformRandomGenerator.h"

namespace common {
  namespace random_new {

class cGaussRandomGenerator: public cAbstractRandomGenerator
{
public:
	cGaussRandomGenerator(const int ARandSeed);
	cGaussRandomGenerator(const bool Randomize = false,double sigma2 = 1.0);
	virtual ~cGaussRandomGenerator();

	double GetInvNormFun(double y);
	double GetNext32();
	virtual double GetNext();
	unsigned int GetNext32buffer(unsigned int max = UINT_MAX);
	unsigned long long GetNext64buffer();
	void InitBuffer(int count);
	double Func(double x);
	unsigned int RandRange(unsigned int min,unsigned int max);
	unsigned long long RandRange64(unsigned long long min,unsigned long long max);

	double sigma,sigma2;
	double maxnum;
	unsigned int *mBuffer;
	unsigned int mBufferCount;

protected:
	cUniformRandomGenerator m_UnifRandom;
	int iset;
	double gset;
};  // cGaussRandomGenerator
}}
#endif  // __cGaussRandomGenerator_h__