#pragma once

#include "common/memdatstruct/cMemoryManager.h"
#include "common/memdatstruct/cMemoryBlock.h"

namespace common {
	namespace memdatstruct {

template <class T>
class cArray2
{
public:
	cArray2();
	cArray2(cMemoryManager * mmanager);													// konstruktor prijme odkaz na hlavny objekt mmanageru, default si ziada objekt o BIG blok pamata
	cArray2(cMemoryManager * mmanager, char sign_block_size);								// konstruktor prijme odkaz na hlavny objekt mmanageru
	cArray2(cMemoryManager * mmanager, char sign_block_size, bool use_system_block);		// konstruktor prijme odkaz na hlavny objekt mmanageru, umozni nasatvit velkost system_block size
	~cArray2(void);

	void Init(cMemoryManager * mmanager);													// inicializacna metoda nahradzuje v pripade potreby konstruktor
	void Init(cMemoryManager * mmanager, char sign_block_size);								// inicializacna metoda nahradzuje v pripade potreby konstruktor
	void Init(cMemoryManager * mmanager, char sign_block_size, bool use_system_block);		// inicializacna metoda nahradzuje v pripade potreby konstruktor

	void Add(const T &item);
	void Add(const T &item, unsigned int position);											// metoda vlozi prvok na zadanu poziciu, ostetne polozky posunie v poli doprava
	void Remove(unsigned int position);														// metoda odstrani prvok zo zadanej pozicie, ostatne prvky posunie dolava
	T& operator[](unsigned int index);
	void Clear(unsigned int count = 0);
	void ClearAll();

	// vlastnosti
	unsigned int& GetSize();
	unsigned long int GetCountOfUsedMemBlocks(void);									// metoda vrati pocet pouzitych (vzhradenych) memory blokov
	char GetSignBlockSize(void);														// metoda vrati priznak, ci struktura pouziva SMALL alebo BIG bloky

private:
	cMemoryManager * mem_manager;
	cMemoryBlock * cMemoryBlocks_managment;

	void CountItemsPerBlock(void);
	
	unsigned int max_block_size;	
	unsigned int items_per_block;

	T ** arrays_blocks;									// pretypovane pole ukazatelov, default si objekt ziada o BIG blok pamata
	unsigned int sizeof_arrays_blocks;					// velkost pola arrays_blocks
	unsigned int type_range;							// pocet bytov, ktory zabera aktualny datovy typ
	unsigned int end_array_position;					// premenna nesie udaj o tom, kde sa nachadza posledna vlozena hodnota
	unsigned long int amb_actual_position;				// pozicia v poli cMemoryBlocks * cMemoryBlocks

	void Reallocate(void);
};
}}
