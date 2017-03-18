/**************************************************************************}
{                                                                          }
{    cSocket.cpp														   }
{                                                                          }
{                                                                          }
{    Copyright (c) 2012                      Ales Nedbalek                 }
{                                                                          }
{    VERSION: 0.1                            DATE 1/10/2012                }
{                                                                          }
{    following functionality:                                              }
{			Sending/receiving stream on TCP,UDP. Set Win32 SOCKETS.        }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      1/10/2012                                                           }
{                                                                          }
{**************************************************************************/

#include "common/networking/cServerDefinition.h"
#include "common/networking/cSocket.h"

using namespace std;

namespace common {
	namespace networking {

		/**
		*	Constructor:
		*	* init WSA DLL(need only once)
		*	* create list Adapter Ips
		*/
		cSocket::cSocket(void)
		{
			if(initWSADLL()){
				//set default ip localhost
				struct in_addr addr;
				addr.S_un.S_addr=inet_addr("127.0.0.1");
				listAdpterCount = 0;
				listAdapterIp[listAdpterCount] = addr;
				initListAdapters = initListAdapter();
			}
		}

		/**
		*	Destructor call cleanWSADLL();
		*/
		cSocket::~cSocket(void){	cleanWSADLL();		}

		/**
		*	Initiate Winsock.dll for using
		*	*init DLL version 2.2
		*/
		bool cSocket::initWSADLL(void)
		{
			WORD wVersionRequested = MAKEWORD(2,2); // Version
			WSADATA data;           // struct with information about lib
			if (WSAStartup(wVersionRequested, &data) != 0)
			{
				cerr << "WSAStartup(Socket) inicialization faild." << endl;
				return false;
			}
			//we dont find usable Winsock DLL
			if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2) {

				cerr <<"Could not find a Winsock.dll"<<endl;
				WSACleanup();
				return false;
			}
			return true;
		}

		/**
		*	Clean up using WSA DLL. 
		*	/return true if is clean correctly.
		*/
		bool cSocket::cleanWSADLL(void){

			int ret = WSACleanup();

			switch(ret){
			case 0: //OK
				return true;
			case WSANOTINITIALISED: cerr<<"cSocket::cleanWSADLL::WSANOTINITIALISED"<<endl;
				return false;
			case WSAENETDOWN: cerr<<"cSocket::cleanWSADLL::WSAENETDOWN"<<endl;
				return false;
			case WSAEINPROGRESS: cerr<<"cSocket::cleanWSADLL::WSAEINPROGRESS"<<endl;
				return false;
			default:
				cerr<<"cSocket::cleanWSADLL::UNKNOWN ERROR number: "<<ret<<endl;
				return false;
			}
		}
		/**
		*	CheckCRC - count CRC Hash an control it with Hash in message
		*	/param pointer to first char in array(with CRC head) 
		*	/param array length
		*	/return true if Hash is correct.
		*/
		bool cSocket::CheckCRC(char* msg,unsigned int *lenght){
			WORD *crc = (WORD*)msg;
			return (*crc == CountCRC(msg,lenght));
		}

		/**
		*	AddCRC - count CRC Hash and write it on firts chars.(free space) 
		*	/param pointer to first char in array(with CRC head) 
		*	/param array length
		*	/return true if is
		*/
		void cSocket::AddCRC(char* msg,unsigned int *lenght){
			WORD crc = CountCRC(msg,lenght);
			memcpy(msg,&crc,SIZE_CRC);
		}

		/**
		*	CountCRC - count CRC hash in array and return it.
		*	/param pointer to first char in array(with CRC head) 
		*	/param array length
		*	/return WORD 
		*/
		WORD cSocket::CountCRC(char* msg,unsigned int *lenght){
			WORD crc = cCRCStatic::GetCRC((BYTE*)(msg+SIZE_CRC),(*lenght-SIZE_CRC),0); 
			return crc;
		}

		/**
		*	Close socket - end of communication on it.
		*	/param pointer to SOCKET
		*	/return true if is close correctly, no WSA ERROR.
		*/
		bool cSocket::CloseSocket(SOCKET *socket)
		{
			int ret = closesocket(*socket);

			switch(ret){
			case 0: //OK
				return true;
			case SOCKET_ERROR: 
				cerr<<"cSocket::CloseSocket::WSAGetLastError:"<<endl;
				//printWSALastError();
				return false;
			default:
				cerr<<"cSocket::CloseSocket::UNKNOWN ERROR"<<endl;
				return false;
			}
		}

