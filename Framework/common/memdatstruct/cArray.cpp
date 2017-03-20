#include "common/memdatstruct/cArray.h"

namespace common {
	namespace memdatstruct {

/// <summary>Prazdny konstruktor triedy</summary>
template <class T>
cArray<T>::cArray() 
{}

/// <summary>Konstruktor triedy prijma jeden parameter, odkaz na spravcu pamate. Inicializuje premenne triedy, poziada o prvy blok pamate pre administraciu a 
/// prvy blok pamatoveho poolu. Implicitne pracuje datova strukura pri inicializacii tymto konstruktorom so SMALL pamatovymi blokmi.</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
template <class T>
cArray<T>::cArray(cMemoryManager * mmanager) 
{
	mem_manager = mmanager;

	cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);

	end_array_position = 0;
	amb_actual_position = 0;
	type_range = sizeof(T);

	*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request();
	max_block_size = mem_manager->GetSize_SMALL() - type_range - 1;
	
	CountItemsPerBlock();
}


/// <summary>Konstruktor triedy prijma dva parametere, odkaz na spravcu pamate a priznak velkosti bloku. Inicializuje premenne triedy, poziada o prvy blok pamate pre 
/// administraciu a prvy blok pamatoveho poolu.</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <param name="sign_block_size">Priznak velkosti bloku, ktore budu v sukromnom pamatovom poole strukturz</param>
template <class T>
cArray<T>::cArray(cMemoryManager * mmanager, char sign_block_size) 
{
	mem_manager = mmanager;

	cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);

	end_array_position = 0;
	type_range = sizeof(T);
	amb_actual_position = 0;
	
	// na zaklade vstupneho parametru zaziadam o blok pamata prislusnej velkosti
	if(sign_block_size == 'S' || sign_block_size == 's')
	{
		*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::SMALL_SIZE);
		max_block_size = mem_manager->GetSize_SMALL() - type_range - 1;
	}
	else 
	{
		*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);
		max_block_size = mem_manager->GetSize_BIG() - type_range - 1;
	}

	CountItemsPerBlock();
}


/// <summary>Konstruktor triedy prijma tri parametre, odkaz na spravcu pamate, priznak velkosti bloku a info ci pouzivat systemovy blok. Inicializuje premenne 
/// triedy, poziada o prvy blok pamate pre administraciu a prvy blok pamatoveho poolu.</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <param name="sign_block_size">Priznak velkosti bloku, ktore budu v sukromnom pamatovom poole strukturz</param>
/// <param name="use_system_block">Priznak, ci pouzivat pre administraciu SYSTEM pamatovy blok. TRUE-pouzivat, FALSE-nepouzivat</param>
template <class T>
cArray<T>::cArray(cMemoryManager * mmanager, char sign_block_size, bool use_system_block) 
{
	mem_manager = mmanager;

	if(use_system_block)
	{
		cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::SYSTEM_SIZE);
	}
	else
	{
		cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);
	}

	end_array_position = 0;
	amb_actual_position = 0;
	type_range = sizeof(T);
	
	// na zaklade vstupneho parametru zaziadam o blok pamata prislusnej velkosti
	if(sign_block_size == 'S' || sign_block_size == 's')
	{
		*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::SMALL_SIZE);
		max_block_size = mem_manager->GetSize_SMALL() - type_range - 1;
	}
	else 
	{
		*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);
		max_block_size = mem_manager->GetSize_BIG() - type_range - 1;
	}

	CountItemsPerBlock();
}


/// <summary>Destruktor triedy vola metodu ClearAll</summary>
template <class T>
cArray<T>::~cArray(void) 
{
	ClearAll();
}


/// <summary>Inicializacna metoda Init sluzi k inicializacii dat.struktury, ak bola vytvorena prazdnym konstruktorom. Metoda prijma jeden 
/// parameter (odakz na spravcu pamate) a implicitne je sukromny pamatovy pool struktury tvoreny SMALL blokmi.</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <returns>Vracia void</returns>
template<class T>
void cArray<T>::Init(cMemoryManager * mmanager)
{
	mem_manager = mmanager;

	cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);

	end_array_position = 0;
	amb_actual_position = 0;
	type_range = sizeof(T);
	
	*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request();
	max_block_size = mem_manager->GetSize_SMALL() - type_range - 1;
	
	CountItemsPerBlock();
}


