#pragma once

#include <mutex>
#include <stdio.h>

#include "common/memdatstruct/cMemoryBlock.h"
#include "common/memdatstruct/cBlockLinkedList.h"
#include "common/memdatstruct/cHeapMemoryBlock.h"
#include "vector"
#include "iostream"
#include "assert.h"

using namespace std;

namespace common {
	namespace memdatstruct {

class cMemoryManager
{
public:
	static int const SMALL_SIZE = 0;
	static int const BIG_SIZE = 1;
	static int const SYSTEM_SIZE = 2; 
	static int const VERYLARGE_SIZE = 2; 


	cMemoryManager(unsigned int count_of_blocks = 100, 
		unsigned int count_of_SYSTEM_blocks = 1, // 10, 
		unsigned int size_SMALL = 512, 
		unsigned int size_BIG = 10240, 
		unsigned long int size_SYSTEM = /*400000000, */ 26000000, // 102400,
		bool extend_pools = true);
	~cMemoryManager(void);

	inline cMemoryBlock *GetMem();
	inline cMemoryBlock *GetMem(unsigned int block_size);
	inline cMemoryBlock *GetMemSmall();
	inline cMemoryBlock *GetMemBig();
	inline cMemoryBlock *GetMemSystem();

	inline void ReleaseMem(cMemoryBlock * memoryBlock);
	inline void ReleaseLargeMem(cMemoryBlock * startBlock, cMemoryBlock * endBlock, unsigned int blockCount);

	// vlastnosti
	inline unsigned int GetSize_SMALL(void);
	inline unsigned int GetSize_BIG(void);
	inline unsigned int GetSize_SYSTEM(void);
	inline bool IsExtend(void);
	inline void SetExtend(bool extend_pools);

	// pomocne metody pre vypis obsahu pamate
	void PrintMemoryBlock(int position);
	void Vypis_VolnuObsadenu_pamat();
	void Vypis_PocetVolnychBlokov();

	void PrinStatistics();
private:
	static int const EXTEND_SIZE = 100000;						// maximalna velkost v bytoch, ktora bude alokovana z haldy pri rozirujucej alokacii. Ak bude velkost bloku (SMALL, BIG, SYSTEM) nastavena na hodnotu > 100 000B najde sa najblizsia velkost vacsia od 100 000

	cMemoryBlock ** blocks_management_SMALL;						// pole nesuce informacie o SMALL blokoch pamati, kazdy blok ma uzivatelom pridelenu velkost, alebo default 512B
	vector<cHeapMemoryBlock*> * vector_block_memory_SMALL;		// vector pre ukladanie pola poli char vyalokovaneho z haldy. Vector sluzi v pripade, ze je manager nastaveny do rezimu extend_pool=true k ukladaniu dalej pamate
	cBlockLinkedList*		mLinkedListSmall;						// zoznam (fronta) so SMALL blokmi pamata, ktore su volne a cakaju na pridelenie.

	cMemoryBlock ** blocks_management_BIG;						// pole nesuce informacie o BIG blokoch pamati, kazdy blok ma uzivatelom pridelenu velkost, alebo default 8192B
	vector<cHeapMemoryBlock*> * vector_block_memory_BIG;		// vector pre ukladanie pola poli char vyalokovaneho z haldy. Vector sluzi v pripade, ze je manager nastaveny do rezimu extend_pool=true k ukladaniu dalej pamate
	cBlockLinkedList*		mLinkedListBig;							// zoznam (fronta) s BIG blokmi pamata, ktore su volne a cakaju na pridelenie.

	cMemoryBlock ** blocks_management_SYSTEM;					// pole nesuce informacie o SYSTEM blokoch pamati, kazdy blok ma uzivatelom pridelenu velkost, alebo default 40960B
	vector<cHeapMemoryBlock*> * vector_block_memory_SYSTEM;		// vector pre ukladanie pola poli char vyalokovaneho z haldy. Vector sluzi v pripade, ze je manager nastaveny do rezimu extend_pool=true k ukladaniu dalej pamate
	cBlockLinkedList*		mLinkedListSystem;						// zoznam (fronta) s SYSTEM blokmi pamata, ktore su volne a cakaju na pridelenie.

	std::mutex mMutex;

