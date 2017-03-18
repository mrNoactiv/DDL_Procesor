#include "common/memdatstruct/cMemoryManager.h"

using namespace common::memdatstruct;

//#define TEST_CORRECTNESS


/// <summary>Konstruktor triedy cMemoryManager, prijma sest parametrov. Konstruktor inicializuje triedne premenne a vola metodu 
/// AllocateMemory s parametrom velkosti size. Implicitne nie je nastavena ziadna velkost bloku.</summary>
/// <para>TRUE - pamatove pooly sa budu rozsirovat v pripade ich vyprazdnenia.</para>
/// <para>FALSE - pamatove pooly sa nebudu rozsirovat</para>
/// <param name="count_of_blocks">Parameter urcuje pocet alokovanych pamatovych blokov typu S,B</param>
/// <param name="count_of_SYSTEM_blocks">Parameter urcuje pocet alokovanych systemovych blokov T</param>
/// <param name="size_SMALL">Parameter urcuje velkost pamatovych blokov typu S</param>
/// <param name="size_BIG">Parameter urcuje velkost pamatovych blokov typu B</param>
/// <param name="size_SYSTEM">Parameter urcuje velkost pamatovych blokov typu T</param>
/// <param name="extend_pools">Parameter urcuje rezim rozsirovatelnosti pamatovych poolov</param>
cMemoryManager::cMemoryManager(unsigned int count_of_blocks, 
							   unsigned int count_of_SYSTEM_blocks, 
							   unsigned int size_SMALL, 
							   unsigned int size_BIG, 
							   unsigned long int size_SYSTEM, 
							   bool extend_pools)
{
	this->extend_pools = extend_pools;				// nasavim mod spravcu, ci budu prazdne pooly alokovat dalsiu pamat z haldy (true-budu alokovat, false-nebudu alokovat)

	this->size_SMALL = size_SMALL;
	this->size_BIG = size_BIG;
	this->size_SYSTEM = size_SYSTEM;	


	vector_block_memory_SMALL = new vector<cHeapMemoryBlock*>();
	vector_block_memory_BIG = new vector<cHeapMemoryBlock*>();
	vector_block_memory_SYSTEM = new vector<cHeapMemoryBlock*>();

	memory_size_SYSTEM = count_of_SYSTEM_blocks;
	memory_size_SMALL = count_of_blocks;			// prideli velkost pamate (pocet blokov pamate)
	memory_size_BIG = count_of_blocks;

	AllocateMemory();
}


/// <summary>Destruktor triedy. Destruktor finalizuje vsetky objekty a uvolnuje vsetku pamat alokovanu na halde.</summary>
cMemoryManager::~cMemoryManager(void)
{
	delete mLinkedListSmall;								// uvolnujem fronty
	delete mLinkedListBig;
	delete mLinkedListSystem;

	for( unsigned int i = 0; i < vector_block_memory_SMALL->size(); i++ )
	{
		delete [] vector_block_memory_SMALL->at(i);										// uvolnenie pola SMALL pamatovych blokov
	}
	delete vector_block_memory_SMALL;

	// rusim spravu pamattovych blokov SMALL
	for(unsigned  int i = 0; i < memory_size_SMALL; i++ )
	{
		delete blocks_management_SMALL[i];
	}
	delete [] blocks_management_SMALL;

	for( unsigned int i = 0; i < vector_block_memory_BIG->size(); i++ )
	{
		delete [] vector_block_memory_BIG->at(i);										// uvolnenie pola BIG pamatovych blokov
	}
	delete vector_block_memory_BIG;

	// rusim spravu pamattovych blokov BIG
	for( unsigned int i = 0; i < memory_size_BIG; i++ )
	{
		delete blocks_management_BIG[i];
	}
	delete [] blocks_management_BIG;

	for( unsigned int i = 0; i < vector_block_memory_SYSTEM->size(); i++ )
	{
		delete [] vector_block_memory_SYSTEM->at(i);										// uvolnenie pola BIG pamatovych blokov
	}
	delete vector_block_memory_SYSTEM;

	// rusim spravu pamattovych blokov SYSTEM
	for( unsigned int i = 0; i < memory_size_SYSTEM; i++ )
	{
		delete blocks_management_SYSTEM[i];
	}
	delete [] blocks_management_SYSTEM;
}


