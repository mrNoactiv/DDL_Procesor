#ifndef __cSocket_h__
#define __cSocket_h__

#include <stdio.h>
#include <windows.h>
#include "winsock.h"
//#include "cString.h"

class cmSocket
{
protected:
    SOCKET mSocket;
	cString mSendString;
	cString mRecString;
	int mSendBytes;
	int mRecBytes;

public:
	cmSocket();
	~cmSocket();
};
#endif