	unsigned int memory_size_SMALL;								// velkost pamate - pocet vyalokovanych blokov pamate v SMALL
	unsigned int memory_size_BIG;								// velkost pamate - pocet vyalokovanych blokov pamate v BIG
	unsigned int memory_size_SYSTEM;							// velkost pamate - pocet vyalokovanych blokov pamate v SYSTEM
	unsigned int size_SMALL;									// velkost maleho bloku v bytoch
	unsigned int size_BIG;										// velkost velkeho bloku v bytoch
	unsigned int size_SYSTEM;									// velkost system bloku v bytoch

	bool extend_pools;											// priznak, ci sa budu pooly blokov rozsirovat, v pripade ze bude pool prazdny. true- sa rozsiruje, false - nerozsiruje

	void Build_block_management(int position_in_vector, int block_size = -1, int extend_count = 0);
	void AllocateMemory();
	void ReallocateNextMemory(int block_size);
	void ReallocateBlocksManagment(int block_size, int extend_count);
};



/// <summary>Metoda MemoryRequest na pridelenie bloku pamate ziadatelovi. Metoda neprijma ziadne parametry a implicitne prideluje SMALL blok.</summary>
/// <returns>Memory_block * (odkaz na pamatovy blok)</returns>
cMemoryBlock * cMemoryManager::GetMem()
{
	std::lock_guard<std::mutex> lock(mMutex);
	
	if( extend_pools && mLinkedListSmall->IsEmpty() )
	{
		ReallocateNextMemory( cMemoryManager::SMALL_SIZE );
	}
	cMemoryBlock * pt_memory_block = mLinkedListSmall->GetFirstNode();

	return pt_memory_block;
}

/// <summary>Metoda MemoryRequest na pridelenie bloku, podla zadaneho parametru. 0 vrati SMALL blok, 1 vrati BIG blok, ostatne vracia NULL</summary>
/// <param name="block_size">Definuje velkost bloku, ktory pozaduje volajuci proces</param>
/// <returns>Memory_block * (odkaz na pamatovy blok)</returns>
cMemoryBlock *cMemoryManager::GetMem(unsigned int block_size)
{
	cMemoryBlock* pt_memory_block = NULL;

	std::lock_guard<std::mutex> lock(mMutex);
	
	if(block_size <= cMemoryManager::size_SMALL)
	{
		if( extend_pools && mLinkedListSmall->IsEmpty() )
		{
			ReallocateNextMemory( cMemoryManager::SMALL_SIZE );
		}
		pt_memory_block = mLinkedListSmall->GetFirstNode();
	}
	else if(block_size <= cMemoryManager::size_BIG)
	{
		if( extend_pools && mLinkedListBig->IsEmpty() )
		{
			ReallocateNextMemory( cMemoryManager::BIG_SIZE );
		}
		pt_memory_block = mLinkedListBig->GetFirstNode();
	}
	else if(block_size <= cMemoryManager::size_SYSTEM)
	{
		if( extend_pools && mLinkedListSystem->IsEmpty() )
		{
			ReallocateNextMemory( cMemoryManager::SYSTEM_SIZE );
		}
		pt_memory_block = mLinkedListSystem->GetFirstNode();
	}
	else 
	{
		/*if( extend_pools && mLinkedListSystem->IsEmpty() )
		{
			ReallocateNextMemory(block_size);
		}
		pt_memory_block = mLinkedListSystem->GetFirstNode();*/

		printf("Warning: cMemoryManager::GetMem(): The block size requested > the maximal size of a block!\n");
	}

	return pt_memory_block;
}

/// <summary>Method returns a SMALL memory block.</summary>
/// <returns>Memory_block * (SMALL memory block)</returns>
cMemoryBlock * cMemoryManager::GetMemSmall()
{
	cMemoryBlock* pt_memory_block = NULL;

	std::lock_guard<std::mutex> lock(mMutex);

	if( extend_pools && mLinkedListSmall->IsEmpty() )
	{
		ReallocateNextMemory( cMemoryManager::SMALL_SIZE );
	}
	pt_memory_block = mLinkedListSmall->GetFirstNode();

	return pt_memory_block;
}

/// <summary>Method returns a BIG memory block.</summary>
/// <returns>Memory_block * (BIG memory block)</returns>
cMemoryBlock * cMemoryManager::GetMemBig()
{
	cMemoryBlock* pt_memory_block = NULL;
	
	std::lock_guard<std::mutex> lock(mMutex);

	if( extend_pools && mLinkedListBig->IsEmpty() )
	{
		ReallocateNextMemory( cMemoryManager::BIG_SIZE );
	}
	pt_memory_block = mLinkedListBig->GetFirstNode();

	return pt_memory_block;
}

/// <summary>Method returns a SYSTEM memory block.</summary>
/// <returns>Memory_block * (SYSTEM memory block)</returns>
cMemoryBlock * cMemoryManager::GetMemSystem()
{
	cMemoryBlock* pt_memory_block = NULL;
	
	std::lock_guard<std::mutex> lock(mMutex);

	if( extend_pools && mLinkedListSystem->IsEmpty() )
	{
		ReallocateNextMemory( cMemoryManager::SYSTEM_SIZE );
	}
	pt_memory_block = mLinkedListSystem->GetFirstNode();

	return pt_memory_block;
}

/// <summary>Metoda Release_memory sluzi k navratu jednoho bloku do prislusneho pamatoveho poolu. Metoda prijma jeden parameter, overi do
/// akej fronty sa bude blok vracat. Pred pristupom k pamatovemu poolu si metoda vyziada pridelenie zamku kritickej sekcie MeteredSection. 
/// Po zaradeni bloku do pamatoveho poolu zamok uvolni.</summary>
/// <param name="pt_memory_block">Odkaz na blok pamate, ktory sa vracia do prislusneho pamatoveho poolu</param>
/// <returns>Vracia void</returns>
void cMemoryManager::ReleaseMem(cMemoryBlock* pt_memory_block)
{
	std::lock_guard<std::mutex> lock(mMutex);

	char sign_size = pt_memory_block->Get_SignSize();

	if(sign_size == 'S')
	{
		mLinkedListSmall->AppendNode(pt_memory_block);
	}
	else if(sign_size == 'B')
	{
		mLinkedListBig->AppendNode(pt_memory_block);
	}
	else if(sign_size == 'T')
	{
		mLinkedListSystem->AppendNode(pt_memory_block);
	} 
	
	//std::cout << "Pamat: " << (int *) pt_memory_block->GetMemAddressBegin() << " bola uvolnena!\n";	// pomocny vypis - nasledne je nutne odstranit
}

/**
* This method of ReleaseMem is capable to release more than one block of memory during one call. 
* It simply append the linked list of memory blocks to the pool.
* \param startMemoryBlock Reference to the first memory block that will be returned to the pool
*/
void cMemoryManager::ReleaseLargeMem(cMemoryBlock* startMemoryBlock, cMemoryBlock* endMemoryBlock, unsigned int blockCount)
{
	char sign_size = startMemoryBlock->Get_SignSize();

	assert(sign_size == 'T');

	std::lock_guard<std::mutex> lock(mMutex);

	mLinkedListSystem->AppendNode(startMemoryBlock, endMemoryBlock, blockCount);

}


/// <summary>Metoda vracia velkost SMALL bloku v bytoch.</summary>
/// <returns>Vracia unsigned int</returns>
unsigned int cMemoryManager::GetSize_SMALL()
{
	return size_SMALL;
}

/// <summary>Metoda vracia velkost BIG bloku v bytoch.</summary>
/// <returns>Vracia unsigned int</returns>
unsigned int cMemoryManager::GetSize_BIG()
{
	return size_BIG;
}

/// <summary>Metoda vracia velkost SYSTEM bloku v bytoch.</summary>
/// <returns>Vracia unsigned int</returns>
unsigned int cMemoryManager::GetSize_SYSTEM()
{
	return size_SYSTEM;
}


/// <summary>Metoda IsExtend vracia logicku hodnotu rezimu spravcu, ci sa budu pamatove pooly v pripade nedostatku pamatovzyh blokov rozsirovat, alebo nie.</summary>
/// <returns>Vracia bool. Pamatove pooly: TRUE-su rozsirovatelne, FALSE-nie su rozsirovatelne</returns>
bool cMemoryManager::IsExtend(void)
{
	return extend_pools;
}


/// <summary>Metoda SetExtend nastavuje logicku hodnotu rezimu spravcu, ci sa budu pamatove pooly v pripade nedostatku pamatovych blokov rozsirovat, alebo nie.</summary>
/// <param name="extend_pools">Urcuje, ci sa budu pamatove pooly rozsirovat alebo nie. Pamatove pooly: TRUE-su rozsirovatelne, FALSE-nie su rozsirovatelne</param>
/// <returns>Vracia void</returns> 
void cMemoryManager::SetExtend(bool extend_pools)
{
	this->extend_pools = extend_pools;
}

}}