/// <summary>Metoda AllocateMemory inicializuje jednotlive pamatove pooly (cBlockLinkedList), vectory pamate z haldy a inicializuje objekty 
/// triedy cHeapMemoryBlock pre kazdy pamatovy blok.</summary>
/// <returns>Vracia void</returns>
void cMemoryManager::AllocateMemory()
{
	// inicializujem fronty
	mLinkedListSmall = new cBlockLinkedList(this->memory_size_SMALL);
	mLinkedListBig = new cBlockLinkedList(this->memory_size_BIG);
	mLinkedListSystem = new cBlockLinkedList(this->memory_size_SYSTEM);

	
	vector_block_memory_SMALL->push_back(new cHeapMemoryBlock[memory_size_SMALL]);					// SMALL - alokuje sa dvojrozmerne pole
	for(int i = 0; i < memory_size_SMALL; i++)
	{
		( vector_block_memory_SMALL->at(0) )[i].Init(size_SMALL);
	}
	blocks_management_SMALL = new cMemoryBlock*[memory_size_SMALL];

	vector_block_memory_BIG->push_back(new cHeapMemoryBlock[memory_size_BIG]);						// BIG - alokuje sa dvojrozmerne pole
	for(int j = 0; j < memory_size_BIG; j++)
	{
		( vector_block_memory_BIG->at(0) )[j].Init(size_BIG);
	}
	blocks_management_BIG = new cMemoryBlock*[memory_size_BIG];

	vector_block_memory_SYSTEM->push_back(new cHeapMemoryBlock[memory_size_SYSTEM]);				// BIG - alokuje sa dvojrozmerne pole
	for(unsigned int j = 0; j < memory_size_SYSTEM; j++)
	{
		( vector_block_memory_SYSTEM->at(0) )[j].Init(size_SYSTEM);
	}
	blocks_management_SYSTEM = new cMemoryBlock*[memory_size_SYSTEM];

	Build_block_management(0);
}


/// <summary>Metoda ReallocateNextMemory rozsiruje pamat ramca o dalsiu pamat z haldy a mapuje rozsirujucu pamat do pamatovych blokov spravcu.
/// Pamat sa alokuje vzdy do prislusneho pamatoveho poolu zadaneho ako parameter metody.</summary>
/// <param name="block_size">Definuje typ pamatoveho bloku (jeho velkost-S,B,T), ktory sa bude rozsirovat</param>
/// <return>Vracie void</return>
void cMemoryManager::ReallocateNextMemory(int block_size)
{
	int extend_count = 0;
	int last_vector_position = 0;

	if(block_size == cMemoryManager::SMALL_SIZE)
	{
		extend_count = memory_size_SMALL / 3;							// vypocitam pocet blokov ktore budem alokovat na halde z velkosti bloku S, M, T a a konstatnty EXTEND_SIZE
		if(extend_count == 0) { extend_count = 1; }												// ak je 0 nastavim na 1, aby sa alokoval aspon jeden blok								

		vector_block_memory_SMALL->push_back(new cHeapMemoryBlock[extend_count]);
		last_vector_position = vector_block_memory_SMALL->size()-1;

		for(int i = 0; i < extend_count; i++)
		{
			( vector_block_memory_SMALL->at(last_vector_position) )[i].Init(size_SMALL);
		}

		ReallocateBlocksManagment( cMemoryManager::SMALL_SIZE, extend_count);
		mLinkedListSmall->ReallocateList(extend_count);
		Build_block_management( vector_block_memory_SMALL->size()-1 , cMemoryManager::SMALL_SIZE, extend_count);

	}
	else if(block_size == cMemoryManager::BIG_SIZE)
	{
		extend_count = memory_size_BIG / 3;								// vypocitam pocet blokov ktore budem alokovat na halde z velkosti bloku S, M, T a a konstatnty EXTEND_SIZE
		if(extend_count == 0) { extend_count = 1; }												// ak je 0 nastavim na 1, aby sa alokoval aspon jeden blok								

		vector_block_memory_BIG->push_back(new cHeapMemoryBlock[extend_count]);
		last_vector_position = vector_block_memory_BIG->size()-1;

		for(int i = 0; i < extend_count; i++)
		{
			( vector_block_memory_BIG->at(last_vector_position) )[i].Init(size_BIG);
		}

		ReallocateBlocksManagment( cMemoryManager::BIG_SIZE, extend_count);
		mLinkedListBig->ReallocateList(extend_count);
		Build_block_management( vector_block_memory_BIG->size()-1 , cMemoryManager::BIG_SIZE, extend_count);

	}
	else if(block_size == cMemoryManager::SYSTEM_SIZE)
	{
		extend_count = memory_size_SYSTEM / 3;								// vypocitam pocet blokov ktore budem alokovat na halde z velkosti bloku S, M, T a a konstatnty EXTEND_SIZE
		if(extend_count == 0) { extend_count = 1; }												// ak je 0 nastavim na 1, aby sa alokoval aspon jeden blok								

		vector_block_memory_SYSTEM->push_back(new cHeapMemoryBlock[extend_count]);
		last_vector_position = vector_block_memory_SYSTEM->size()-1;

		for(int i = 0; i < extend_count; i++)
		{
			( vector_block_memory_SYSTEM->at(last_vector_position) )[i].Init(size_SYSTEM);
		}

		ReallocateBlocksManagment( cMemoryManager::SYSTEM_SIZE, extend_count);
		mLinkedListSystem->ReallocateList(extend_count);
		Build_block_management( vector_block_memory_SYSTEM->size()-1 , cMemoryManager::SYSTEM_SIZE, extend_count);
	}
}