		/**
		*	Print last error in WSA. 
		*/
		void cSocket::printWSALastError()
		{
			int ret = WSAGetLastError();

			switch(ret){
			case 0: //OK
				break;
			case WSANOTINITIALISED: 
				cerr<<"WSANOTINITIALISED"<<endl;
				break;
			case WSAENETDOWN: 
				cerr<<"WSAENETDOWN"<<endl;
				break;
			case WSAENOTSOCK: 
				cerr<<"WSAENOTSOCK"<<endl;
				break;
			case WSAEINPROGRESS: 
				cerr<<"WSAEINPROGRESS"<<endl;
				break;
			case WSAEINTR: 
				cerr<<"WSAEINTR"<<endl;
				break;
			case WSAEWOULDBLOCK: 
				cerr<<"WSAEWOULDBLOCK"<<endl;
				break;
			case WSAECONNRESET: 
				cerr<<"WSAECONNRESET"<<endl;
				break;
			case WSAEFAULT: 
				cerr<<"WSAEFAULT"<<endl;
				break;
			case WSAECONNABORTED: 
				cerr<<"WSAECONNABORTED"<<endl;
				break;
			default:
				cerr<<"UNKNOWN ERROR: "<<ret<<endl;
				break;
			}
		}


		/**
		*	Initiate TCP socket for listening on IP:PORT
		*	/param socket IP
		*	/param socket number port
		*	/param setReuseAddr = true
		*	/param setWaitTime = true
		*	/return socket number > 0, else return Error number
		*/
		int cSocket::initSocketTCPListen(ULONG ip,unsigned short port, bool setReuseAddr,bool setWaitTime,bool setTCPNoDelay){
			sockaddr_in sockName;   // server info
			SOCKET main_socket;      // Socket listen

			// create socket
			if ((main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) 
				== INVALID_SOCKET)
			{
				//cerr << "cSocket::initSocketTCPListen::Cannot create socket  TCP." << endl;
				return ERROR_TCP_CREATE;
			}
			//fill struct 
			sockName.sin_family = AF_INET; //AF_UNSPEC
			sockName.sin_port = htons(port);
			//listen on WinSock supply address
			//clientService.sin_addr.s_addr = INADDR_ANY;
			sockName.sin_addr.s_addr = ip;

			if(setReuseAddr)
				if(!setSocketReuseAddr(&main_socket))
					return ERROR_SET_REUSEADDR;

			if(setWaitTime)
				if(!setSocketWaitTime(&main_socket,SOCKET_LINGER_WAIT))
					return ERROR_SET_TIMEWAIT;

			if(!setSocketTCPNoDelay(&main_socket))
				cerr<<"cSocket::initSocketTCPListen::Cant set setSocketTCPNoDelay."<< endl;


			if (::bind(main_socket, (SOCKADDR *)&sockName, sizeof(sockName)) == SOCKET_ERROR)
			{
				//cerr << "cSocket::initSocketTCPListen::Naming TCP socket faild." << endl;
				return ERROR_TCP_BIND;
			}

			if (listen(main_socket,SOCKET_BACKLOG) == SOCKET_ERROR)
			{
				//cerr << "cSocket::initSocketTCPListen::Cannot create front in listen TCP." << endl;
				return ERROR_TCP_ESTABLISH;
			}

			return main_socket;
		}

