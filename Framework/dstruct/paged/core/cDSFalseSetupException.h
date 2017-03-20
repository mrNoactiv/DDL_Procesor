/**
*	\file cDSFalseSetupException.h
*	\author Michal Kratky
*	\version 0.1
*	\date aug 2008
*	\brief cDSFalseSetupException
*/

#ifndef __cDSFalseSetupException_h__
#define __cDSFalseSetupException_h__

#include <iostream>
#include <exception>

using namespace std;

namespace dstruct {
  namespace paged {
	namespace core {

class cDSFalseSetupException: public exception
{
	char *mDetail;

public:
	cDSFalseSetupException(const char *detail)
	{
		mDetail = new char[strlen(detail)];
		strcpy(mDetail, detail);
	}

	virtual ~cDSFalseSetupException() throw()
	{
		if (mDetail != NULL)
		{
			delete mDetail;
		}
	}

	virtual const char* what() const throw()
	{
	  return mDetail;
	}
};
}}}
#endif