/// <summary>Metoda ReallocateBlockManagement realokuje polia block_management pre spravu blokov u jednotlivych velkosti 
/// blokov S,B,T v pripade rozsirovania urcite pamatoveho poolu. Metoda prijme dva parametre.</summary>
/// <param name="block_size">Definuje typ pamatoveho bloku (jeho velkost-S,B,T), ktory sa rozsiroval</param>
/// <param name="extend_count">Definuje pocet blokov o ktory sa rozsiruje pamatovy pool</param>
/// <returns>Vracia void</returns>
void cMemoryManager::ReallocateBlocksManagment(int block_size, int extend_count)
{
	cMemoryBlock** tmp_blocks_managment;
	int tmp_memory_size = 0;

	if( block_size == cMemoryManager::SMALL_SIZE )
	{
		tmp_memory_size = this->memory_size_SMALL + extend_count;
		tmp_blocks_managment = new cMemoryBlock*[tmp_memory_size];

		for(unsigned int i = 0; i < memory_size_SMALL; i++)
		{
			tmp_blocks_managment[i] = blocks_management_SMALL[i];
		}

		delete [] blocks_management_SMALL;
		blocks_management_SMALL = tmp_blocks_managment;
		this->memory_size_SMALL = tmp_memory_size;
		
	}
	else if( block_size == cMemoryManager::BIG_SIZE )
	{
		tmp_memory_size = memory_size_BIG + extend_count;
		tmp_blocks_managment = new cMemoryBlock*[tmp_memory_size];

		for(unsigned int i = 0; i < memory_size_BIG; i++)
		{
			tmp_blocks_managment[i] = blocks_management_BIG[i];
		}

		delete [] blocks_management_BIG;
		blocks_management_BIG = tmp_blocks_managment;
		this->memory_size_BIG = tmp_memory_size;
	}
	else if( block_size == cMemoryManager::SYSTEM_SIZE )
	{
		tmp_memory_size = memory_size_SYSTEM + extend_count;
		tmp_blocks_managment = new cMemoryBlock*[tmp_memory_size];

		for(unsigned int i = 0; i < memory_size_SYSTEM; i++)
		{
			tmp_blocks_managment[i] = blocks_management_SYSTEM[i];
		}

		delete [] blocks_management_SYSTEM;
		blocks_management_SYSTEM = tmp_blocks_managment;
		this->memory_size_SYSTEM = tmp_memory_size;
	}

	tmp_blocks_managment = 0;
}


