#include "common/stream/cStream_lnx.h"

cStream::cStream(): m_handle((HANDLE)NULL), m_Status(0), mSeek(0)
{
}

cStream::~cStream()
{
	Close();
}

bool cStream::Close()
{
	/*
	if (m_handle != (HANDLE)NULL)
	{
		if (!CloseHandle(m_handle))
		{
			m_Status = GetLastError();
		}	
		m_handle = (HANDLE)NULL;
	}
	return m_Status == 0;
	*/
}

bool cStream::Seek(llong pos, /*DWORD*/ unsigned int origin)
{
	/*
	if (m_handle != (HANDLE)NULL)
	{
		LARGE_INTEGER li;
		li.QuadPart = pos;

		if (SetFilePointer(m_handle, li.LowPart, &li.HighPart, origin) == 0xFFFFFFFF)
		{
			m_Status = GetLastError();
		}	
	}
	mSeek = pos;
	return m_Status == 0;
	*/
}

/*DWORD*/ unsigned short cStream::GetPos()
{
	/*
	DWORD Pos = 0;
	if (m_handle != (HANDLE)NULL)
	{
		Pos = SetFilePointer(m_handle, 0, NULL, FILE_CURRENT);
		if (Pos == 0xFFFFFFFF)
		{
			m_Status = GetLastError();
		}
	}
	return Pos;*/
	return 0;
}

/*DWORD*/ unsigned short cStream::GetSize()
{
	/*
	DWORD Size = 0;
	if (m_handle != (HANDLE)NULL)
	{
		Size = GetFileSize(m_handle, NULL);
		if (Size == 0xFFFFFFFF)
		{
			m_Status = GetLastError();
		}
	}
	return Size;
	*/
	return 0;
}

bool cStream::Write(char* buf, const int size, int* buflen)
{
	/*
	UNREFERENCED_PARAMETER(buf);
	UNREFERENCED_PARAMETER(size);
	UNREFERENCED_PARAMETER(buflen);
	*/
	return false;
}
	
bool cStream::Read(char* buf, const int size, int* buflen)
{
	/*
	UNREFERENCED_PARAMETER(buf);
	UNREFERENCED_PARAMETER(size);
	UNREFERENCED_PARAMETER(buflen);
	*/
	return false;
}

bool cStream::CopyFrom(cStream* Stream)
{
	/*bool bRet = true;
	char* buff;
	int buflen = 0;
	int writelen = 1;
	int size = 1024*100;
	buff = new char[size];
	while (writelen != 0)
	{
		bRet = bRet && Stream->Read(buff, size, &buflen);
		if (buflen != 0)
		{
			bRet = bRet && Write(buff, buflen, &writelen);
		}
		else
		{
			writelen = 0;
		}
	}
	delete [] buff;
	return bRet;*/
}

void cStream::Print()
{
}

void cStream::Print(unsigned int offset, unsigned int n)
{
}

void cStream::PrintChar(unsigned int n)
{
}

cInputStream::cInputStream(): cStream()
{
}

cInputStream::cInputStream(const char* AFName, /*DWORD*/ unsigned short mode): cStream()
{
	Open(AFName, mode);
}

/**
 * mode: OPEN_EXISTING, CREATE_ALWAYS, OPEN_ALWAYS and so on.
 */
#ifdef UNICODE
bool cInputStream::Open(WCHAR* AFName, DWORD mode) //bas064
#else
bool cInputStream::Open(const char* AFName, /*DWORD*/ unsigned short mode)
#endif
{
	/*
	m_handle = CreateFile(AFName,
				GENERIC_READ,
				FILE_SHARE_READ,
				(LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING,
				//FILE_FLAG_RANDOM_ACCESS |
				//FILE_FLAG_NO_BUFFERING |
				// FILE_FLAG_OVERLAPPED |
				FILE_ATTRIBUTE_NORMAL | mode,
               (HANDLE)NULL);
	if (m_handle == INVALID_HANDLE_VALUE)
	{
		m_handle = (HANDLE)NULL;
		m_Status = GetLastError();
		printf("cInputStream::Open - CreateFile() error with status %d\n", m_Status);
	}
	else
	{
		m_Status = 0;
	}
	return m_Status == 0;*/
}

bool cInputStream::Read(char* buf, const int size, int* buflen)
{
	/*
	if (buflen == NULL)
	{
		int tmp;
		buflen = &tmp;
	}
	*/

	/* old:
	OVERLAPPED overlap;
	overlap.Offset = mSeek;
	overlap.OffsetHigh = 0;

	if (!ReadFileEx(m_handle, buf, size, &overlap, (LPOVERLAPPED_COMPLETION_ROUTINE)NULL))
	{
		m_Status = GetLastError();
		printf("error: %d, %d\n", m_Status, ERROR_HANDLE_EOF);
	}*/

	/*
	if (!ReadFile(m_handle, buf, size, (LPDWORD)buflen, (LPOVERLAPPED)NULL))
	{
		m_Status = GetLastError();
	}
	else
	{
		mSeek += size;
		if (*buflen != 0)
		{
			m_Status = 0;
		}
		else
		{
			m_Status = -1;
		}
	}

	return m_Status == 0;*/
}

