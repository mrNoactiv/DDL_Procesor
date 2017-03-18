#include "cTokenInputStream.h"

cTokenInputStream::cTokenInputStream(int bufferSize)
{
	if (bufferSize > 0)
	{
		mCharBuffer = new char[bufferSize+1];     // one char for 0
		mWCharBuffer = new wchar_t[bufferSize+1];
	}
	else
	{
		mCharBuffer = NULL;
		mWCharBuffer = NULL;
	}
	mFile = NULL;
	mBufferWchar = false;
}

cTokenInputStream::cTokenInputStream(const char* InputFilePath)
{
	mCharBuffer = new char[BufferSize];
	mWCharBuffer = new wchar_t[BufferSize];
	mFile = NULL;
	mBufferWchar = false;
	Open(InputFilePath);
}

cTokenInputStream::~cTokenInputStream()
{
	if (mFile != NULL)
	{
		fclose(mFile);
	}
	if (mCharBuffer != NULL)
	{
		delete mCharBuffer;
	}
	if (mWCharBuffer != NULL)
	{
		delete mWCharBuffer;
	}
	mBufferWchar = false;
}

bool cTokenInputStream::Open(const char* InputFilePath)
{
	bool ret = true;
	mBufPtr = 0;
	mBufLen = 0;
	mCharOrder = 0;

	// convert char* to wchar_t*
	wchar_t filename[1024];
	size_t origsize = strlen(InputFilePath) + 1;
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, filename, origsize, InputFilePath, _TRUNCATE);

	// int i = _wfopen_s(&mFile, filename, L"rb,ccs=UTF-8"); // rtS   ccs=UNICODE, ccs=UTF-8 UTF-16LE
	mFile = _wfopen(filename, L"r");
	if (mFile == NULL)
	{
		printf("cTokenInputStream::Open: mFile == NULL\n");
		ret = false;
	}
	else
	{
		// get the file size
		_fseeki64(mFile, 0, SEEK_END);
		mFileSize = _ftelli64(mFile);
		_fseeki64(mFile, 0, SEEK_SET);

		ReadChar();
	}
	return ret;
}

bool cTokenInputStream::Close()
{
	if (mFile != NULL)
	{
		fclose(mFile);
	}
	return true;
}

bool cTokenInputStream::GetToken(wchar_t* Token, const int MaxTokenLength)
{
	Token[0] = 0x0000;
	while (mLastCharType != EndOfFile)
	{
		switch (mLastCharType)
		{
		case TagBegin:
			do
			{
				ReadChar();
			} while (mLastCharType != TagEnd && mLastCharType != EndOfFile);
			break;

		case TagEnd:
			ReadChar();
			break;

		case Others:
			ReadChar();
			break;

		case Alpha:
			Token[0] = mLastChar;
			if (mLastChar == '[')
			{
				int bla = 0;
			}

			int TokenLength = 1;
			while (ReadChar() == Alpha && TokenLength < MaxTokenLength-1)
			{
				Token[TokenLength++] = mLastChar;
			}
			if (TokenLength == 1) // omit empty chars
			{
				continue;
			}
			Token[TokenLength] = 0x0000;
			return true;
			break;
		}
	}
	return false;
}

cTokenInputStream::CharTypes cTokenInputStream::ReadChar()
{
	wint_t wc = ReadCharFromBuffer();
	mCharOrder++;

	if (wc == WEOF)
	{
		mLastCharType = EndOfFile;
		return EndOfFile;
	}
	mLastChar = wc;
	if (mLastChar == L'<')
	{
		mLastCharType = TagBegin;
	}
	else
	{
		if (mLastChar == L'>')
		{
			mLastCharType = TagEnd;
		}
		else
		{
			if (!IsDelimiter(mLastChar))
			{
				mLastCharType = Alpha;
			}
			else
			{
				mLastCharType = Others;
			}

			//if (L'a' <= mLastChar && mLastChar <= L'z' || L'A' <= mLastChar && mLastChar <= L'Z')
			//{
			//	mLastCharType = Alpha;
			//}
			//else
			//{
			//	mLastCharType = Others;
			//}
		}
	}
	return mLastCharType;
}

bool cTokenInputStream::IsDelimiter(wchar_t wc)
{
	const unsigned int delimsCount = 10;
	wchar_t delims[] = {' ', '.', ',', ';', ':', '?', '!', '\n', 13, '\t'};
	bool ret = false;

	for (unsigned int i = 0 ; i < delimsCount ; i++)
	{
		if (delims[i] == wc)
		{
			ret = true;
			break;
		}
	}

	return ret;
}