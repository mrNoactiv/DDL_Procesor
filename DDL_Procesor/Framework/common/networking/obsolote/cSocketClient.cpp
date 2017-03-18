#include "cSocketClient.h"

cSocketClient::cSocketClient()
{
    // Initialize Winsock.
    WSADATA wsaData;
    int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
    if ( iResult != NO_ERROR )
	{
        printf("Error at WSAStartup()\n");
	}

    // Create a socket.
    mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (mSocket == INVALID_SOCKET) 
	{
        printf("Error at socket(): %ld\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Connect to a server.
    sockaddr_in clientService;

    clientService.sin_family = AF_INET;
    // clientService.sin_addr.s_addr = inet_addr("158.196.157.9");
	clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientService.sin_port = htons(40001);

    if (connect(mSocket, (SOCKADDR*) &clientService, sizeof(clientService)) == SOCKET_ERROR)
	{
        printf( "Failed to connect.\n" );
        WSACleanup();
        return;
    }
}

cSocketClient::~cSocketClient()
{
    WSACleanup();
}

int cSocketClient::SendReceive()
{
    // Send and receive data.
    int bytesSent;
    int bytesRecv = SOCKET_ERROR;
	char sendbuf[32] = "q:titles/title";
    char recvbuf[1024] = "";

    bytesSent = send(mSocket, sendbuf, strlen(sendbuf), 0);
    printf("Bytes Sent: %ld\n", bytesSent);
    printf("%s\n", sendbuf);

    /*if ( bytesRecv == 0 || 
        (bytesRecv == SOCKET_ERROR && 
         WSAGetLastError()== WSAECONNRESET ))
    {*/
	//while(true)
	//{
        bytesRecv = recv(mSocket, recvbuf, 1024, 0);
        if (bytesRecv == -1)
        { 
			//continue;
            printf( "Connection Closed.\n");
            return 0;
        }
        if (bytesRecv < 0)
		{
            return 0;
		}
        printf( "Bytes Recv: %ld\n", bytesRecv);
		recvbuf[1024] = '\0';
		// printf("%s\n", recvbuf); 
		for (unsigned int i = 0 ; i < 1024 ; i++)
		{
			printf("%c", recvbuf[i]);
		}
		
    // }

    return 0;
}