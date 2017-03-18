#ifndef __cListener_h__
#define __cListener_h__

#include "cString.h"

class cListener
{	
public:
	virtual char* Execute(char *in, unsigned int size, unsigned int &length) = 0;
};
#endif