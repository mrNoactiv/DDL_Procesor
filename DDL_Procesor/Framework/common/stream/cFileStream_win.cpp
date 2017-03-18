namespace common {
	namespace stream {

cFileStream::cFileStream(): mHandle((HANDLE)NULL), mStatus(0), mOffset(0)
{
}

cFileStream::~cFileStream()
{
	Close();
}

bool cFileStream::Close()
{
	if (mHandle != (HANDLE)NULL)
	{
		if (!CloseHandle(mHandle))
		{
			mStatus = GetLastError();
		}	
		mHandle = (HANDLE)NULL;
	}
	return mStatus == 0;
}

/**
 * Open the file.
 */
#ifdef UNICODE
bool cFileStream::Open(wchar_t* name, const ushort accessMode, const ushort createMode, const ushort fileFlags)
#else
bool cFileStream::Open(const char* name, const ushort accessMode, const ushort createMode, const ushort fileFlags)
#endif
{
	uint win32AccessMode = TranslateAccessMode(accessMode);
	uint win32CreateMode = TranslateCreateMode(createMode);
	uint win32FileFlags = TranslateFileFlags(fileFlags);

	mHandle = CreateFile(name,
				win32AccessMode,
				FILE_SHARE_READ,
				(LPSECURITY_ATTRIBUTES)NULL,
				win32CreateMode,
				win32FileFlags,
               (HANDLE)NULL);
	if (mHandle == INVALID_HANDLE_VALUE)
	{
		mHandle = (HANDLE)NULL;
		mStatus = GetLastError();
		printf("\ncFileInputStream::Open - CreateFile() error with status %d\n", mStatus);

		const int BUF_SIZE = 256;
		wchar_t buf[BUF_SIZE];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, mStatus, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, BUF_SIZE, NULL);
		printf("Detail Description: %S", buf);
	}
	else
	{
		mStatus = 0;
	}
	return mStatus == 0;
}


uint cFileStream::TranslateAccessMode(const ushort accessMode) const
{
	uint win32AccessMode;

	if (accessMode == ACCESS_READ)
	{
		win32AccessMode = GENERIC_READ;
	}
	else if (accessMode == ACCESS_WRITE)
	{
		win32AccessMode = GENERIC_WRITE;
	}
	else if (ACCESS_READWRITE)
	{
		win32AccessMode = GENERIC_READ | GENERIC_WRITE;
	}
	else
	{
		throw("cFileStream::TranslateAccessMode: Unsupported Access Mode!");
	}
	return win32AccessMode;
}

uint cFileStream::TranslateCreateMode(const ushort createMode) const
{
	uint win32CreateMode;

	if (createMode == FILE_CREATE)
	{
		win32CreateMode = CREATE_ALWAYS;
	}
	else if (createMode == FILE_OPEN)
	{
		win32CreateMode = OPEN_EXISTING;
	}
	else
	{
		throw("cFileStream::TranslateCreateMode: Unsupported Create Mode!");
	}
	return win32CreateMode;
}

uint cFileStream::TranslateFileFlags(const ushort fileFlags) const
{
	uint win32FileFlag;

	if (fileFlags == FLAGS_NORMAL)
	{
		win32FileFlag = FILE_ATTRIBUTE_NORMAL;
	}
	else if (fileFlags == DIRECT_IO)
	{
		win32FileFlag = FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH;
	}
	else if (SEQUENTIAL_SCAN)
	{
		win32FileFlag = FILE_FLAG_SEQUENTIAL_SCAN;
	}
	else
	{
		throw("cFileStream::TranslateFileFlags: Unsupported File Flags!");
	}
	return win32FileFlag;
}

bool cFileStream::Seek(llong offset, const ushort origin)
{
	if (mHandle != (HANDLE)NULL)
	{
		LARGE_INTEGER li;
		li.QuadPart = offset;

		if (SetFilePointer(mHandle, li.LowPart, &li.HighPart, origin) == 0xFFFFFFFF)
		{
			mStatus = GetLastError();
		}	
	}
	mOffset = offset;
	return mStatus == 0;
}

llong cFileStream::GetOffset()
{
	DWORD Pos = 0;
	if (mHandle != (HANDLE)NULL)
	{
		Pos = SetFilePointer(mHandle, 0, NULL, FILE_CURRENT);
		if (Pos == 0xFFFFFFFF)
		{
			mStatus = GetLastError();
		}
	}
	return Pos;
}

llong cFileStream::GetSize()
{
	DWORD Size = 0;
	if (mHandle != (HANDLE)NULL)
	{
		Size = GetFileSize(mHandle, NULL);
		if (Size == 0xFFFFFFFF)
		{
			mStatus = GetLastError();
		}
	}
	return Size;
}

int cFileStream::Status(void) const
{
	return mStatus;
}

bool cFileStream::Write(char* buf, const int size, int* buflen)
{
	if (buflen == NULL)
	{
		int tmp;
		buflen = &tmp;
	}

	if (!WriteFile(mHandle, buf, size, (LPDWORD)buflen, (LPOVERLAPPED)NULL))
	{
		mStatus = GetLastError();
	}
	return mStatus == 0;
}
	
bool cFileStream::Read(char* buf, const int size, int* buflen)
{
	if (buflen == NULL)
	{
		int tmp;
		buflen = &tmp;
	}

	if (!ReadFile(mHandle, buf, size, (LPDWORD)buflen, (LPOVERLAPPED)NULL))
	{
		mStatus = GetLastError();
	}
	else
	{
		mOffset += size;
		if (*buflen != 0)
		{
			mStatus = 0;
		}
		else
		{
			mStatus = -1;
		}
	}

	return mStatus == 0;
}

//bool cFileStream::CopyFrom(cFileStream* Stream)
//{
//	bool bRet = true;
//	char* buff;
//	int buflen = 0;
//	int writelen = 1;
//	int size = 1024*100;
//	buff = new char[size];
//	while (writelen != 0)
//	{
//		bRet = bRet && Stream->Read(buff, size, &buflen);
//		if (buflen != 0)
//		{
//			bRet = bRet && Write(buff, buflen, &writelen);
//		}
//		else
//		{
//			writelen = 0;
//		}
//	}
//	delete [] buff;
//	return bRet;
//}

void cFileStream::Print()
{
}

void cFileStream::Print(unsigned int offset, unsigned int n)
{
}

void cFileStream::PrintChar(unsigned int n)
{
}

cFileInputStream::cFileInputStream(): cFileStream()
{
}

/**
 * mode: OPEN_EXISTING, CREATE_ALWAYS, OPEN_ALWAYS and so on.
 */
#ifdef UNICODE
bool cFileInputStream::Open(wchar_t* name, const ushort fileFlags)
#else
bool cFileInputStream::Open(const char* name, const ushort fileFlags)
#endif
{
	return cFileStream::Open(name, ACCESS_READ, FILE_OPEN, fileFlags);
}

bool cFileInputStream::Write(char* buf, const int size, int* buflen)
{
	UNUSED(buf);
	UNUSED(size);
	UNUSED(buflen);
	throw("cFileInputStream::Write is not allowed operation!");
}

cFileOutputStream::cFileOutputStream(): cFileStream()
{
}

#ifdef UNICODE
bool cFileOutputStream::Open(wchar_t* name, const ushort createMode, const ushort flags)
#else
bool cFileOutputStream::Open(const char* name, const ushort createMode, const ushort fileFlags)
#endif
{
	return cFileStream::Open(name, ACCESS_WRITE, createMode, fileFlags);
}

bool cFileOutputStream::Read(char* buf, const int size, int* buflen)
{
	UNUSED(buf);
	UNUSED(size);
	UNUSED(buflen);
	throw("cFileOutputStream::Read is not allowed operation!");
}

//BOOL cFileStream::GetDriveGeometry(DISK_GEOMETRY *pdg)
//{
//  HANDLE hDevice;               // handle to the drive to be examined 
//  BOOL bResult;                 // results flag
//  DWORD junk;                   // discard results
//
//  hDevice = CreateFile(TEXT("\\\\.\\PhysicalDrive0"),  // drive 
//                    0,                // no access to the drive
//                    FILE_SHARE_READ | // share mode
//                    FILE_SHARE_WRITE, 
//                    NULL,             // default security attributes
//                    OPEN_EXISTING,    // disposition
//                    0,                // file attributes
//                    NULL);            // do not copy file attributes
//
//  if (hDevice == INVALID_HANDLE_VALUE) // cannot open the drive
//  {
//    return (FALSE);
//  }
//
//  bResult = DeviceIoControl(hDevice,  // device to be queried
//      IOCTL_DISK_GET_DRIVE_GEOMETRY,  // operation to perform
//                             NULL, 0, // no input buffer
//                            pdg, sizeof(*pdg),     // output buffer
//                            &junk,                 // # bytes returned
//                            (LPOVERLAPPED) NULL);  // synchronous I/O
//
//  CloseHandle(hDevice);
//
//  return (bResult);
//}
//
//void cFileStream::TurnHWCacheOff()
//{
//	DISK_CACHE_INFORMATION info;                 
//	DWORD returned;
//	HANDLE hDevice = CreateFile("\\\\.\\C:",  // drive to open
//                  GENERIC_READ,                      FILE_SHARE_READ | // share mode
//                  FILE_SHARE_WRITE,
//                  NULL,             // default security attributes
//                  OPEN_EXISTING,    // disposition
//                  0,                // file attributes
//                  NULL);            // do not copy file attributes
//	DWORD err;
//
//	if (!DeviceIoControl(hDevice, IOCTL_DISK_GET_CACHE_INFORMATION, NULL, 0, (LPVOID)&info, (DWORD)sizeof(info), (LPDWORD)&returned, (LPOVERLAPPED)NULL))
//	{
//		err = GetLastError();
//	}
//	CloseHandle(hDevice);
//
//	info.WriteCacheEnabled = true;
//	info.ReadCacheEnabled = false;
//
//	DISK_CACHE_INFORMATION newInfo;              
//	hDevice = CreateFile("\\\\.\\C:",  // drive to open
//                  GENERIC_READ | GENERIC_WRITE,                      FILE_SHARE_READ | // share mode
//                  FILE_SHARE_WRITE,
//                  NULL,             // default security attributes
//                  OPEN_EXISTING,    // disposition
//                  0,                // file attributes
//                  NULL);            // do not copy file attributes
//
//
//	if (!DeviceIoControl(hDevice, IOCTL_DISK_SET_CACHE_INFORMATION, (LPVOID)&info, (DWORD)sizeof(info), NULL, 0, (LPDWORD)&returned, (LPOVERLAPPED)NULL))
//	{
//		err = GetLastError();
//	}
//
//	CloseHandle(hDevice);
//	hDevice = CreateFile("\\\\.\\C:",  // drive to open
//                  GENERIC_READ,                      FILE_SHARE_READ | // share mode
//                  FILE_SHARE_WRITE,
//                  NULL,             // default security attributes
//                  OPEN_EXISTING,    // disposition
//                  0,                // file attributes
//                  NULL);            // do not copy file attributes
//
// 
//	if (!DeviceIoControl(hDevice, IOCTL_DISK_GET_CACHE_INFORMATION, NULL, 0, (LPVOID)&info, (DWORD)sizeof(info), (LPDWORD)&returned, (LPOVERLAPPED)NULL))
//	{
//		err = GetLastError();
//	}
//	CloseHandle(hDevice);
//}

}}