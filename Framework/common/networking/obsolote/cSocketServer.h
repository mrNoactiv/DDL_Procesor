#ifndef __cSocketServer_h__
#define __cSocketServer_h__

#include <stdio.h>
#include <windows.h>
#include "winsock.h"

#include "cSocket.h"
#include "cListener.h"

class cSocketServer: public cSocket
{	

public:
	cSocketServer(char* addr, unsigned short port);
	~cSocketServer();

	int Listen(cListener *listener);
};
#endif