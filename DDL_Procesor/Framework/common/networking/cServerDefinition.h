#ifndef __cServerDefinition_h__
#define __cServerDefinition_h__

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>

//***************TCP,UDP*******************
#define DEFAULT_PORT_TCP	40002
#define UDP_HELLOU			40003

//*************SERVER SOCKET***************
/*use number(10,100,.. 1024) = The maximum length of the pending connections queue.
	0-5000 - is max values to port numbers IP in virtual adapter(virtual server)
	SOMAXCONN, it set maximum reasonable value, SOMAXCONN = 5, sometimes 128.
	*/
#define SOCKET_BACKLOG 1000

/*time in sec, 0 can be good, ad sometime TCP is empty..
	set sock opt for close socket imediately, for reuse
	*/
#define SOCKET_LINGER_WAIT 0

//how many times try resend replicated data to other server
#define MAX_REPLICATE_TRY 20//max is 255
#define REPLICATE_TRY_WAIT 100

//server can return max this number of results
#define SERVER_RESULT 255

//time in milisecond
#define MAX_RESULT_TIME 10000

//time between sendin periodical server Hellous in milisecound
#define TIME_SEND_HELLOUS 1000
		
//define max size char buffer for receive message
#define BUFFER_LEN 1024//1450//1000

//***************THREAD************************
#define S_THREAD 8//Server threads, min = 1

//****************VIEW,SETTINGS ARRAY*******************

const short NAME_LENGTH = 20;//name of server
const short MAX_RESULTS = 100;

//*****************TRAFFIC**************
const unsigned int DEFAULT_TRAFFIC = 1;//def server state trafic 
const short TRAFFIC_TIME_SECOUND = 1;//time after i start recount Traffic t
		

//****************THREAD STRUCTS**************


struct StructServer{
	//init start new thread
	bool startFlag;
	//server tcp thread mutex
	HANDLE hMutex;
	//server udp thread mutex
	HANDLE hMutexUDP;
	//server adres, socket
	SOCKET socketS;
	//pointers
	int pointerDB; 
	int pointerL;
	int pointerNewView;
	int pointerMSocket;
	//init sockets
	int serverTCPListen;
	int serverUDPListen;
	//lock flag
	bool closeListen;
	//flag for send udp broadcast
	short PTypeUDP;
	ULONG PServerIP_UDP;
};

//******************ERRORS****************

#define ERROR_TCP_RESEND_FAIL		-1
#define ERROR_SERVER_NOT_FOUND		-2
#define ERROR_TCP_SEND_TO_SERVER	-3 
#define ERROR_TCP_ESTABLISH			-4
#define ERROR_UDP_ESTABLISH			-5
#define ERROR_TCP_QUERY				-6
#define ERROR_TCP_CREATE			-7 
#define ERROR_UDP_CREATE			-8
#define ERROR_TCP_INSERT			-9
#define ERROR_SERVER_CLOSE			-10
#define ERROR_UNKNOWN				-11
#define ERROR_TCP_BIND				-12
#define ERROR_SET_REUSEADDR			-13
#define ERROR_SET_TIMEWAIT			-14
#define ERROR_UDP_BIND				-15
#define ERROR_UDP_BROADCAST			-16
#define ERROR_UDP_CONNECT			-17
#define ERROR_SET_TCP_DELAY			-18
//MAP
#define ERROR_RECV					-30
#define ERROR_MAP					-31
#define ERROR_MAP_OBJECT			-32
//********************************************
#endif