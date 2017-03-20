/**
 *	\file cTokenInputStream.h
 *	\author Jiri Dvorsky
 *	\version 0.1
 *	\date mar 2011
 *	\brief Token Input Stream
 */

#pragma once

#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

class cTokenInputStream
{
public:
	cTokenInputStream(const char* InputFilePath);
	cTokenInputStream(int bufferSize = BufferSize);
	~cTokenInputStream();

	bool GetToken(wchar_t* Token, const int MaxTokenLength);

	bool Open(const char* InputFilePath);
	bool Close();

	inline long GetPos() const;
	inline float GetPosPercent() const;
	inline int GetCharOrder() const;

	inline void SetBuffer(wchar_t* buffer, int size);

private:
	bool IsDelimiter(wchar_t wc);

private:
	enum CharTypes
	{
		TagBegin,
		TagEnd,
		Alpha,
		Others,
		EndOfFile
	};

	CharTypes ReadChar();
	wint_t ReadCharFromBuffer();

	static const int BufferSize = 64*1024;

	wchar_t mLastChar;
	CharTypes mLastCharType;

	FILE* mFile;
	char *mCharBuffer;
	wchar_t *mWCharBuffer;
	int mBufPtr;
	int mBufLen;
	__int64 mFileSize;
	int mCharOrder;
	bool mBufferWchar;   // if it is true, if SetBuffer is called
};  // TokenInputStream

inline wint_t cTokenInputStream::ReadCharFromBuffer()
{
	wint_t ch = WEOF;

	if (mBufPtr < mBufLen)
	{
		ch = (wint_t)mWCharBuffer[mBufPtr++];
	}
	else 
	{
		if (!mBufferWchar)
		{
			mBufLen = fread(mCharBuffer, 1, BufferSize, mFile);
			mCharBuffer[mBufLen] = '\0';

			if (mBufLen != 0)
			{
				mBufLen = ::MultiByteToWideChar(CP_UTF8, 0, mCharBuffer, -1, mWCharBuffer, BufferSize);
				mBufLen--; // the last byte is the 0 delim
				mBufPtr = 1;
				ch = (wint_t)mWCharBuffer[0];
			}
		}
	}

	return ch;
}

inline long cTokenInputStream::GetPos() const
{
	return _ftelli64(mFile);
}

inline float cTokenInputStream::GetPosPercent() const
{
	float res = 0.0;
	if (mFileSize != 0)
	{
		res = ((double)_ftelli64(mFile)/mFileSize)*100.0;
	}
	return res;
}

inline int cTokenInputStream::GetCharOrder() const
{
	return mCharOrder;
}

/**
 * The content of the input stream is the memory document. To proper destruction of this object
 * it is necessary to call SetBuffer(NULL);
 */
inline void cTokenInputStream::SetBuffer(wchar_t* buffer, int size)
{
	if (mWCharBuffer != NULL && !mBufferWchar)
	{
		delete mWCharBuffer;
	}
	mWCharBuffer = buffer;
	mFile = NULL;

	mBufLen = size;
	mBufPtr = 0;
	mBufferWchar = true;

	if (mWCharBuffer != NULL)
	{
		ReadChar();  // it is necessary for GetToken
	}
}

