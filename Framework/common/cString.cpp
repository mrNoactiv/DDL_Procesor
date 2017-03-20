/**************************************************************************}
{                                                                          }
{    cString.cpp																				}
{                                                                          }
{                                                                          }
{                 Copyright (c) 1999, 2002	 				Vaclav Snasel	   }
{                                                                          }
{    VERSION: 1.0														DATE 23/3/1999    }
{                                                                          }
{             following functionality:                                     }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{                                                                          }
{                                                                          }
{**************************************************************************/


#include "cString.h"

cString::cString() 
{
}


cString::cString(const cString& other)
{
	if (this != &other)
	{
		m_str.Move((char*)((const_cast<cString*>(&other))->m_str), other.Length());
	}
}


cString::cString(const char* str)
{
	m_str.Move(str, (unsigned int)strlen(str));
	m_str.Add("\0");
}


cString::cString(const char* str, const int len)
{
	m_str.Move(str, len);
	m_str.Add("\0");
}


const cString& cString::operator = (const cString& other)
{
	if (this != &other)
	{
		m_str.Move((char*)((const_cast<cString*>(&other))->m_str), other.Length());
	}
	return *this;
} 


const cString& cString::operator = (char ch)
{
	m_str.Move((const char*)&ch, 1);
	m_str.Add("\0");
	return *this;
}


const cString& cString::operator = (char* str)
{
	m_str.Move(str, (unsigned int)strlen(str));
	m_str.Add("\0");
	return *this;
}



void cString::Load(cStream* inp, const int BytesPerLength)
{
	unsigned int len;
	inp->Read((char*)&len, BytesPerLength);
	m_str.Resize(len, len);
	inp->Read((char*)m_str, len);
}


void cString::Store(cStream* out, const int BytesPerLength)
{
	int len = Length();
	out->Write((char*)&len, BytesPerLength);
	out->Write((char*)m_str, len);
}



void cString::MakeString(const char ch, const unsigned int Count)
{
	m_str.Fill(ch, Count);
}


void cString::MakeString(const char* Str, const unsigned int Count)
{
	if (m_str.Size() <= Count)
	{
		m_str.Resize(Count, false);
	}
	m_str.ClearCount();
	m_str.Add(Str, Count);	
}



int cString::Compare(const cString& other) const
{
	//int iRet;
	//int l0 = m_str.Count();
	//int l1 = other.m_str.Count();
	//int end = min(l0, l1);
	//
	//char* tmp0 = (char*)(*(const_cast<cArray<char>*>(&m_str)));
	//char* tmp1 = (char*)(*(const_cast<cArray<char>*>(&other.m_str)));

	//for(int i = 0; i < end; i++)
	//{
	//	iRet = weight_table[(char)tmp0[i]] - weight_table[(char)tmp1[i]];
	//	if (iRet != 0)
	//		return iRet;
	//}  
	//return l0 - l1;
	printf("cString::Comparet() Is not implemeted!");
	return 0;
}  


int cString::CompareLeft(const cString& other) const
{
	//int iRet;
	//unsigned int l0 = m_str.Count();
	//unsigned int l1 = min(l0, other.m_str.Count());
	//unsigned int end = min(l0, l1);
	//
	//char* tmp0 = (char*)(*(const_cast<cArray<char>*>(&m_str)));
	//char* tmp1 = (char*)(*(const_cast<cArray<char>*>(&other.m_str)));

	//for(unsigned int i = 0; i < end; i++)
	//{
	//	iRet = weight_table[(char)tmp0[i]] - weight_table[(char)tmp1[i]];
	//	if (iRet != 0)
	//		return iRet;
	//}  
	//return (int)l0 - (int)l1;
	printf("cString::CompareLeft() Is not implemeted!");
	return 0;
}  

int cString::FindLastCharacter(char c)
{
	int len = m_str.Count();

	for (unsigned int i = len - 1; i >= 0; i--)
	{
		if (*(m_str.GetItem(i)) == c)
		{
			return i;
		}
	}
	return -1;
}

void cString::Kamenicky_CP1250()
{
	char* tmp = (char*)m_str;
	int Count = m_str.Count();
	for(int i = 0; i < Count; i++)
	{
		tmp[i] = ::Kamenicky_CP1250(tmp[i]);
	}
}

