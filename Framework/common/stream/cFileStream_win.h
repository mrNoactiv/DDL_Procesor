/**
 *	\file cFileStream_win.h
 *	\author Jiøí Dvorský (1998), Michal Krátký (2014)
 *	\version 0.2
 *	\date jan 2014
 *	\brief File streams for win32
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <guiddef.h>
#include <winioctl.h>

namespace common {
	namespace stream {

class cFileStream: public IFileStream
{
protected:
	HANDLE mHandle;
	int mStatus;
	llong mOffset;

	uint TranslateAccessMode(const ushort accessMode) const;
	uint TranslateCreateMode(const ushort createMode)const;
	uint TranslateFileFlags(const ushort fileFlags) const;

public:
	cFileStream();
	~cFileStream();
	virtual bool Open(const char* name, const ushort accessMode, const ushort createMode, const ushort flags = FLAGS_NORMAL);
	bool Close();
	bool Seek(const llong offset, const ushort origin = FILE_BEGIN);
	virtual llong GetOffset();
	virtual llong GetSize();
	inline int Status(void) const;

	virtual bool Write(char* buf, const int size, int* buflen = NULL);
	virtual bool Read(char* buf, const int size, int* buflen = NULL);
	void Print();
	void Print(unsigned int offset, unsigned int n);
	void PrintChar(unsigned int n);

	//bool CopyFrom(cStream *stream);
	//virtual void TurnHWCacheOff();
	static BOOL GetDriveGeometry(DISK_GEOMETRY *pdg);
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
	bool Write(char* buf, const int size, int* buflen = NULL);
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
	bool Read(char* buf, const int size, int* buflen = NULL);
};

}}