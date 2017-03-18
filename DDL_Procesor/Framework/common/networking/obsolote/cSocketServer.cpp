#include "cSocketServer.h"

cSocketServer::cSocketServer(char* addr, unsigned short port)
{
    // Initialize Winsock.
    WSADATA wsaData;
    int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
    if ( iResult != NO_ERROR )
        printf("Error at WSAStartup()\n");

    // Create a socket.
    mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if (mSocket == INVALID_SOCKET) 
	{
        printf("Error at socket(): %ld\n", WSAGetLastError());
        WSACleanup();
        return;
		// return;
    }

    // Bind the socket.
    sockaddr_in service;

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(addr);
    service.sin_port = htons(port);

    if (bind(mSocket, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR) 
	{
        printf( "bind() failed.\n" );
        closesocket(mSocket);
		return;
        // return;
    }
}

int cSocketServer::Listen(cListener *listener)
{
	// Listen on the socket.
    if (listen(mSocket, 1) == SOCKET_ERROR)
	{
        printf( "Error listening on socket.\n");
	}

    // Accept connections.
    SOCKET AcceptSocket;

    printf( "Waiting for a client to connect...\n" );
    while (1) {
        AcceptSocket = SOCKET_ERROR;
        while (AcceptSocket == SOCKET_ERROR) 
		{
            AcceptSocket = accept(mSocket, NULL, NULL);
        }
		// mSocket = AcceptSocket;
        printf( "Client Connected.\n");
    
		// Send and receive data.
		printf("%s\n", "Server: Sending Data.");
		mSendString = "";
    
		char str[1024];
		mRecBytes = recv(AcceptSocket, str, 1024, 0);
		// mRecBytes = recv(mSocket, (char*)mRecString.str(), mRecString.Size(), 0);
		printf("Bytes Recv: %ld\n", mRecBytes);

		bool shutdownFlag = true;

		char* sendBytes;
		unsigned int sendBytesLength;

		if (mRecBytes != -1)
		{
			// mRecString[mRecBytes] = '\0';
			// str[mRecBytes] = '\0';
			// mRecString = str;
			// printf("%s\n", mRecString.str()); 
			sendBytes = listener->Execute(str, mRecBytes, sendBytesLength);   // listen to statements
		}

		mSendBytes = send(AcceptSocket, sendBytes, sendBytesLength, 0);
		printf("Bytes Sent: %ld\n", sendBytesLength);

		closesocket(AcceptSocket);

		if (!shutdownFlag)
		{
			break;
		}
    }

	closesocket(mSocket);

    return 0;
}

cSocketServer::~cSocketServer()
{
    WSACleanup();
}