void cString::Latin2_CP1250()
{
	char* tmp = (char*)m_str;
	int Count = m_str.Count();
	for(int i = 0; i < Count; i++)
	{
		tmp[i] = ::Latin2_CP1250(tmp[i]);
	}
}

void cString::CP1250_Kamenicky()
{
	char* tmp = (char*)m_str;
	int Count = m_str.Count();
	for(int i = 0; i < Count; i++)
	{
		tmp[i] = ::CP1250_Kamenicky(tmp[i]);
	}
}

void cString::CP1250_Latin2()
{
	char* tmp = (char*)m_str;
	int Count = m_str.Count();
	for(int i = 0; i < Count; i++)
	{
		tmp[i] = ::CP1250_Latin2(tmp[i]);
	}
}

void cString::UpCase()
{
	char* tmp = (char*)m_str;
	int Count = m_str.Count();
	for(int i = 0; i < Count; i++)
	{
		tmp[i] = ::UpCase(tmp[i]);
	}
}

void cString::LoCase()
{
	char* tmp = (char*)m_str;
	int Count = m_str.Count();
	for(int i = 0; i < Count; i++)
	{
		tmp[i] = ::LoCase(tmp[i]);
	}
}


void cString::AsCase()
{
	char* tmp = (char*)m_str;
	int Count = m_str.Count();
	for(int i = 0; i < Count; i++)
	{
		tmp[i] = ::AsCase(tmp[i]);
	}
}


int cString::Hash(const int maxhash) const
{
	unsigned int Count = m_str.Count();
	unsigned int h = 0;
	char* tmp = (char*)(*(const_cast<cArray<char>*>(&m_str)));
	
	/*
	for(int i = 0; i < m_str.Count(); i++)
	{
		h = (h*h*h + (unsigned int)tmp[i] - ' ') % maxhash;
	}
	return h;
	*/

/*--- ElfHash ---------------------------------------------------
 *  The published hash algorithm used in the UNIX ELF format
 *  for object files. Accepts a pointer to a string to be hashed
 *  and returns an unsigned long.
 *-------------------------------------------------------------*/

	
	for(unsigned int i = 0; i < Count; i++)
	{
		unsigned long g;
		h = (h << 4) + tmp[i] - ' ';

		g = h & 0xF0000000;
		if (g)
		{
			h ^= g >> 24;
		}
		h &= ~g;
	}
	return h % maxhash;
	
}


void cString::Trim()
{
	int i;
	for(i = 0; i < (int)m_str.Count(); i++)
	{
		if ((unsigned char)m_str[i] > ' ')
		{
			if (i != 0)
				m_str.Right(m_str.Count() - i);
			break;
		}
	}

	for(i = m_str.Count() -1; i >= 0; i--)
	{
		if ((unsigned char)m_str[i] > ' ')
		{
			m_str.Left(i + 1);
			break;
		}
	}
}

void cString::TrimLastSuffix()
{
	for(int i = m_str.Count() - 1; i >= 0; i--)
	{
		if (m_str[i] == '.')
		{
			m_str.SetCount(i);
			return;
		}
	}
}

bool cString::Insert(const cString &str, int firstIndex, int lastIndex)
{
	m_str.ClearCount(); // clear

	if (lastIndex-firstIndex >= (int)m_str.Size())
	{
		return false;
	}

	for (int i = firstIndex ; i < lastIndex ; i++)
	{
		if (i >= (int)str.Length())
		{
			return false;
		}
		*this += str[i];
	}
	*this += '\0';
	return true;
}

unsigned int cString::GetSerialSize()
{
	return m_str.Size();
}

void cString::Write(cStream *stream)
{
	stream->Write(m_str.GetArray(), m_str.Count());
}

void cString::Read(cStream *stream)
{
	stream->Read(m_str.GetArray(), m_str.Count());
}

void cString::Print() const
{
	for (unsigned int i = 0 ; i < m_str.Count() ; i++)
	{
		printf("%c", m_str[i]);
	}
	printf("\n");
	// printf("%s\n", m_str);
}