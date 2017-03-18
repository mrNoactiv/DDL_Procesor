/**
*	\file cAbstractRandomGenerator.h
*	\author Jiri Dvorsky (1998-2003), Jiří Walder (2010-2011)
*	\version 2.1
*	\date dec 2011
*	\brief Abstraktni generator nahodnych cisel. Metoda GetNext vraci nahodna cisla z otevreneho intervalu (0;1).
*/

#ifndef __cAbstractRandomGenerator_new_h__
#define __cAbstractRandomGenerator_new_h__

namespace common {
  namespace random_new {

#define M_PI       3.14159265358979323846

class cAbstractRandomGenerator
{
public:
	cAbstractRandomGenerator();
	virtual ~cAbstractRandomGenerator();

	virtual double GetNext() = 0;
};  // cAbstractRandomGenerator


inline cAbstractRandomGenerator::cAbstractRandomGenerator()
{
}


inline cAbstractRandomGenerator::~cAbstractRandomGenerator()
{
}
}}
#endif  // __cAbstractRandomGenerator_h__
