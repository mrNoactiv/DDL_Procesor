#include "common/stream/cCharStream.h"

namespace common {
	namespace stream {

cCharStream::cCharStream(int capacity): cStream()
{
	m_Array = new char[capacity];
	m_Capacity = capacity;
	m_Size = 0;
	m_Seek = 0;
	m_Lock = false;
}

cCharStream::cCharStream(char *array, int size)
{
	m_Array = new char[CAPACITY];
	memcpy(m_Array, array, size);
	m_Capacity = CAPACITY;
	m_Size = size;
	m_Seek = 0;
	m_Lock = false;
}

cCharStream::~cCharStream(void)
{
	if (m_Array != NULL)
	{
		delete m_Array;
		m_Array = NULL;
	}
}

bool cCharStream::Write(char* buf, const int size, int* buflen)
{
	assert(m_Seek + size <= m_Capacity);

	bool ret = false;
	int tmpSize;

	assert(!m_Lock);

	if (buflen == NULL)
	{
		int tmp;
		buflen = &tmp;
	}

	if (m_Seek > m_Size)
	{
		ret = false;
		*buflen = -1;
	}
	else
	{
		if (m_Seek + size > m_Capacity)
		{
			tmpSize = m_Capacity - m_Seek;
		}
		else
		{
			tmpSize = size;
		}

		if (memcpy(m_Array+m_Seek, buf, tmpSize) != NULL)
		{
			ret = true;
		}

		m_Seek += tmpSize;
		m_Size = m_Seek;
		*buflen = tmpSize;

		if (tmpSize != size)
		{
			ret = false;
		}
	}

	return ret;
}

bool cCharStream::Read(char* buf, const int size, int* buflen)
{
	bool ret = false;

	assert(!m_Lock);

	if (buflen == NULL)
	{
		int tmp;
		buflen = &tmp;
	}

	if (m_Seek >= m_Size)
	{
		ret = false;
		*buflen = -1;
	}
	else
	{
		int tmpSize = size;
		if (m_Seek + size >= m_Size)
		{
			tmpSize = m_Size - m_Seek;
		}

		if (memcpy(buf, m_Array + m_Seek, tmpSize) != NULL)
		{
			ret = true;
		}

		m_Seek += tmpSize;
		*buflen = tmpSize;

		if (tmpSize != size)
		{
			ret = false;
		}
	}
	return ret;
}


/**
 * Write size bytes from char stream into stream.
 */
bool cCharStream::Write(cStream *stream, unsigned int size)
{
	unsigned int sz;

	assert(!m_Lock);
	if (size == cCommon::UNDEFINED_UINT)
	{
		sz = m_Size;
	}
	else
	{
		sz = size;
	}
	assert(size <= m_Capacity);
	bool ret = stream->Write(m_Array, sz);
	return ret;
}

void cCharStream::Print()
{
	printf("\ncCharStream::Print()\n");
	for (unsigned int i = 0 ; i < m_Size ; i++ )
	{
		printf("%d, ", (unsigned char)m_Array[i]);
	}
	printf("\n--------------------------------------\n");
}


void cCharStream::Print(unsigned int offset, unsigned int n)
{
	printf("\ncCharStream::Print()\n");
	for (unsigned int i = 0 ; i < n ; i++ )
	{
		printf("%u, ", *(unsigned int*)(m_Array + offset + i * sizeof(unsigned int)));
	}

	printf("\n--------------------------------------\n");
}


void cCharStream::PrintChar(unsigned int n)
{
	printf("\ncCharStream::Print()\n");
	for (unsigned int i = 0 ; i < n ; i++ )
	{
		printf("%d, ", m_Array[i]);
	}
	printf("\n--------------------------------------\n");
}

}}