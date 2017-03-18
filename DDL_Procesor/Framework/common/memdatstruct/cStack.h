#pragma once

#include "common/memdatstruct/cMemoryManager.h"
#include "common/memdatstruct/cMemoryBlock.h"
#include "string"
#include "iostream"

using namespace std;

namespace common {
	namespace memdatstruct {

template <class T>
class cStack
{
public:
	cStack();
	cStack(cMemoryManager * mmanager);								// konstruktor prijme odkaz na hlavny objekt mmanageru, default si ziada objekt o BIG blok pamata
	cStack(cMemoryManager * mmanager, char sign_block_size);		// konstruktor prijme odkaz na hlavny objekt mmanageru a uzivatel moze stanovit pozadovanu velkost blokov pamata (S-small,B-big)
	cStack(cMemoryManager * mmanager, char sign_block_size, bool use_system_block);
	~cStack(void);

	void Init(cMemoryManager * mmanager);												// inicializacna metoda nahradzuje v pripade potreby konstruktor
	void Init(cMemoryManager * mmanager, char sign_block_size);							// inicializacna metoda nahradzuje v pripade potreby konstruktor
	void Init(cMemoryManager * mmanager, char sign_block_size, bool use_system_block);	// inicializacna metoda nahradzuje v pripade potreby konstruktor

	bool IsEmpty(void);
	void Push(const T &value);
	T Pop(void);
	T Top(void);
	void Clear(unsigned int count = 0);
	void ClearAll(void);

	// vlastnosti
	unsigned int GetSize(void);
	int& GetTop(void);
	unsigned long int GetCountOfUsedMemBlocks(void);									// metoda vrati pocet pouzitych (vzhradenych) memory blokov
	char GetSignBlockSize(void);														// metoda vrati priznak, ci struktura pouziva SMALL alebo BIG bloky

private:
	cMemoryManager * mem_manager;
	cMemoryBlock * cMemoryBlocks_managment;

	//unsigned int sizeof_cMemoryBlocks;					// velkost pola cMemoryBlocks
	void CountItemsPerBlock(void);
	
	unsigned int max_block_size;	
	unsigned int items_per_block;

	unsigned int type_range;							// pocet bytov, ktory zabera aktualny datovy typ
	int top;
	unsigned long int amb_actual_position;				// pozicia v poli cMemoryBlocks * cMemoryBlocks
	unsigned int end_array_position;					// premenna nesie udaj o tom, kde sa nachadza posledna vlozena hodnota. Presnost orientacie je dana amb_actual_position, end_array_position 

	void AllocateNextMemory();
	void ReleaseLastMemoryBlock();								// metoda pre uvolnenie bloku pamata v pripade, ze zo zasobniku sa vybera a zasobnik sa zmensuje
};
}}



