/**
 *	\file cFileStream_lnx.h
 *	\author Michal Krátký
 *	\version 0.2
 *	\date jan 2014
 *	\brief File streams for Linux
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// typedef unsigned int tNodeIndex;

namespace common {
	namespace stream {

class cFileStream: public IFileStream
{
protected:
	int mHandle;
	int mStatus;
	llong mOffset;

	uint TranslateParams(const ushort accessMode, const ushort createMode, const ushort fileFlags) const;

public:
	cFileStream();
	~cFileStream();
	virtual bool Open(const char* name, const ushort accessMode, const ushort createMode, const ushort flags = FLAGS_NORMAL);
	bool Close();
	bool Seek(const llong offset, const ushort origin = FILE_OPEN);
	virtual llong GetOffset();
	virtual llong GetSize();
	inline int Status(void) const;

	virtual bool Write(char* buf, const int count, int* buflen = NULL);
	virtual bool Read(char* buf, const int count, int* buflen = NULL);
	void Print();
	void Print(unsigned int offset, unsigned int n);
	void PrintChar(unsigned int n);

	//bool CopyFrom(cStream *stream);
};

class cFileInputStream: public cFileStream
{
public:
	cFileInputStream();
#ifdef UNICODE
	bool Open(wchar_t* name, const ushort flags = FLAGS_NORMAL);
#else
	bool Open(const char* name, const ushort flags = FLAGS_NORMAL);
#endif
	bool Write(char* buf, const int count, int* buflen = NULL);
};

class cFileOutputStream : public cFileStream
{
public:
	cFileOutputStream();
#ifdef UNICODE
	bool Open(wchar_t* name, const ushort mode = FILE_CREATE, const ushort flags = FLAGS_NORMAL);
#else
	bool Open(const char* name, const ushort mode = FILE_CREATE, const ushort flags = FLAGS_NORMAL);
#endif
	bool Read(char* buf, const int count, int* buflen = NULL);
};

}}
