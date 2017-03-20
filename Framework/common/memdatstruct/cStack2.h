#pragma once

#include "common/memdatstruct/cMemoryManager.h"
#include "common/memdatstruct/cMemoryBlock.h"
#include "string"
#include "iostream"

using namespace std;

namespace common {
	namespace memdatstruct {

template <class T>
class cStack2
{
public:
	cStack2();
	cStack2(cMemoryManager * mmanager);													// konstruktor prijme odkaz na hlavny objekt mmanageru, default si ziada objekt o BIG blok pamata
	cStack2(cMemoryManager * mmanager, char sign_block_size);								// konstruktor prijme odkaz na hlavny objekt mmanageru a uzivatel moze stanovit pozadovanu velkost blokov pamata (S-small,B-big)
	cStack2(cMemoryManager * mmanager, char sign_block_size, bool use_system_block);		// konstruktor prijme odkaz na hlavny objekt mmanageru, umozni nasatvit velkost system_block size
	~cStack2(void);

	void Init(cMemoryManager * mmanager);													// inicializacna metoda nahradzuje v pripade potreby konstruktor
	void Init(cMemoryManager * mmanager, char sign_block_size);								// inicializacna metoda nahradzuje v pripade potreby konstruktor
	void Init(cMemoryManager * mmanager, char sign_block_size, bool use_system_block);		// inicializacna metoda nahradzuje v pripade potreby konstruktor

	bool IsEmpty(void);
	void Push(const T &value);
	T& Pop(void);
	T& Top(void);
	void Clear(unsigned int count = 0);
	void ClearAll();
	
	// vlastnosti
	unsigned int GetSize(void);
	int& GetTop(void);
	unsigned long int GetCountOfUsedMemBlocks(void);										// metoda vrati pocet pouzitych (vzhradenych) memory blokov
	char GetSignBlockSize(void);															// metoda vrati priznak, ci struktura pouziva SMALL alebo BIG bloky

private:
	cMemoryManager * mem_manager;
	cMemoryBlock * cMemoryBlocks_managment;

	void CountItemsPerBlock(void);

	unsigned int max_block_size;
	unsigned int items_per_block;

	T ** arrays_blocks;						// pretypovane pole ukazatelov, default si objekt ziada o BIG blok pamata
	int sizeof_arrays_blocks;				// velkost pola arrays_blocks
	int top;
	unsigned int type_range;				// pocet bytov, ktrory zabera aktualny datovy typ
	unsigned long int amb_actual_position;	// pozicia v poli arrays_blocks

	void Reallocate(void);
};


class cStack2Exception
{
	private:
		string description;

	public:
		cStack2Exception(string exp) { description="Exception : "+ exp; }
		string get_exp() { return description; }
};
}}
