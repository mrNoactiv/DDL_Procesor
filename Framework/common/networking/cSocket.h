/**************************************************************************}
{                                                                          }
{    cSocket.h														       }
{                                                                          }
{                                                                          }
{    Copyright (c) 2012                      Ales Nedbalek                 }
{                                                                          }
{    VERSION: 0.1                            DATE 1/10/2012                }
{                                                                          }
{    following functionality:                                              }
{		Sending/receiving stream on TCP,UDP. Set Win32 SOCKETS.            }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      1/10/2012                                                           }
{                                                                          }
{**************************************************************************/

#ifndef __cSocket_h__
#define __cSocket_h__

#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include "dbms/ddbms/cResultSet.h"
#include "dbms/ddbms/cCommand.h"
//CRC
#include "common/cCRCStatic.h"

using namespace CSDBMS;

namespace common {
	namespace networking {

		class cSocket
		{
		private:
			//times for non-block wait max, after that it wil end in never end loop
			const static int Time = 1000;//sec
			const static int uTime = 0;//usec

			//settings for view network adapters
			const static int LIST_ADAPTER_SIZE = 20;
			const static int HOST_NAME_SIZE = 80;

		public:
			cSocket(void);
			~cSocket(void);

			//*******************TCP*******************
			static int initSocketTCPListen(ULONG ip,unsigned short port,bool setReuseAddr = true, bool setWaitTime= true,bool setTCPNoDelay = false);
			static int SocketTCPListenAccept(SOCKET *s);
			static int SocketTCPListenAccept(SOCKET *s, sockaddr* addr, int* addrlen);
			//*********************************************************
			static int initSocketTCPWriteRead(ULONG ip,unsigned short port,bool setWaitTime = true);
			//old
			//bool ReadTCP(SOCKET *s,cResultSet *resultSet);
			//bool ReadTCP(SOCKET *s,char *str);
			//bool WriteTCP(SOCKET *s,cCommand *command);
			//bool WriteTCP(SOCKET *s,char *str,int size);
			static bool WriteReadTCP(SOCKET *s,cCommand *command,cResultSet *resultSet);
			static bool WriteReadTCPSERVER(SOCKET *s,cCommand *c,cResultSet *r);

			//bool WriteReadTCP(SOCKET *s,char *bufferIN,int *sizeIN,char *buffer0ut,int *sizeOut,int *len);

			//*********************SEND RECV**********************
			//
			static int SelectSend(SOCKET *socket);
			static int SelectRecv(SOCKET *socket);
			//old
			//static bool Send(SOCKET *s,char *buffer,unsigned int size);
			//static bool Recv(SOCKET *s,char *buffer,unsigned int *size);
			//
			static bool Send(SOCKET *s,cCommand *com);
			static bool Send(SOCKET *s,cResultSet *res);
			static bool Recv(SOCKET *s,cCommand *com);
			static bool Recv(SOCKET *s,cResultSet *res);
			//
			static bool Send(SOCKET *s,cFrame *frame);
			static bool Recv(SOCKET *s,cFrame *frame);
			//
			static bool CloseSocket(SOCKET *s);

			//*******************UDP**********************************
			static int initSocketUDPListen(ULONG ip,unsigned short port);
			static bool SocketUDPListenAcceptRead(ULONG ip,unsigned short port,SOCKET *s,char *str, int *size);
			static int SocketUDPWrite(ULONG ip,unsigned short port,char *str, int size, bool broadcast);

			//**********Adapters****************** 
			bool printListAdapter(void);
			bool printHostName(void);
			in_addr getListAdapter(unsigned short index);
			unsigned short getListAdapterCount(void);

			//set/get sockOPT on SOCKET
			static bool setSocketNonBlock(SOCKET *socket);

			static int getSocketSendBuf(SOCKET *socket);
			static int getSocketRecvBuf(SOCKET *socket);
			static bool setSocketRecvBuf(SOCKET *socket,int len);
			static bool setSocketSendBuf(SOCKET *socket,int len);
			static bool setSocketRecvTimeOut(SOCKET *socket,DWORD uSec);
			static bool setSocketSendTimeOut(SOCKET *socket,DWORD uSec);
			static bool setSocketTCPNoDelay(SOCKET *socket);
			static bool setSocketUDPBroadcast(SOCKET *s);
			static bool setSocketReuseAddr(SOCKET *s);
			static bool setSocketWaitTime(SOCKET *s,unsigned short timeWait);

			//CRC - cyclic redendant count, message control mechanism
			static bool CheckCRC(char* msg,unsigned int *lenght);
			static void AddCRC(char* msg,unsigned int *lenght);
			static WORD CountCRC(char* msg,unsigned int *lenght);

		private:
			//init/close WIN32 DLL for work with SOCKETS
			bool initWSADLL(void);
			bool cleanWSADLL(void);

			//print last WSA SOCKET ERROR
			void printWSALastError();

			bool initListAdapter(void);

			in_addr listAdapterIp[LIST_ADAPTER_SIZE]; 
			char hostName[HOST_NAME_SIZE];
			bool initListAdapters;
			unsigned short listAdpterCount;
		};
	}
}
#endif