/// <summary>Metoda Build_block_management inicializuje polia pre spravu blokov pamate. Metoda prijma tri parametre.</summary>
/// <param name="position_in_vector">Definuje poziciu vo vektore uchovavajucom pamat ramca alokovanu z hlady</param>
/// <param name="block_size">Definuje typ pamatoveho bloku (jeho velkost-S,B,T), ktory sa rozsiroval</param>
/// <param name="extend_count">Definuje pocet blokov o ktory sa rozsiruje pamatovy pool</param>
/// <returns>Vracia void</returns>
void cMemoryManager::Build_block_management(int position_in_vector, int block_size, int extend_count)
{
	if(block_size == -1)
	{
		for(unsigned int i = 0; i < memory_size_SMALL; i++)										// budujem block_managment pre SMALL
		{
			blocks_management_SMALL[i] = new cMemoryBlock( i, 'S', size_SMALL, ( vector_block_memory_SMALL->at(position_in_vector) )[i].GetBlock_memory() );
			mLinkedListSmall->AppendNode(blocks_management_SMALL[i]);
		}

		for(unsigned int i = 0; i < memory_size_BIG; i++)										// budujem block_managment pre big
		{
			blocks_management_BIG[i] = new cMemoryBlock( i, 'B', size_BIG, ( vector_block_memory_BIG->at(position_in_vector) )[i].GetBlock_memory() );
			mLinkedListBig->AppendNode(blocks_management_BIG[i]);
		}

		for(unsigned int i = 0; i < memory_size_SYSTEM; i++)										// budujem block_managment pre system
		{
			blocks_management_SYSTEM[i] = new cMemoryBlock( i, 'T', size_SYSTEM, ( vector_block_memory_SYSTEM->at(position_in_vector) )[i].GetBlock_memory() );
			mLinkedListSystem->AppendNode(blocks_management_SYSTEM[i]);
		}
	}
	else if(block_size == cMemoryManager::SMALL_SIZE)
	{
		int pool_size = mLinkedListSmall->GetCountOfPointers()-extend_count;

		for(int i = 0, j = pool_size; i < extend_count; i++, j++)										// budujem block_managment pre SMALL
		{
			blocks_management_SMALL[j] = new cMemoryBlock( j, 'S', size_SMALL, ( vector_block_memory_SMALL->at(position_in_vector) )[i].GetBlock_memory() );
			mLinkedListSmall->AppendNode(blocks_management_SMALL[j]);
		}
	}
	else if(block_size == cMemoryManager::BIG_SIZE)
	{
		int pool_size = mLinkedListBig->GetCountOfPointers()-extend_count;

		for(int i = 0, j = pool_size; i < extend_count; i++, j++)										// budujem block_managment pre big
		{
			blocks_management_BIG[j] = new cMemoryBlock( j, 'B', size_BIG, ( vector_block_memory_BIG->at(position_in_vector) )[i].GetBlock_memory() );
			mLinkedListBig->AppendNode(blocks_management_BIG[j]);
		}
	}
	else if(block_size == cMemoryManager::SYSTEM_SIZE)
	{
		int pool_size = mLinkedListSystem->GetCountOfPointers()-extend_count;

		for(int i = 0, j = pool_size; i < extend_count; i++, j++)										// budujem block_managment pre system
		{
			blocks_management_SYSTEM[j] = new cMemoryBlock( j, 'T', size_SYSTEM, ( vector_block_memory_SYSTEM->at(position_in_vector) )[i].GetBlock_memory() );
			mLinkedListSystem->AppendNode(blocks_management_SYSTEM[j]);
		}
	}
}




/* **************************************************************************************************** */
/* ************************************  POMOCNE METODY  ********************************************** */
/* **************************************************************************************************** */