/// <summary>Inicializacna metoda Init sluzi k inicializacii dat.struktury, ak bola vytvorena prazdnym konstruktorom. Metoda prijma dva 
/// parametre (odkaz na spravcu pamate, priznak velkosti pam.bloku).</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <param name="sign_block_size">Priznak velkosti bloku, ktore budu v sukromnom pamatovom poole strukturz (S, B)</param>
/// <returns>Vracia void</returns>
template<class T>
void cArray<T>::Init(cMemoryManager * mmanager, char sign_block_size)
{
	mem_manager = mmanager;

	cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);

	end_array_position = 0;
	type_range = sizeof(T);
	amb_actual_position = 0;
	
	// na zaklade vstupneho parametru zaziadam o blok pamata prislusnej velkosti
	if(sign_block_size == 'S' || sign_block_size == 's')
	{
		*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::SMALL_SIZE);
		max_block_size = mem_manager->GetSize_SMALL() - type_range - 1;
	}
	else 
	{
		*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);
		max_block_size = mem_manager->GetSize_BIG() - type_range - 1;
	}

	CountItemsPerBlock();
}


/// <summary>Inicializacna metoda Init sluzi k inicializacii dat.struktury, ak bola vytvorena prazdnym konstruktorom. Metoda prijma tri 
/// parametre (odkaz na spravcu pamate, priznak velkosti pam.bloku, info ci pouzivat systemovy blok).</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <param name="sign_block_size">Priznak velkosti bloku, ktore budu v sukromnom pamatovom poole strukturz (S, B)</param>
/// <param name="use_system_block">Priznak, ci pouzivat pre administraciu SYSTEM pamatovy blok. TRUE-pouzivat, FALSE-nepouzivat</param>
/// <returns>Vracia void</returns>
template<class T>
void cArray<T>::Init(cMemoryManager * mmanager, char sign_block_size, bool use_system_block)
{
	mem_manager = mmanager;

	if(use_system_block)
	{
		cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::SYSTEM_SIZE);
	}
	else
	{
		cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);
	}

	end_array_position = 0;
	amb_actual_position = 0;
	type_range = sizeof(T);
	
	// na zaklade vstupneho parametru zaziadam o blok pamata prislusnej velkosti
	if(sign_block_size == 'S' || sign_block_size == 's')
	{
		*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::SMALL_SIZE);
		max_block_size = mem_manager->GetSize_SMALL() - type_range - 1;
	}
	else 
	{
		*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);
		max_block_size = mem_manager->GetSize_BIG() - type_range - 1;
	}

	CountItemsPerBlock();
}


/// <summary>Metoda CountItemsPerBlock nastavi maximalny pocet poloziek, ktore bude mozne ulozit do jedneho bloku pamate.</summary>
/// <returns>Vracia void</returns>
template <class T>
void cArray<T>::CountItemsPerBlock()
{
	if( (max_block_size % type_range) == 0 )						// v pripade ze je delitelne bezzvysku
	{
		items_per_block = (max_block_size / type_range) + 1;
	}
	else
	{
		items_per_block = (max_block_size / type_range);
	}
}


/// <summary>Metoda AllocateNextMemory ziada o dalsi blok pamate prislusnej velkosti a blok zaradi do sukromneho pamatoveho poolu.</summary>
/// <returns>Vracia void</returns>
template <class T>
void cArray<T>::AllocateNextMemory() 
{
	cMemoryBlock * tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position );

	if( tmp_cMemoryBlock->Get_SignSize() == 'B' )														// ak je posledny block BIG, beriem BIG
	{
		end_array_position = 0;																			// nastavim size na zaciatok pola
		amb_actual_position += 1;																		// nastavim aktualnu poziciu v poli blokov pamate

		*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);
	}
	else
	{
		end_array_position = 0;																			// nastavim size na zaciatok pola
		amb_actual_position += 1;																		// nastavim aktualnu poziciu v poli blokov pamate

		*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::SMALL_SIZE);
	}
}


/// <summary>Metoda Clear uvolni pamat na zaklade vstupnej premennej, ak sa vstupna premenna nezada, uvolnia sa vsetky bloky okrem prveho.</summary>
/// <param name="count">Definuje pocet prvkov, ktore maju zostat v datovej strukture.</param>
/// <returns>Vracia void</returns>
template <class T>
void cArray<T>::Clear(unsigned int count = 0)
{
	unsigned int count_of_keep_blocks = 0;																// 0 preto, lebo pt na bloki pamati zacinaju od 0 (ak je count_of_keep_blocks=0 ostava 1 blok)
	end_array_position = 0;																				// nastavim ukazovatel na koniec pola v aktualnom bloku na 0

	if( count > 0 && count < (items_per_block * (amb_actual_position+1) ) )
	{
		count_of_keep_blocks = count / items_per_block;
		end_array_position = count - (count_of_keep_blocks * items_per_block);									// nastavim ukazovatel na koniec pola v aktualnom bloku
	}

	// uvolnujem vsetku pamat mimo prvy blok
	for(unsigned int i = amb_actual_position; i > count_of_keep_blocks; i--)
	{
		mem_manager->Release_memory( *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+i ) );
	}

	amb_actual_position = count_of_keep_blocks;															// nastavim aktualnu poziciu v poli blokov pamate
}


