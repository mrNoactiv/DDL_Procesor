#ifndef __cSocketServer_h__
#define __cSocketServer_h__

#include <stdio.h>
#include "winsock2.h"
#include "cSocket.h"

class cSocketClient: public cSocket
{

public:
	cSocketClient();
	~cSocketClient();

	int SendReceive();
};
#endif