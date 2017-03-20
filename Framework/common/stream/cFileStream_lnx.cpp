namespace common {
	namespace stream {

cFileStream::cFileStream(): mHandle(-1), mStatus(0), mOffset(0)
{
}

cFileStream::~cFileStream()
{
	Close();
}

bool cFileStream::Close()
{
	if (mHandle != -1)
	{
		if (close(mHandle) == -1)
		{
			mStatus = errno;
			printf("cFileStream::Close - close() error with status %d: %s\n", mStatus, strerror(errno));
		}
		mHandle = -1;
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
	uint lnxFlags = TranslateParams(accessMode, createMode, fileFlags);

	if ((mHandle = open(name, lnxFlags, 0666))  == -1)
	{
		mStatus = errno;
		printf("cFileStream::Open - CreateFile() error with status %d: %s\n", mStatus, strerror(errno));
	}
	else
	{
		mStatus = 0;
	}
	return mStatus == 0;
}


uint cFileStream::TranslateParams(const ushort accessMode, const ushort createMode, const ushort fileFlags) const
{
	uint lnxFlags = 0;
	const char* exceptionStr = "cFileStream::TranslateAccessMode: Unsupported Access Mode!";

	if (accessMode == ACCESS_READ)
	{
		// printf("O_RDONLY\n");
		lnxFlags = O_RDONLY;
	}
	else if (accessMode == ACCESS_WRITE)
	{
		// printf("O_WRONLY\n");
		lnxFlags = O_WRONLY;
	}
	else if (ACCESS_READWRITE)
	{
		// printf("O_RDWR\n");
		lnxFlags = O_RDWR;
	}
	else
	{
		throw(exceptionStr);
	}

	if (createMode == FILE_CREATE)
	{
		// printf("O_CREAT\n");
		lnxFlags |= O_CREAT;
	}
	else if (createMode == FILE_OPEN)
	{
	}
	else
	{
		throw(exceptionStr);
	}

	if (fileFlags == DIRECT_IO)
	{
		// printf("O_DIRECT\n");
		// lnxFlags |= O_DIRECT;
	}
	else if (fileFlags == FLAGS_NORMAL)
	{
	}
	else
	{
		throw(exceptionStr);
	}

	return lnxFlags;
}

bool cFileStream::Seek(llong offset, const ushort origin)
{
	int lnxOrigin;

	if (origin == SEEK_BEGIN)
	{
		lnxOrigin = SEEK_SET;
	}
	else
	{
		throw("cFileStream::Seek: Unsupported seek parameter!");
	}

	if (mHandle != -1)
	{
		mStatus = 0;
		// off64_t offset;
		off_t newOffset;

		// if ((offset = lseek64(mHandle, offset, lnxOrigin)) == -1)
		if ((newOffset = lseek(mHandle, (off_t)offset, lnxOrigin)) == -1)
		{
			mStatus = errno;
			printf("Critical Error: cFileStream::Seek(): %d: %s\n", mStatus, strerror(errno));
		}
		else
		{
			mOffset = (llong)newOffset;
		}
	}
	return mStatus == 0;
}

llong cFileStream::GetOffset()
{
	return mOffset;
}

llong cFileStream::GetSize()
{
	llong size = 0;
	if (mHandle != -1)
	{
		// off64_t offset = lseek64(int fd, 0, SEEK_END);
		off_t offset = lseek(mHandle, 0, SEEK_END);

		if (offset == -1)
		{
			mStatus = errno;
		}
		else
		{
			size = (llong)offset;
		}
	}
	return size;
}

int cFileStream::Status(void) const
{
	return mStatus;
}

bool cFileStream::Write(char* buf, const int count, int* buflen)
{
	int tmpBuflen;
	// char alignBuf[count] __attribute__((aligned(8192)));
	// strncpy(alignBuf, buf,  count);

	if (buflen == NULL)
	{
		buflen = &tmpBuflen;
	}

	if (mHandle != - 1)
	{
		if (write(mHandle, buf, count) == -1)
		{
			mStatus = errno;
			printf("Critical Error: cFileStream::Write(): %d: %s\n", mStatus, strerror(errno));
		}
	}

	return mStatus == 0;
}
	
bool cFileStream::Read(char* buf, const int count, int* buflen)
{
	int bytesRead;

	if ((bytesRead = read(mHandle, buf, count)) == -1)
	{
		mStatus = errno;
		printf("Critical Error: cFileStream::Read(): %d\n", mStatus);
	}
	else
	{
		mOffset += count;
		if (bytesRead != count)
		{
			mStatus = -1;
		} else 
		{
			mStatus = 0;
		}
	}
	if (buflen != NULL)
	{
		buflen[0] = bytesRead;
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

bool cFileInputStream::Write(char* buf, const int count, int* buflen)
{
	UNUSED(buf);
	UNUSED(count);
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

bool cFileOutputStream::Read(char* buf, const int count, int* buflen)
{
	UNUSED(buf);
	UNUSED(count);
	UNUSED(buflen);
	throw("cFileOutputStream::Read is not allowed operation!");
}

}}