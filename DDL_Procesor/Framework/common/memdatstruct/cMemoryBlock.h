#pragma once

#include <stddef.h>

namespace common {
	namespace memdatstruct {

//#define TEST_CORRECTNESS

class cMemoryBlock
{
public:
	cMemoryBlock(void) {}
	/// <summary>Konstruktor triedy cMemoryBlock prijme 4 parametre a inicializuje triedne premenne.</summary>
	/// <param name="pointer_position">Pozicia ukazovatela v poli mangementu blokov. Udaj identifikuje cMemoryBlock</param>
	/// <param name="sign_size">Priznak velkosti bloku: S,B,T</param>
	/// <param name="block_size">Velkost bloku. Berie sa z MM z inicializacnych hodnot S,B,T</param>
	/// <param name="pt_memaddr_begin">Ukazovatel na pociatocnu adresu bloku v hlavnej pamati</param>
	cMemoryBlock(int pointer_position, char sign_size, unsigned int block_size, char * pt_memaddr_begin) 
	{
		this->pointer_position = pointer_position;
		this->sign_size = sign_size;
		this->block_size = block_size;
		mIsFree = true;
		this->pt_memaddr_begin = pt_memaddr_begin;
		this->pt_memaddr_end = pt_memaddr_begin + (this->block_size-1);
		this->pt_memaddr = pt_memaddr_begin;

		#ifdef TEST_CORRECTNESS
		this->ownerThreadID = -1;
		#endif
	}
	~cMemoryBlock(void)
	{
		this->pt_memaddr_begin = 0;
		this->pt_memaddr_end = 0;
		this->pt_memaddr = 0;
	}

	inline bool Release_block(void);
	inline int Get_pointer_position(void) { return pointer_position; }
	inline char Get_SignSize(void);
	inline unsigned int GetBlock_size(void);
	inline bool Get_is_free(void) { return mIsFree; }
	inline void Set_is_free(bool is_free) { mIsFree = is_free; }
	inline char* GetMem();
	inline void SetMemAddress(char * pt_memaddr);
	inline char* GetMemAddressBegin(void);
	inline void SetMemAddressBegin(char * pt_memaddr_begin);
	inline char * GetMemAddressEnd(void);
	inline void SetMemAddressEnd(char * pt_memaddr_end);
	inline char * GetMemory(int memoryuse_size);
	inline void Clear(void);
	inline cMemoryBlock* GetPrevious();
	inline void SetPrevious(cMemoryBlock* prev);

	#ifdef TEST_CORRECTNESS
	void Set_ownerThreadID(int threadID);
	int Get_ownerThreadID(void);
	#endif

private:
	int pointer_position;
	char sign_size;					// priznak velkosti bloku SMALL: 'S', BIG: 'B'
	unsigned int block_size;
	bool mIsFree;
	char * pt_memaddr;
	char * pt_memaddr_begin;
	char * pt_memaddr_end;
	cMemoryBlock* mPrevious;

	#ifdef TEST_CORRECTNESS
	int ownerThreadID;
	#endif
};

/// <summary>Metoda GetMemAddressBegin vracia ukazovatel na zaciatok pamatoveho bloku v hlavnej pamati.</summary>
/// <returns>Vracia char *</returns>
char* cMemoryBlock::GetMem()
{
	return pt_memaddr;
}

/// <summary>Metoda GetMemAddressBegin vracia ukazovatel na zaciatok pamatoveho bloku v hlavnej pamati.</summary>
/// <returns>Vracia char *</returns>
char* cMemoryBlock::GetMemAddressBegin()
{
	return pt_memaddr_begin;
}

	

/// <summary>Metoda Releasa_block nastavuje premennu is_free na TRUE, ci sa oznacuje blok pamate za volny.</summary>
/// <returns>Vracia bool</returns>
bool cMemoryBlock::Release_block()
{
	mIsFree = true;

	return true;
}


/// <summary>Metoda Get_SignSize vracia priznak velkosti bloku: S,B,T</summary>
/// <returns>Vracia char</returns>
char cMemoryBlock::Get_SignSize()
{
	return sign_size;
}


/// <summary>Metoda GetBlock_size vracia velkost bloku v bytoch. Vychadza z priznaku velkosti bloku S,B,T.</summary>
/// <returns>Vracia unsigned int</returns>
unsigned int cMemoryBlock::GetBlock_size()
{
	return block_size;
}



/// <summary>Metoda SetMemAddress nastavuje ukazovatel bloku v hlavnej pamati.</summary>
/// <param name="pt_memaddr_begin">Ukazovatel pamatoveho bloku v hlavnej pamati</param>
/// <returns>Vracia void</returns>
void cMemoryBlock::SetMemAddress(char * pt_memaddr)
{
	this->pt_memaddr = pt_memaddr;
}

/// <summary>Metoda SetMemAddressBegin nastavuje ukazovatel zaciatku bloku v hlavnej pamati.</summary>
/// <param name="pt_memaddr_begin">Ukazovatel zaciatku pamatoveho bloku v hlavnej pamati</param>
/// <returns>Vracia void</returns>
void cMemoryBlock::SetMemAddressBegin(char * pt_memaddr_begin)
{
	this->pt_memaddr_begin = pt_memaddr_begin;
}


/// <summary>Metoda GetMemAddressEnd vracia ukazovatel na koniec pamatoveho bloku v hlavnej pamati.</summary>
/// <returns>Vracia char *</returns>
char * cMemoryBlock::GetMemAddressEnd()
{
	return pt_memaddr_end;
}


/// <summary>Metoda SetMemAddressEnd nastavuje ukazovatel konca bloku v hlavnej pamati.</summary>
/// <param name="pt_memaddr_end">Ukazovatel konca pamatoveho bloku v hlavnej pamati</param>
/// <returns>Vracia void</returns>
void cMemoryBlock::SetMemAddressEnd(char * pt_memaddr_end)
{
	this->pt_memaddr_end = pt_memaddr_end;
}

/// <summary>Metoda Clear rusi poziciu ukazovatela pociatku nevyuzitej pamate bloku. Ukazovatel sa nastavi na pociatocnu adresu pamatoveho bloku.</summary>
/// <returns>Vracia void</returns>
void cMemoryBlock::Clear()
{
	this->pt_memaddr = this->pt_memaddr_begin;
}

/**
*<summary>Method returns a memory block of specified size. It returns NULL if there is not enough space.</summary>
* <returns>Return NULL if there is not enough space in the block</returns>
*/
char * cMemoryBlock::GetMemory(int memory_size)
{
	if(pt_memaddr+memory_size < pt_memaddr_end)
	{
		char * pt_tmp = pt_memaddr;
		pt_memaddr += memory_size;					// posuniem ukazovatel v bloku pamati
		return pt_tmp;
	}
	else
	{
		return NULL;
	}
}

cMemoryBlock* cMemoryBlock::GetPrevious()
{
	return mPrevious;
}

void cMemoryBlock::SetPrevious(cMemoryBlock* prev)
{
	mPrevious = prev;
}

}}

