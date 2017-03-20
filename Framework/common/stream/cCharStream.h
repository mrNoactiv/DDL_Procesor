/**************************************************************************}
{                                                                          }
{    cCharStream.h                                    		      					 }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001, 2003	   			       Michal Kratky                 }
{                                                                          }
{    VERSION: 0.2														 DATE 20/02/2003               }
{                                                                          }
{    following functionality:                                              }
{       char stream                                                        }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cCharStream_h__
#define __cCharStream_h__

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "common/stream/cStream.h"
#include "common/cCommon.h"

using namespace common;

namespace common {
	namespace stream {

class cCharStream : public cStream
{
private:
	static const int CAPACITY = 32769;
	char *m_Array;
	char *m_TmpArray;
	uint m_Size;
	uint m_Seek;
	uint m_Capacity;
	bool m_Lock;

public:
	cCharStream(int capacity = CAPACITY);
	cCharStream(char* array, int size);
	~cCharStream();

	virtual bool Write(char* buf, const int size, int* buflen = NULL);
	virtual bool Read(char* buf, const int size, int* buflen = NULL);
	bool Read(cStream *stream, unsigned int size);
	bool Write(cStream *stream, unsigned int size = cCommon::UNDEFINED_UINT);
	
	inline bool Seek(const llong offset, const ushort origin = SEEK_BEGIN);
	inline void SeekAdd(const unsigned int pos);
	virtual inline llong GetSize();
	virtual inline llong GetOffset();
	virtual inline uint GetRest() const;
	inline void IncreaseSize(uint size);
	inline void DecreaseSize(uint size);
	inline unsigned int GetCapacity() const;
	inline char *GetCharArray();
	inline void SetCharArray(char* arr, unsigned int size, unsigned int capacity = 0);
	inline void SetDefault();

	inline void LockStream();
	inline void UnlockStream();

	virtual void Print();
	virtual void Print(unsigned int offset, unsigned int n);
	virtual void PrintChar(unsigned int n);
};


inline llong cCharStream::GetSize()
{
	return m_Size;
}

inline llong cCharStream::GetOffset()
{
	return m_Seek;
}

inline uint cCharStream::GetRest() const
{
	return m_Capacity - m_Seek;
}

inline void cCharStream::IncreaseSize(uint size)
{
	assert(!m_Lock);
	m_Size += size;
}

inline void cCharStream::DecreaseSize(uint size)
{
	assert(!m_Lock && m_Size >= size);
	m_Size -= size;
}

inline unsigned int cCharStream::GetCapacity() const
{
	return m_Capacity;
}

inline void cCharStream::SeekAdd(const unsigned int pos)
{
	assert((m_Seek + pos) <= m_Capacity);
	assert(!m_Lock);
	m_Seek += pos;
}

inline bool cCharStream::Seek(const llong pos, const ushort origin)
{
	assert(pos < m_Capacity);
	assert(!m_Lock);
	m_Seek = pos;
	return true;
}

inline char *cCharStream::GetCharArray()
{
	return m_Array + m_Seek;
}


inline void cCharStream::SetCharArray(char* arr, unsigned int size, unsigned int capacity)
{
	m_TmpArray = m_Array;
	m_Array = arr;
	m_Size = size;
	if (capacity != 0)
	{
		m_Capacity = capacity;
	}
}

inline void cCharStream::SetDefault()
{
	m_Array = m_TmpArray;
}

/**
 * Read size bytes from stream into char stream.
 */
inline bool cCharStream::Read(cStream *stream, unsigned int size)
{
	assert(size <= m_Capacity);
	assert(!m_Lock);
	m_Size = size;
	m_Seek = 0;
	bool ret = stream->Read(m_Array, m_Size);
	return ret;
}

inline void cCharStream::LockStream()
{
	m_Lock = true;
}

inline void cCharStream::UnlockStream()
{
	m_Lock = false;
}
}}

#endif