		/**
		*	Initiate UDP socket for listening on IP:PORT
		*	/param socket IP
		*	/param socket number port
		*	/return socket number > 0, else return Error number
		*/
		int cSocket::initSocketUDPListen(ULONG ip,unsigned short port)
		{
			sockaddr_in sockName;//sock name, port
			SOCKET mainSocket;     
			//create socket
			if ((mainSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
			{
				cerr << "cSocket::initSocketUDPListen:Cannot create socket  UDP." << endl;
				return SOCKET_ERROR;
			}

			sockName.sin_family = AF_INET;
			sockName.sin_port = htons(port);
			sockName.sin_addr.s_addr = ip;

			if (::bind(mainSocket, (sockaddr *)&sockName, sizeof(sockName)) == SOCKET_ERROR)
			{
				cerr << "cSocket::initSocketUDPListen:Naming UDP socket faild." << endl;
				return ERROR_UDP_BIND;
			}

			return mainSocket;
		}

		/**
		*	Initiate TCP socket connection to server IP:PORT
		*	/param server IP
		*	/param server number port
		*	/param setWaitTime = true
		*	/return socket number > 0, else return Error number
		*/
		int cSocket::initSocketTCPWriteRead(ULONG ip,unsigned short port,bool setWaitTime){
			SOCKET main_socket;      // Soket
			// Create socket
			if ((main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
			{
				//cerr << "cSocket::initSocketTCPSend::Cannot create socket." << endl;
				return ERROR_TCP_CREATE;
			}
			// fill struct sockaddr_in
			sockaddr_in clientService;

			clientService.sin_family = AF_INET;
			clientService.sin_addr.s_addr = ip;
			clientService.sin_port = htons(port);

			if (connect(main_socket, (sockaddr *)&clientService, sizeof(clientService)) == SOCKET_ERROR)
			{
				//cerr << "cSocket::initSocketTCPSend::Cannot create connection TCP to :"<< ip << endl;
				return ERROR_TCP_ESTABLISH;
			}

			if(setWaitTime)
				if(!setSocketWaitTime(&main_socket,SOCKET_LINGER_WAIT))
					return ERROR_SET_TIMEWAIT;

			return (int)main_socket;
		}

		/**
		*	Read data from TCP socket.
		*	/param pointer to SOCKET
		*	/param pointer where write stream
		*	/return true if read stream from socket < BUFFER_LEN

		bool cSocket::ReadTCP(SOCKET *s,char *str){
		int len;
		if((len = recv(*s,str, BUFFER_LEN, 0)) != SOCKET_ERROR){
		if(len > BUFFER_LEN){//message is longer then BUFFER_LEN
		//TODO::Read all
		cerr <<"cSocket::ReadTCP::Message is longer then BUFFER_SIZE."<<endl;
		}
		return true;
		}
		return false;
		}*/
		/**
		*	Read data from TCP socket.
		*	/param pointer to SOCKET
		*	/param pointer to cResultSet where write stream
		*	/return true if read stream from socket < BUFFER_SIZE

		bool cSocket::ReadTCP(SOCKET *s,cResultSet *r)
		{
		int len;
		char str[BUFFER_LEN];
		//char *str = new char[BUFFER_LEN];

		if((len = recv(*s,str, BUFFER_LEN, 0)) != SOCKET_ERROR){
		r->Set(str,len);
		return true;
		}
		return false;
		}
		*/

		/**
		*	Write data to TCP socket.
		*	/param pointer to SOCKET
		*	/param pointer to cCommand where read stream
		*	/return true if read stream from socket < BUFFER_SIZE

		bool cSocket::WriteTCP(SOCKET *s,cCommand *c)
		{
		if(send(*s, c->GetChar(),c->GetSize(), 0) != SOCKET_ERROR)
		return true;
		cerr<<"cSocket::WriteTCP Error"<<endl;
		return false;
		}
		*/
		/**
		*	Write data to TCP socket.
		*	/param pointer to SOCKET
		*	/param pointer to stream
		*	/param stream lenght
		*	/return true if read stream from socket < BUFFER_SIZE

		bool cSocket::WriteTCP(SOCKET *s, char *str, int size){

		if(send(*s, str,size, 0) != SOCKET_ERROR)
		return true;
		cerr<<"cSocket::WriteTCP Error"<<endl;
		return false;
		}
		*/

		/**
		*	Write cCommand to TCP socket and read response to cResultSet from TCP connection. Use setSocketTCPNoDelay() for set SOCKET.
		*	/param pointer to SOCKET
		*	/param pointer cCommand
		*	/param pointer cResultSet
		*	/param result stream lenght
		*	/return true if read stream from socket < BUFFER_SIZE
		*/
		bool cSocket::WriteReadTCP(SOCKET *s,cCommand *c,cResultSet *r)
		{
			if(!setSocketTCPNoDelay(s)){
				cerr<<"cSocket::WriteReadTCP::Cant set setSocketTCPNoDelay."<< endl;
			}

			bool res = false;

			if(Send(s,c)){
				if(Recv(s,r)){
					res = true;
				}
			}

			setSocketWaitTime(s,SOCKET_LINGER_WAIT);
			return res;
		}


		/**
		*	Write cCommand to TCP socket and read response to cResultSet from TCP connection. Not use setSocketTCPNoDelay() for set SOCKET.
		*	/param pointer to SOCKET
		*	/param pointer cCommand
		*	/param pointer cResultSet
		*	/param result stream lenght
		*	/return true if read stream from socket < BUFFER_SIZE
		*/
		bool cSocket::WriteReadTCPSERVER(SOCKET *s,cCommand *c,cResultSet *r)
		{
			bool res = false;
			if(Send(s,c)){
				if(Recv(s,r)){
					res = true;
				}
			}

			setSocketWaitTime(s,SOCKET_LINGER_WAIT);
			return res;
		}

		/**
		*	Read stream from UDP - listen on IP:PORT.
		*	/param socket IP
		*	/param socket number port
		*	/param pointer to SOCKET
		*	/param str - pointer read stream
		*	/param size - pointer read stream lenght
		*	/return true if accept stream from socket ok < BUFFER_LEN
		*/
		bool cSocket::SocketUDPListenAcceptRead(ULONG ip,unsigned short port,SOCKET *s,char *str, int *size){
			int slen;
			sockaddr_in sockName;//sock name, port
			sockName.sin_family = AF_INET;
			sockName.sin_port = htons(port);
			sockName.sin_addr.s_addr = ip;

			slen = sizeof(sockName);
			//waiting for recive stream 
			if((*size = recvfrom(*s, str, BUFFER_LEN, 0,(sockaddr *)&sockName,&slen)) != SOCKET_ERROR){
				if(*size > BUFFER_LEN){
					//TODO::Read all
					cerr <<"cSocket::SocketUDPListenAcceptRead::Message is longer then BUFFER_SIZE."<<endl;
				}
				return true;
			}
			return false;
		}


		/**
		*	Accept incoming connection on IP:PORT and return session Socket number.
		*	/param pointer to listen SOCKET
		*	/return new socket session number.
		*/
		int cSocket::SocketTCPListenAccept(SOCKET *s)
		{
			sockaddr_in clientInfo;
			int addrlen = sizeof(clientInfo);
			int size = 0;
			SOCKET newConnectSocket;
			//wait for connection
			newConnectSocket = accept(*s, (sockaddr*)&clientInfo,&addrlen);
			//return socket with client
			if(newConnectSocket==INVALID_SOCKET){//error
				cerr <<"cSocket::SocketTCPListenAccept."<<endl;
				//printWSALastError();
			}
			return newConnectSocket;
		}
		/**
		*	Accept incoming connection on IP:PORT and return session Socket number.
		*	/param pointer to listen SOCKET
		*	/param pointer to sockaddr_in clientInfo
		*	/param pointer to sizeof(clientInfo)
		*	/return new socket session number.
		*/
		int SocketTCPListenAccept(SOCKET *s, sockaddr* addr, int* addrlen){
			return (int)accept(*s, (sockaddr*)addr,addrlen);
		}


		/**
		*	Create UDP socket and write/send on it stream.
		*	/param socket IP
		*	/param socket number port
		*	/param str - pointer write stream
		*	/param size - write stream lenght
		*	/param broadcast - set socket to send stream like broadcast message
		*	/return true if send correctly, else return error number.
		*/
		int cSocket::SocketUDPWrite(ULONG ip,unsigned short port,char *str, int size, bool broadcast)
		{
			SOCKET mainSocket;   
			// create socket
			if ((mainSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
			{
				cerr << "cSocket::SocketUDPWrite:Cannot create socket." << endl;
				return ERROR_UDP_CREATE;
			}

			sockaddr_in clientService;

			clientService.sin_family = AF_INET;
			clientService.sin_addr.s_addr = ip;
			clientService.sin_port = htons(port);

			// this need broadcast packets to be sent
			if(broadcast)
				setSocketUDPBroadcast(&mainSocket);

			int addrlen;
			addrlen = sizeof(clientService);

			if (connect(mainSocket, (sockaddr*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
			{
				cerr << "cSocket::SocketUDPWrite:Cannot take destination adress to connection UDP." << endl;
				return ERROR_UDP_ESTABLISH;
			}

			if(send(mainSocket, str, size , 0)!= SOCKET_ERROR){
				return true;		
			}
			cerr << "cSocket::SocketUDPWrite:Cannot be send."<<endl;
			return ERROR_UNKNOWN;
		}

		/**
		*	Send stream on socket, use SelectSend() for test socket bee free for send. Use While() for sending data on frame = BUFFER_LEN.
		*	/param socket for sending
		*	/param buffer - pointer write stream
		*	/param size - write stream lenght
		*	/return true if send correct, else you need call WSALastError() for get error number. 

		bool cSocket::Send(SOCKET *s,char *buffer, unsigned int size)
		{
		unsigned long pos = 0;
		int sLen;

		while (true)
		{
		switch(SelectSend(s)){
		case SOCKET_ERROR:
		return false;
		break;
		case 0:
		//time limit expired
		break;
		case 1:
		sLen = send(*s,	(char*) (buffer+pos), size, 0);
		switch(sLen){
		case SOCKET_ERROR:

		if(WSAGetLastError()!= WSAEWOULDBLOCK ){
		//printWSALastError();
		return false;
		}
		break;
		case BUFFER_LEN:
		pos += sLen;
		size -= sLen;
		case 0 : 
		default:

		return true;
		};
		default:
		break;
		};
		}
		}
		*/


		/*Send - use Non-Block mechanism TCP. Will count CRC and send char array in cResultSet.
		*	/param - pointer to connected socket with client/server
		*	/param - pointer to object cCommand(child cFrame)
		*	/return - true if send was correct. Other return false on socket error.
		*/
		bool cSocket::Send(SOCKET *s,cCommand *command){return Send(s,(cFrame*)command);}

		/*Send - use Non-Block mechanism TCP. Will count CRC and send char array in cResultSet.
		*	/param - pointer to connected socket with client/server
		*	/param - pointer to object cResultSet(child cFrame)
		*	/return - true if send was correct. Other return false on socket error.
		*/
		bool cSocket::Send(SOCKET *s,cResultSet *resultSet){return Send(s,(cFrame*)resultSet);}

		/*Send - use Non-Block mechanism TCP. Will count CRC and send char array in cResultSet.
		*	/param - pointer to connected socket with client/server
		*	/param - pointer to object cFrame(parent cResultSet, cCommand)
		*	/return - true if send was correct. Other return false on socket error.
		*/
		bool cSocket::Send(SOCKET *s, cFrame *frame)
		{
			unsigned long pos = 0;
			int sLen, sel;
			unsigned int size = 0;
			char* buffer = frame->GetChar();  
			size = frame->GetSize();

			AddCRC(buffer,&size);

			fd_set fdSend;
			struct timeval t ={Time,uTime};
			// Set up the struct timeval for the timeout.
			FD_ZERO(&fdSend);
			FD_SET(*s, &fdSend);
			//FD_SET(STDIN_FILENO, &fdSend);


			while((sel = select( *s+ 1,(fd_set*)0 ,&fdSend, (fd_set*)0, &t ))<1){}

			while(!FD_ISSET(*s, &fdSend)){}
			if(send(*s, (char*)&size,SIZE_INT, 0) == SOCKET_ERROR){
				//cannt sent msg size
				return false;
			}

			while (true)
			{

				switch(select ( *s+ 1,(fd_set*)0, &fdSend, (fd_set*)0, &t )){
				case SOCKET_ERROR:
					//FD_CLR(*s,&fdSend);
					//return false;
					break;
				case 0:
					//time limit expired
					break;
				case 1:
					if(FD_ISSET(*s, &fdSend)){
						sLen = send(*s,	(char*) (buffer+pos), size, 0);
						switch(sLen){
						case SOCKET_ERROR:
							FD_CLR(*s,&fdSend);
							if(WSAGetLastError()!= WSAEWOULDBLOCK ){
								//printWSALastError();
								return false;
							}
							break;
						case BUFFER_LEN:
							pos += sLen;
							size -= sLen;
							break;
						case 0 : 
						default:
							FD_CLR(*s,&fdSend);
							return true;
						};
					}
				default:
					break;
				};
			}
		}

		/**
		*	Recieve stream on socket, use non-Blocking TCP mechanism. Will test incoming stream on CRC Hash an fill cCommand char array.
		*	/param socket for receive
		*	/param pointer to object cResultSet ready for fill from incoming data .
		*	/return true if receive correct. 
		*/
		bool cSocket::Recv(SOCKET *s,cResultSet *resultSet)	{return Recv(s,(cFrame*)resultSet);}

		/**
		*	Recieve stream on socket, use non-Blocking TCP mechanism. Will test incoming stream on CRC Hash an fill cCommand char array.
		*	/param socket for receive
		*	/param pointer to object cCommand ready for fill from incoming data .
		*	/return true if receive correct. 
		*/
		bool cSocket::Recv(SOCKET *s,cCommand *command){	return Recv(s,(cFrame*)command);}

		/**
		*	Recieve stream on socket, use non-Blocking TCP mechanism. Will test incoming stream on CRC Hash an fill cCommand char array.
		*	/param socket for receive
		*	/param pointer to object cFrame ready for fill from incoming data .
		*	/return true if receive correct. 
		*/
		bool cSocket::Recv(SOCKET *s,cFrame *frame)
		{
			unsigned long pos = 0;
			int sLen,sel;
			bool set = false;
			unsigned int MSGsize = 0, msgLen;
			char *buffer;

			fd_set fdRecv;
			struct timeval t ={Time,uTime};
			// Set up the struct timeval for the timeout.
			FD_ZERO(&fdRecv);
			FD_SET(*s, &fdRecv);

			while((sel = select( *s+ 1,&fdRecv,(fd_set*)0 , (fd_set*)0, &t ))<1){}
			while(!FD_ISSET(*s, &fdRecv)){}
			if(recv(*s, (char*)&MSGsize,SIZE_INT, 0) == SOCKET_ERROR){
				//cannt recv msg size
				return false;
			}

			//is not bigger then buffer
			if(MSGsize > frame->GetCapacity()){
				buffer = new char[MSGsize];
				frame->Set(buffer,MSGsize,false);
			}
			else{
				buffer = frame->GetChar();
				frame->SetSize(MSGsize);
			}
			while (true)
			{
				switch(sel = select ( *s+ 1,&fdRecv,(fd_set*)0 , (fd_set*)0, &t )){
				case SOCKET_ERROR:
					FD_CLR(*s,&fdRecv);
					return false;
					break;
				case 0:
					if(CheckCRC(buffer,&MSGsize)){
						frame->ReadHead();
						return true;
					}
					cout<<"Wrong CRC"<<endl;
					return false;
					//time limit expired  or all receive
				case 1:if(FD_ISSET(*s, &fdRecv)){
					sLen = recv(*s, (char *)(buffer + pos), BUFFER_LEN, 0);
					switch(sLen){
					case SOCKET_ERROR:
						FD_CLR(*s,&fdRecv);
						if(WSAGetLastError() != WSAEWOULDBLOCK ){
							//printWSALastError();
							return false;
						}
						break;
					case 0:
						FD_CLR(*s,&fdRecv);
						if(CheckCRC(buffer,&MSGsize)){
							frame->ReadHead();
							return true;
						}
						cout<<"Wrong CRC."<<endl;
						return false;
					case BUFFER_LEN:
					default:
						pos+= sLen;
						if(pos == MSGsize){//all receive
							FD_CLR(*s,&fdRecv);
							if(CheckCRC(buffer,&MSGsize)){
								frame->ReadHead();
								return true;
							}
							cout<<"Wrong CRC."<<endl;
							return false;
						}
						break;
					};
					   }
				default:
					break;
				};
			}
		}

		/**
		*	Test Socket connection - cann i Send?
		*	/param pointer to socket
		*	/return 
		*	* -1 - Socket error
		*	*  0 - time limit expired
		*	*  1 - you cann use send() on socket
		*/
		int cSocket::SelectSend(SOCKET *s){
			fd_set fdSend;
			struct timeval t = {Time,uTime};
			FD_ZERO(&fdSend);
			FD_SET(*s, &fdSend);
			select ( *s+1,(fd_set*)0, &fdSend, (fd_set*)0, &t ) ;
			return FD_ISSET(*s, &fdSend)? 1 : 0;
			//FD_CLR(*s,&fdSend);
		}

		/**
		*	Test Socket connection - cann i Receive?
		*	/param pointer to socket
		*	/return 
		*	* -1 - Socket error
		*	*  0 - time limit expired
		*	*  1 - you cann use recv() on socket
		*/
		int  cSocket::SelectRecv(SOCKET *s){
			fd_set fdRecv;
			struct timeval t = {Time,uTime};
			FD_ZERO(&fdRecv);
			FD_SET(*s, &fdRecv);
			select  ( *s+1,&fdRecv, (fd_set*)0, (fd_set*)0, &t ) ;
			return FD_ISSET(*s, &fdRecv)? 1 : 0;
			//FD_CLR(*s,&fdRecv);
		}

		/**
		*	Print on screan list of usible network adapters.
		*	/return true if have list with network adapters
		*/
		bool cSocket::printListAdapter(void){
			if(initListAdapters){
				for(short i = 0; i <= listAdpterCount; i++){
					cout << "IP Addres  [" << i << "] : " << inet_ntoa(listAdapterIp[i]) << endl;
				}
				return true;
			}
			cerr<<""<<endl;
			return false;
		}

		/**
		*	Print on screan hostName.
		*	/return true if have list with network adapters
		*/
		bool cSocket::printHostName(void){
			if(initListAdapters){
				cout<<hostName<<endl;
				return true;
			}
			cerr<<"No host name."<<endl;
			return false;
		}

		/**
		*	Print on screan hostName.
		*	/param index from list of network adapters
		*	/return struct in_addr with IP(default is localhost IP=127.0.0.1)
		*/
		in_addr cSocket::getListAdapter(unsigned short index){

			if(initListAdapters && index <=listAdpterCount){
				return listAdapterIp[index];
			}
			return listAdapterIp[0];//return default ip - localhost
		}


		/**
		*	Initiate field in_addr[],ListAdapterCount, Host Name, list of network adapters.
		*	/return true if initiate list of adapters was correct.
		*/
		bool cSocket::initListAdapter(void){

			if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR) {
				cerr << "cSocket::initListAdapter:Error getting local host name." << endl;
				printWSALastError();
				return false;
			}

			struct hostent *phe = gethostbyname(hostName);

			if (phe == 0) {
				cerr << "cSocket::initListAdapter:Error, bad host lookup." << endl;
				return false;
			}

			for (short i = 0; phe->h_addr_list[i] != 0; ++i) {
				memcpy(&listAdapterIp[i+1], phe->h_addr_list[i], sizeof(struct in_addr));
				listAdpterCount = i+1;
			}

			return true;
		}


		/**
		*	Number of found network adapters.
		*	/return number of network adapters.
		*/
		unsigned short cSocket::getListAdapterCount(void){
			if(initListAdapters)
				return listAdpterCount;
			return 0;
		}


		//**********SET GET SOCKOPT******************************************
		//*******************************************************************


		/**
		*	Use getsockopt(SO_RCVBUF) on socket to set size of Receive Buffer on network Adapter.
		*	/param pointer to socket
		*	/return optval from OS, if interupt WSA error return NULL.
		*/
		int cSocket::getSocketRecvBuf(SOCKET *socket)
		{
			short iResult;
			int optlen = sizeof(int);
			int optval;
			iResult = getsockopt(*socket, SOL_SOCKET, SO_RCVBUF,(char*) &optval, &optlen);

			if (iResult == SOCKET_ERROR) {
				cerr<<"cSocket::getSocketRecvBuf:Setsockopt for SO_RCVBUF failed."<<endl;
				//printWSALastError();
				return NULL;
			}
			return optval;
		}

		/**
		*	Use getsockopt(SO_SNDBUF) on socket to set size of Send Buffer on network Adapter.
		*	/param pointer to socket
		*	/return optval from OS, if interupt WSA error return NULL.
		*/
		int cSocket::getSocketSendBuf(SOCKET *socket){
			short iResult;

			int optlen = sizeof(int);
			int optval;
			iResult = getsockopt(*socket, SOL_SOCKET, SO_SNDBUF,(char*) &optval, &optlen);

			if (iResult == SOCKET_ERROR) {
				cerr<<"cSocket::getSocketSendBuf:Setsockopt for SO_SNDBUF failed."<<endl;
				//printWSALastError();
				return NULL;
			}
			return optval;
		}

		/**
		*	Use setsockopt(SO_RCVTIMEO) on socket to set SOCKET receive time-out on network Adapter.
		*	/param pointer to socket
		*	/param usec time
		*	/return true if set was accepted.
		*/
		bool cSocket::setSocketRecvTimeOut(SOCKET *socket,DWORD usec)
		{
			short iResult;
			iResult = setsockopt(*socket,SOL_SOCKET, SO_RCVTIMEO,(const char *)&usec,sizeof(usec));
			if (iResult == SOCKET_ERROR) {
				cerr<<"cSocket::setSocketRecvTimeOut:Setsockopt for SO_RCVTIMEO failed."<<endl;
				//printWSALastError();
				return false;
			}
			return true;
		}

		/**
		*	Use setsockopt(SO_SNDTIMEO) on socket to set SOCKET sending time-out on network Adapter.
		*	/param pointer to socket
		*	/param usec time
		*	/return true if set was accepted.
		*/
		bool cSocket::setSocketSendTimeOut(SOCKET *socket,DWORD usec){
			short iResult;
			iResult = setsockopt(*socket,SOL_SOCKET,  SO_SNDTIMEO,(char *)&usec,sizeof(usec));
			if (iResult == SOCKET_ERROR) {
				cerr<<"cSocket::setSocketSendTimeOut:Setsockopt for SO_SNDTIMEO failed."<<endl;
				//printWSALastError();
				return false;
			}
			return true;
		}

		/**
		*	Use setsockopt(SO_BROADCAST) on socket for sending UDP BROADCASTS.
		*	/param pointer to socket
		*	/return true if set was accepted.
		*/
		bool cSocket::setSocketUDPBroadcast(SOCKET *socket){
			char broadcast = '1';
			short iResult;
			// this need broadcast packets to be sent
			iResult = setsockopt(*socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast);

			if (iResult == SOCKET_ERROR) {
				cerr<<"cSocket::setSocketReuseAddr::Setsockopt for SO_REUSEADDR failed."<<endl;
				//printWSALastError();
				return false;
			}
			return true;
		}

		/**
		*	Use setsockopt(SO_REUSEADDR) set reusing port addres after close connection, not wait for lost packets on route.
		*	/param pointer to socket
		*	/return true if set was accepted.
		*/
		bool cSocket::setSocketReuseAddr(SOCKET *socket){

			int optvalONE = 1;
			short iResult;
			//set sock opt for reuse addres immediately 
			iResult = setsockopt(*socket,SOL_SOCKET, SO_REUSEADDR,(char *) &optvalONE, sizeof(int));

			if (iResult == SOCKET_ERROR) {
				cerr<<"cSocket::setSocketReuseAddr::Setsockopt for SO_REUSEADDR failed."<<endl;
				//printWSALastError();
				return false;
			}
			return true;
		}

		/**
		*	Use setsockopt(SO_LINGER) set time between call close connection and execution.
		*	/param pointer to socket
		*	/return true if set was accepted.
		*/
		bool cSocket::setSocketWaitTime(SOCKET *socket,unsigned short timeWait){

			short iResult;
			struct linger so_linger;
			so_linger.l_onoff = TRUE;
			so_linger.l_linger = timeWait;

			iResult = setsockopt(*socket,SOL_SOCKET, SO_LINGER,(char *) &so_linger, sizeof(so_linger));
			if (iResult == SOCKET_ERROR) {
				//cerr<<"cSocket::setSocketWaitTime:Setsockopt for SO_LINGER failed."<<endl;
				//printWSALastError();
				return false;
			}
			return true;
		}


		/*	Set the socket I/O mode: In this case FIONBIO
		*	enables or disables the blocking mode for the 
		*	socket based on the numerical value of iMode.
		*	*If iMode = 0, blocking is enabled; 
		*	*If iMode != 0, non-blocking mode is enabled.
		*	/param pointer to socket
		*	/return true if set was accepted.
		*/
		bool cSocket::setSocketNonBlock(SOCKET *socket){
			short iResult;
			u_long iMode = 1;

			iResult = ioctlsocket(*socket, FIONBIO, &iMode);
			if (iResult == SOCKET_ERROR){
				cerr<<"cSocket::setSocketNonBlock::Setsockopt for FIONBIO failed."<<endl;
				//printWSALastError();
				return false;
			}
			return true;

		}

		/*
		*	TCP_NODELAY is for a specific purpose; to disable the Nagle buffering
		*	algorithm. It should only be set for applications that send frequent
		*	small bursts of information without getting an immediate response,
		*	where timely delivery of data is required (the canonical example is
		*	mouse movements).
		*/
		bool cSocket::setSocketTCPNoDelay(SOCKET *socket){
			short iResult;
			int set = 1;
			iResult = setsockopt(*socket, IPPROTO_TCP, TCP_NODELAY,(char *) &set, sizeof(int));
			if (iResult == SOCKET_ERROR) {
				cerr<<"cSocket::setSocketTCPNoDelay:Setsockopt for TCP_NODELAY failed."<<endl;
				//printWSALastError();
				return false;
			}
			return true;
		}	


	}
}