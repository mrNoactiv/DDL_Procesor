/**
*	\file cDSCriticalException.h
*	\author Michal Kratky
*	\version 0.1
*	\date aug 2008
*	\brief cDSCriticalException
*/

#ifndef __cDSCriticalException_h__
#define __cDSCriticalException_h__

#include <iostream>
#include <exception>

using namespace std;

namespace dstruct {
  namespace paged {
	namespace core {

class cDSCriticalException: public exception
{
	char *mDetail;

public:

	cDSCriticalException(const char *detail)
	{
		mDetail = new char[strlen(detail)];
		strcpy(mDetail, detail);
	}

	virtual ~cDSCriticalException() throw()
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