/// <summary>Pomocna metoda Vypis_Memory_block, vypisuje informacie o bloku pamati v poli Memory_block na zaklade predaneho ID.</summary>
/// <param name="position">Definuje poziciu pamatoveho bloku v poli spravy blokov.</param>
/// <returns>Vracia void</returns>
void cMemoryManager::PrintMemoryBlock(int position)
{
		std::cout << "\n\n*** *** *** *** *** *** *** ***\n--- Informacie o bloku pamati! ---\n" << 
				"\nPozicia pola: " << blocks_management_SMALL[position]->Get_pointer_position() << "\nJe blok pamati volny?: " << blocks_management_SMALL[position]->Get_is_free() << "\n*** *** *** *** *** *** *** ***\n\n";
}


/// <summary>Pomocna metoda Vypis_VolnuObsadenu_pamat, ktora do konzoly vypise statistiku (pocty) obsadenia S, B blokov.</summary>
/// <returns>Vracia void</returns>
void cMemoryManager::Vypis_VolnuObsadenu_pamat()
{
	std::cout << "*******************************************************************************\nSMALL - Volne bloky pamati:\n";
	for(unsigned int i = 0; i < memory_size_SMALL; i++)
	{
		if(blocks_management_SMALL[i]->Get_is_free())
		{
			std::cout << blocks_management_SMALL[i]->Get_pointer_position() << ",";
		}
	}

	std::cout << "\nSMALL - Obsadene bloky pamati:\n";
	for(unsigned int i = 0; i < memory_size_SMALL; i++)
	{
		if(!blocks_management_SMALL[i]->Get_is_free())
		{
			std::cout << blocks_management_SMALL[i]->Get_pointer_position() << ",";
		}
	}

	std::cout << "\n-----------------------------------------------------------------------------\nBIG - Volne bloky pamati:\n";
	for(unsigned int i = 0; i < memory_size_BIG; i++)
	{
		if(blocks_management_BIG[i]->Get_is_free())
		{
			std::cout << blocks_management_BIG[i]->Get_pointer_position() << ",";
		}
	}

	std::cout << "\nBIG - Obsadene bloky pamati:\n";
	for(unsigned int i = 0; i < memory_size_BIG; i++)
	{
		if(!blocks_management_BIG[i]->Get_is_free())
		{
			std::cout << blocks_management_BIG[i]->Get_pointer_position() << ",";
		}
	}
	std::cout << "\n*******************************************************************************\n";
}


/// <summary>Metoda Vypis_PocetVolnychBlokov poskytuje jednoduchy vypis do konzoly o stave pamatovych poolov. To znamena, pocet 
/// volnych pamatovych blokov v jednotlivych pooloch.</summary>
/// <returns>Vracia void</returns>
void cMemoryManager::Vypis_PocetVolnychBlokov()
{
	std::cout << "\nPocet volnych blokov SMALL size: " << mLinkedListSmall->GetSize() << "\n";
	std::cout << "Pocet volnych blokov BIG size: " << mLinkedListBig->GetSize() << "\n\n";
}

/**
* Vypise velikost pameti zabrane jednotlivymi typy bloku
*/
void cMemoryManager::PrinStatistics()
{
	printf("--------------------- Memory manager stats\n");
	float smallsize = (float)(memory_size_SMALL * size_SMALL) / (float)(1024 * 1024);
	float bigsize = (float)(memory_size_BIG * size_BIG) / (float)(1024 * 1024);
	float systemsize = (float)(memory_size_SYSTEM * size_SYSTEM) / (float)(1024 * 1024);
	printf("SMALL: %f[MB] , free %f[MB]\n", smallsize, (float)(mLinkedListSmall->GetSize() * size_SMALL) / (float)(1024 * 1024));
	printf("BIG: %f[MB] , free %f[MB]\n", bigsize, (float)(mLinkedListBig->GetSize() * size_BIG) / (float)(1024 * 1024));
	printf("SYSTEM: %f[MB] , free %f[MB]\n", systemsize, (float)(mLinkedListSystem->GetSize() * size_SYSTEM) / (float)(1024 * 1024));
	printf("All: %f[MB]\n", smallsize + systemsize + bigsize);
}