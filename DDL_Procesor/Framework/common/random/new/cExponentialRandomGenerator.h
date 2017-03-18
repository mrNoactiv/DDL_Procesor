/**
*	\file cExponentialRandomGenerator.h
*	\author Jiri Dvorsky (1998-2003), Jiří Walder (2010-2011)
*	\version 2.1
*	\date dec 2011
*	\brief Generator nahodnych cisel s exponencialnim rozlozenim. Zalozeno na kodu z knihy Numerical Recipes.
*
*   Poznamky:
*    cExponentialRandomGenerator(const bool Randomize = true)
*    Randomize == true
*      Pri kazdem vytvoreni tridy se bude generovat jina sekvence cisel.
*      Pocatecni hodnota (seminko) je odvozeno od casu.
*    Randomize == false
*      Pokazde se generuje stejna posloupnost.
*
*    cExponentialRandomGenerator(const int ARandSeed)
*      Pocatecni hodnotu (seminko) je mozno explicitne uvest.
*/

#ifndef __cExponentialRandomGenerator_new_h__
#define __cExponentialRandomGenerator_new_h__

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "cAbstractRandomGenerator.h"
#include "cUniformRandomGenerator.h"

namespace common {
  namespace random_new {

class cExponentialRandomGenerator: public cAbstractRandomGenerator
{
public:
	cExponentialRandomGenerator(const int ARandSeed);
	cExponentialRandomGenerator(const bool Randomize=false,double lambda2 = 2.0);
	double GetInvExpFun(double y);
	virtual ~cExponentialRandomGenerator();
	double GetNext32();
	virtual double GetNext();
	unsigned int GetNext32buffer(unsigned int max=UINT_MAX);
	void InitBuffer(int count);
	double Func(double x);
	unsigned int RandRange(unsigned int min,unsigned int max);

	double mLambda, mLambda2;
	double mMaxnum;
	unsigned int *mBuffer;
	unsigned int mBufferCount;

protected:
	cUniformRandomGenerator m_UnifRandom;
};  // cExponentialRandomGenerator
}}
#endif  // __cExponentialRandomGenerator_h__