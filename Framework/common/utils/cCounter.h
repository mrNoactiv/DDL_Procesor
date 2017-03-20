/**************************************************************************}
{                                                                          }
{    cCounter.h                                                            }
{      Warning - Win32 only!                                               }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001                      Michal Kratky                 }
{                                                                          }
{    VERSION: 0.2                            DATE 21/8/2002                }
{                                                                          }
{    following functionality:                                              }
{       timer                                                              }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      //                                                                  }
{                                                                          }
{**************************************************************************/

#ifndef __cCounter_h__
#define __cCounter_h__

#include <iostream>

namespace common {
	namespace utils {

class cCounter
{
private:
	unsigned int mValue;
	unsigned int mSumValue;  // suma of counter for average counter
	unsigned int mCount;     // number of counter for average counter

public:
	cCounter();
	~cCounter();

	inline void SetValue(unsigned int value);
	inline void Add(unsigned int value);
	inline void AddSum(unsigned int value);
	inline void Reset();
	inline void ResetSum();
	inline void Increment();
	inline void AddCounter();

	inline unsigned int GetValue() const;
	inline double GetAverage() const;
	inline unsigned int GetSum() const;

	inline int operator=(int value);

};

inline void cCounter::SetValue(unsigned int value) 
{ 
	mValue = value; 
}
inline void cCounter::Add(unsigned int value) 
{ 
	mValue += value; 
}
inline void cCounter::AddSum(unsigned int value) 
{ 
	mSumValue += value; 
}
inline int cCounter::operator=(int value) 
{ 
	return (unsigned int)(mValue = value); 
}
inline void cCounter::Reset()     
{	
	mValue = 0; 
} 
inline void cCounter::Increment() 
{ 
	mValue++; 
}

inline void cCounter::ResetSum()
{
	mValue = 0;
	mSumValue = 0;
	mCount = 0;
}

void cCounter::AddCounter()
{
	mSumValue += mValue;
	mCount++;
}

inline unsigned int cCounter::GetValue() const
{	
	return (unsigned int)mValue; 
}

inline double cCounter::GetAverage() const
{	
	double average = 0.0;
	if (mCount != 0)
	{
		average = (double)mSumValue/mCount;
	}
	return average; 
}

inline unsigned int cCounter::GetSum() const
{	
	return mSumValue; 
}

}}
#endif