/**************************************************************************}
{                                                                          }
{    cString.h                                 		      						}
{                                                                          }
{                                                                          }
{                 Copyright (c) 1998, 2002	   			Vaclav Snasel     }
{                                                                          }
{    VERSION: 1.0														DATE 5/10/1998    }
{                                                                          }
{             following functionality:                                     }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{    29.8.1999 JD Add MakeString and other operator +=                     }
{                                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __CSTRING_H__
#define __CSTRING_H__

#include <stdio.h>
#include <stdlib.h>

#include "common/czech.h"
#include "common/memorystructures/cArray.h"
#include "common/stream/cStream.h"

using namespace common::stream;

class cString
{
public:
	static const int STR_LENGTH = 128;
	static const int LONGSTR_LENGTH = 1024;

public:
	cString();
	cString(const cString& other);
	cString(const char* str);
	cString(const char* str, const int len);
	const cString& operator = (const cString& other);
	const cString& operator = (char ch);
	const cString& operator = (char* str);

	void Load(cStream* inp, const int BytesPerLength = 4);
	void Store(cStream* out, const int BytesPerLength = 4);

	inline const char* str(void);
	inline operator const char* (void);
	inline operator char* (void);
	inline char& operator[](const int index);
	inline const char operator[](const int index) const;
	inline bool IsEmpty() const;
	inline void Clear();
	inline unsigned int Length() const;
	inline void ClearCount();
	void MakeString(const char ch, const unsigned int Count);
	void MakeString(const char* Str, const unsigned int Count);
	inline const cString& operator += (const char ch);
	inline const cString& operator += (const unsigned int ch);
	inline const cString& operator += (char* str);	
	inline const cString& operator += (const cString& other);

	inline void Move(const char* Source, const int Count);

	int Compare(const cString& other) const;
	int CompareLeft(const cString& other) const;
	int FindLastCharacter(char c);

	bool operator == (const char* str) const;
	bool operator == (const cString& other) const;
	bool operator != (const cString& other) const;
	bool operator >  (const cString& other) const;
	bool operator >= (const cString& other) const;
	bool operator <  (const cString& other) const;
	bool operator <= (const cString& other) const;

	inline void Left(const int count);
	inline void Right(const int count);
	inline void Mid(const unsigned int Left, const unsigned int Right);

	void Kamenicky_CP1250();
	void Latin2_CP1250();
	void CP1250_Kamenicky();
	void CP1250_Latin2();
	void UpCase();
	void LoCase();
	void AsCase();

	int Hash(const int maxhash) const;
	void Trim();
	void TrimLastSuffix();
	inline void Resize(const unsigned int Size);
	inline unsigned int Count(void);
	inline void SetCount(unsigned int count);
	inline unsigned int Size(void);

	bool Insert(const cString &str, int firstIndex, int lastIndex);

	unsigned int GetSerialSize();
	void Write(cStream *stream);
	void Read(cStream *stream);

	void Print() const;

protected:
	
	cArray<char> m_str;
};

char& cString::operator[](const int index)
{
	return m_str[index];
}

const char cString::operator[](const int index) const
{
	return m_str[index];
}


inline const char* cString::str(void)
{
	return (const char* )m_str;
}

inline cString::operator const char* (void)
{
	m_str.Append(0);
	return (const char* )m_str;
}


inline cString::operator char* (void)
{
	m_str.Append(0);
	return (char* )m_str;
}

inline bool cString::IsEmpty() const
{
	return m_str.Count() == 0;
}


inline void cString::Clear()
{
	m_str.Clear();
}

inline unsigned int cString::Length() const
{
	return m_str.Count();
}

inline void cString::ClearCount()
{
	m_str.ClearCount();
}

inline const cString& cString::operator += (const char ch)
{
	m_str.Add((const char*)&ch);
	return *this;
}

inline const cString& cString::operator += (const unsigned int val)
{
	char number[10];
	sprintf(number,"%u",val);
	m_str.Add(number);
	return *this;
}

inline const cString& cString::operator += (char* str)
{
	if (m_str.Count() > 0 && m_str[m_str.Count() - 1] == '\0')
	{
		m_str.SetCount(m_str.Count() - 1);
	}
	m_str.Add(str, (unsigned int)strlen(str));	
	m_str.Add("\0");
	return *this;
}

inline const cString& cString::operator += (const cString& other)
{
	if (other.Length() > 0)
	{
		if (m_str.Count() > 0 && m_str[m_str.Count() - 1] == '\0')
		{
			m_str.SetCount(m_str.Count() - 1);
		}
		m_str.Add(const_cast<cArray<char>*>(&other.m_str)->GetArray(), other.Length());
	}
	return *this;
}

inline void cString::Move(const char* Source, const int Count)
{
	m_str.Move(Source, Count);
}

inline bool cString::operator == (const char* str) const
{

	int iRet;
	int l0 = m_str.Count() - 1;
	int l1 = strlen(str);
	char* tmp0 = (char*)(*(const_cast<cArray<char>*>(&m_str)));

	if (l0 != l1)
	{
		return false;
	}

	for(int i = 0; i < l0; i++)
	{
		iRet = weight_table[(char)tmp0[i]] - weight_table[(char)str[i]];
		if (iRet != 0)
		{
			return false;
		}
	}  
	return true;
}

inline bool cString::operator == (const cString& other) const
{ 
	return Compare(other) == 0;
}

inline bool cString::operator != (const cString& other) const
{ 
	return Compare(other) != 0;
}

inline bool cString::operator >  (const cString& other) const
{ 
	return Compare(other) > 0;
}

inline bool cString::operator >= (const cString& other) const
{ 
	return Compare(other) >= 0;
}

inline bool cString::operator <  (const cString& other) const
{ 
	return Compare(other) < 0;
}

inline bool cString::operator <= (const cString& other) const
{ 
	return Compare(other) <= 0;
}

inline void cString::Left(const int count)
{
	m_str.Left(count);
}


inline void cString::Right(const int count)
{
	m_str.Right(count);
}


inline void cString::Mid(const unsigned int left, const unsigned int right)
{
	m_str.Mid(left, right);
}

inline void cString::Resize(const unsigned int Size)
{
	m_str.Resize(Size, false);
}


inline unsigned int cString::Count(void)
{
	return m_str.Count();
}

inline void cString::SetCount(unsigned int count)
{
	m_str.SetCount(count);
}

inline unsigned int cString::Size(void)
{
	return m_str.Size();
}
#endif  // __CSTRING_H__
