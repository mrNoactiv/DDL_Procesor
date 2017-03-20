#pragma once

#include "common/memdatstruct/cMemoryManager.h"
#include "common/memdatstruct/cMemoryBlock.h"

namespace common {
	namespace memdatstruct {

template <class T>
class cArray
{
public:
	cArray();
	cArray(cMemoryManager * mmanager);													// konstruktor prijme odkaz na hlavny objekt mmanageru, array pracuje so SMALL blokmi pamate 
	cArray(cMemoryManager * mmanager, char sign_block_size);							// konstruktor prijma odkaz na hlavny mem.manager a priznak velkosti blokov: S - SMALL, B - BIG
	cArray(cMemoryManager * mmanager, char sign_block_size, bool use_system_block);
	~cArray();

	void Init(cMemoryManager * mmanager);												// inicializacna metoda nahradzuje v pripade potreby konstruktor
	void Init(cMemoryManager * mmanager, char sign_block_size);						    // inicializacna metoda nahradzuje v pripade potreby konstruktor
	void Init(cMemoryManager * mmanager, char sign_block_size, bool use_system_block);	// inicializacna metoda nahradzuje v pripade potreby konstruktor

	void Add(const T &item);
	T& operator[](unsigned int index);
	void Clear(unsigned int count = 0);
	void ClearAll();

	// vlastnosti
	unsigned int GetSize();
	unsigned long int GetCountOfUsedMemBlocks(void);									// metoda vrati pocet pouzitych (vzhradenych) memory blokov
	char GetSignBlockSize(void);														// metoda vrati priznak, ci struktura pouziva SMALL alebo BIG bloky

private:
	cMemoryManager * mem_manager;
	cMemoryBlock * cMemoryBlocks_managment;

	void CountItemsPerBlock(void);
	
	unsigned int max_block_size;	
	unsigned int items_per_block;

	unsigned int type_range;					// pocet bytov, ktory zabera aktualny datovy typ
	unsigned int end_array_position;			// premenna nesie udaj o tom, kde sa nachadza posledna vlozena hodnota. Presnost orientacie je dana amb_actual_position, end_array_position 
	unsigned long int amb_actual_position;		// pozicia v poli cMemoryBlocks * cMemoryBlocks

	void AllocateNextMemory();
};
}}
