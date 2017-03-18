#include "cSocket.h"

cSocket::cSocket()
{
	mSendString.Resize(cString::LONGSTR_LENGTH);
	mRecString.Resize(cString::LONGSTR_LENGTH);
}

cSocket::~cSocket()
{
    WSACleanup();
}