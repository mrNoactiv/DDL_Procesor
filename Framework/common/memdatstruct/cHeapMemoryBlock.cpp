#include "common/memdatstruct/cHeapMemoryBlock.h"

namespace common {
	namespace memdatstruct {

/// <summary>Konstruktor triedy cHeapMemoryBlock, neprijma ziaden parameter a inicializuje velkost bloku na halde na 0.</summary>
cHeapMemoryBlock::cHeapMemoryBlock()
{
	size = 0;
}


/// <summary>Destruktor triedy cHeapMemoryBlock</summary>
cHeapMemoryBlock::~cHeapMemoryBlock(void)
{
	if(size > 0)
	{
		delete block_memory;
	}
}


/// <summary>Metoda Init() prijma jeden parameter a inicualizuje velkost bloku na hlade na zaklade parametru metody. Nasledne alokuje na halde
/// pole typu char o velkosti size.</summary>
/// <param name="size">Definuje velkost pola typu char, ktore sa bude alokovat na halde</param>
/// <returns>Vracia void</returns>
void cHeapMemoryBlock::Init(unsigned int size)
{
	this->size = size;
	this->block_memory = new char[size];
}


/// <summary>Metoda GetSize() vracia velkost pola.</summary>
/// <returns>Vracia int</returns>
int cHeapMemoryBlock::GetSize(void)
{
	return this->size;
}


/// <summary>Metoda GetBlock_memory() vracia pole typu char alokovane na halde.</summary>
/// <returns>char *</returns>
char * cHeapMemoryBlock::GetBlock_memory(void)
{
	return this->block_memory;
}
}}