/// <summary>Metoda ClearAll finalizuje objekt triedy. Vrati spravcovi vsetky pamatove bloky, ktorych bola vlastnikom.</summary>
/// <returns>Vracia void</returns>
template <class T>
void cArray<T>::ClearAll()
{
	// uvolnujem vsetku pamat
	for(unsigned int i = 0; i <= amb_actual_position; i++)
	{
		mem_manager->Release_memory( *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+i ) );
	}

	mem_manager->Release_memory(cMemoryBlocks_managment);
}


/// <summary>Metoda Add vlozi prvok predany parametrom metody na koniec pola. V pripade, ze su bloky sukromneho pamatoveho poolu vsetky obsadene, zavola sa metoda AllocateNextMemory.</summary>
/// <param name="value">Hodnota vkladaneho prvku.</param>
/// <returns>Vracia void</returns>
template <class T>
void cArray<T>::Add(const T &value)
{
	cMemoryBlock * tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position );

	if( ((T *)tmp_cMemoryBlock->GetMemAddressBegin() + items_per_block) <= ((T *)tmp_cMemoryBlock->GetMemAddressBegin() + end_array_position) )
	{
		AllocateNextMemory();
		tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position );
		
		*( (T *)tmp_cMemoryBlock->GetMemAddressBegin() + end_array_position ) = value;
	}
	else
	{
		*( (T *)tmp_cMemoryBlock->GetMemAddressBegin() + end_array_position ) = value;
	}
	end_array_position++;
}


/// <summary>Pretazeny operator pre pristu k prvkom pola. Vo vnutri metody dochadza k aritmetike ukazovatelov, kde sa vypocitava pozicia prvku v sukromnom pam.poole.</summary>
/// <param name="index">Pozicia prvku ku ktoremu pristupujeme.</param>
/// <returns>Vracia T&</returns>
template <class T>
T& cArray<T>::operator[](unsigned int index)
{
	cMemoryBlock * tmp_cMemoryBlock;
	
	if(amb_actual_position > 0)															// kontrolujem, ci cArray ma alokovanych viac ako jeden blok pamate
	{
		int tmp_index = 0;

		unsigned int memblock_position = index / (items_per_block);						// zistim v ktorom bloku pamate sa nachadza polozka/pozicia pola

		tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+memblock_position );
		
		if( memblock_position <= amb_actual_position )									// kontrolujem, ci zadany index (parameter funkcie) neprekracuje aktualnu velkost pola
		{
			tmp_index = index - (memblock_position * (items_per_block) );				// odcitam od zadaneho indexu pocet poloziek v predchadzajucich pam.blokoch. Takto ziskam poziciu v pozadovanom bloku
		}
		return *( (T *)tmp_cMemoryBlock->GetMemAddressBegin() + tmp_index );
	}
	else
	{
		tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position );

		return *( (T *)tmp_cMemoryBlock->GetMemAddressBegin() + index );
	}
}


// Vlastnosti triedy

/// <summary>Metoda GetSize vracia velkost pola.</summary>
/// <returns>Vracia unsigned int</returns>
template <class T>
unsigned int cArray<T>::GetSize()
{
	unsigned int real_array_size = (amb_actual_position * items_per_block) + end_array_position;		// pocitam realnu velkost pola (skutocny pocet poloziek v poli)
	return real_array_size;
}


/// <summary>Metoda GetCountOfUsedMemBlocks vracia pocet pamatovych blokov v sukromnom pamatovom poole datovej struktury.</summary>
/// <returns>Vracia unsigned long int</returns>
template<class T>
unsigned long int cArray<T>::GetCountOfUsedMemBlocks()
{
	return (amb_actual_position+1);
}


/// <summary>Metoda GetSignBlockSize vracia priznak velkosti (S,B,T) pamatovych blokov, z ktorych je tvoreny sukromny pam.pool datovej struktury.</summary>
/// <returns>Vracia char</returns>
template<class T>
char cArray<T>::GetSignBlockSize()
{
	cMemoryBlock * tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin() );

	return tmp_cMemoryBlock->Get_SignSize();
}
}}