cOutputStream::cOutputStream(): cStream()
{
}

cOutputStream::cOutputStream(const char* AFName, ushort mode): cStream()
{
	Open(AFName, mode);
}

#ifdef UNICODE
bool cOutputStream::Open(WCHAR* AFName, const unsigned short mode, const unsigned short flag)
#else
bool cOutputStream::Open(const char* AFName, const unsigned short mode, const unsigned short flag)
#endif
{
	/*
	m_handle = CreateFile(AFName, 
               GENERIC_WRITE,
               0,
               (LPSECURITY_ATTRIBUTES)NULL,
               mode,
               flag /* FILE_ATTRIBUTE_NORMAL */ /*,
               (HANDLE)NULL);

	if (m_handle == INVALID_HANDLE_VALUE)
	{
		m_handle = (HANDLE)NULL;
		m_Status = GetLastError();
	}
	else
	{
		m_Status = 0;
	}

	return m_Status == 0;*/
}

bool cOutputStream::Write(char* buf, const int size, int* buflen)
{
	/*
	if (buflen == NULL)
	{
		int tmp;
		buflen = &tmp;
	}

	if (!WriteFile(m_handle, buf, size, (LPDWORD)buflen, (LPOVERLAPPED)NULL))
	{
		m_Status = GetLastError();
	}
	return m_Status == 0; */
}

cIOStream::cIOStream(): cStream()
{
}

cIOStream::cIOStream(const char* AFName, ushort mode): cStream()
{
	Open(AFName, mode);
}

#ifdef UNICODE
bool cIOStream::Open(WCHAR* AFName, ushort mode)
#else
bool cIOStream::Open(const char* AFName, ushort mode)
#endif
{
	return Open(AFName, mode, 0 /* FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH */);
	//return Open(AFName, mode, FILE_FLAG_RANDOM_ACCESS);
}

#ifdef UNICODE
bool cIOStream::Open(WCHAR* AFName, ushort mode, const ushort flags)
#else
bool cIOStream::Open(const char* AFName, ushort mode, const ushort flags)
#endif
{
	/*
	m_handle = CreateFile(AFName, 
               GENERIC_WRITE | GENERIC_READ,
               0,
               (LPSECURITY_ATTRIBUTES)NULL,
               mode,
			   flags,
			   //FILE_FLAG_RANDOM_ACCESS,
			   //FILE_FLAG_NO_BUFFERING,
			   //FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH,
			   //FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
               (HANDLE)NULL);

	if (m_handle == INVALID_HANDLE_VALUE)
	{
		m_handle = (HANDLE)NULL;
		m_Status = GetLastError();
	}
	else
	{
		m_Status = 0;
	}
	return m_Status == 0;*/
}


bool cIOStream::Write(char* buf, const int size, int* buflen)
{
	/*
	if (buflen == NULL)
	{
		int tmp;
		buflen = &tmp;
	}

	if (!WriteFile(m_handle, buf, size, (LPDWORD)buflen, (LPOVERLAPPED)NULL))
	{
		m_Status = GetLastError();
	}
	return m_Status == 0;*/
}

bool cIOStream::Read(char* buf, const int size, int* buflen)
{
	/*
	if (buflen == NULL)
	{
		int tmp;
		buflen = &tmp;
	}

	if (!ReadFile(m_handle, buf, size, (LPDWORD)buflen, (LPOVERLAPPED)NULL))
	{
		m_Status = GetLastError();
	}

	if (*buflen != size)  // check the number of bytes read
	{
		m_Status = 1;
	}

	return m_Status == 0; */
}

/*
BOOL cStream::GetDriveGeometry(DISK_GEOMETRY *pdg)
{

  HANDLE hDevice;               // handle to the drive to be examined 
  BOOL bResult;                 // results flag
  DWORD junk;                   // discard results

  hDevice = CreateFile(TEXT("\\\\.\\PhysicalDrive0"),  // drive 
                    0,                // no access to the drive
                    FILE_SHARE_READ | // share mode
                    FILE_SHARE_WRITE, 
                    NULL,             // default security attributes
                    OPEN_EXISTING,    // disposition
                    0,                // file attributes
                    NULL);            // do not copy file attributes

  if (hDevice == INVALID_HANDLE_VALUE) // cannot open the drive
  {
    return (FALSE);
  }

  bResult = DeviceIoControl(hDevice,  // device to be queried
      IOCTL_DISK_GET_DRIVE_GEOMETRY,  // operation to perform
                             NULL, 0, // no input buffer
                            pdg, sizeof(*pdg),     // output buffer
                            &junk,                 // # bytes returned
                            (LPOVERLAPPED) NULL);  // synchronous I/O

  CloseHandle(hDevice);

  return (bResult);
}*/
//
//void cStream::TurnHWCacheOff()
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
