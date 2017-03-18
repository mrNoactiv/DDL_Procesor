#include "common/memdatstruct/cArray2.h"

namespace common {
	namespace memdatstruct {

/// <summary>Prazdny konstruktor triedy</summary>
template <class T>
cArray2<T>::cArray2() {}


/// <summary>Konstruktor triedy prijma jeden parameter, odkaz na spravcu pamate. Inicializuje premenne triedy, poziada o prvy blok pamate pre administraciu a 
/// prvy blok pamatoveho poolu. Nasledne sa alokuje pole ukazovatelov, ktore sa inicializuju, ako ukazovatele do pamate blokov sukromneho pam.poolu. 
/// Implicitne pracuje datova strukura pri inicializacii tymto konstruktorom s BIG pamatovymi blokmi.</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
template <class T>
cArray2<T>::cArray2(cMemoryManager * mmanager)
{
	mem_manager = mmanager;
	
	cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);

	amb_actual_position = 0;
	end_array_position = 0;
	type_range = sizeof(T);

	*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);

	max_block_size = mem_manager->GetSize_BIG() - type_range - 1;
	CountItemsPerBlock();

	// inicializujem a naplnim arrays_blocks
	sizeof_arrays_blocks = items_per_block;															// nastavim pociatocnu velkost pola arrays_block
	arrays_blocks = new T* [sizeof_arrays_blocks];

	cMemoryBlock * tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position );
	for(unsigned int i = 0; i < sizeof_arrays_blocks; i++)
	{
		arrays_blocks[i] =  (T *)tmp_cMemoryBlock->GetMemAddressBegin()+i;
	}
	tmp_cMemoryBlock = 0;
}


/// <summary>Konstruktor triedy prijma dva parametere, odkaz na spravcu pamate a priznak velkosti bloku. Inicializuje premenne triedy, poziada o prvy blok pamate pre 
/// administraciu a prvy blok pamatoveho poolu. Nasledne sa alokuje pole ukazovatelov, ktore sa inicializuju, ako ukazovatele do pamate blokov sukromneho pam.poolu. </summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <param name="sign_block_size">Priznak velkosti bloku, ktore budu v sukromnom pamatovom poole dat struktury</param>
template <class T>
cArray2<T>::cArray2(cMemoryManager * mmanager, char sign_block_size)
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

	// inicializujem a naplnim arrays_blocks
	sizeof_arrays_blocks = items_per_block;															// nastavim pociatocnu velkost pola arrays_block
	arrays_blocks = new T* [sizeof_arrays_blocks];
	
	cMemoryBlock * tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position );
	for(unsigned int i = 0; i < sizeof_arrays_blocks; i++)
	{
		arrays_blocks[i] =  (T *)tmp_cMemoryBlock->GetMemAddressBegin()+i;
	}
	tmp_cMemoryBlock = 0;
}


/// <summary>Konstruktor triedy prijma tri parametre, odkaz na spravcu pamate, priznak velkosti bloku a info ci pouzivat systemovy blok. Inicializuje premenne 
/// triedy, poziada o prvy blok pamate pre administraciu a prvy blok pamatoveho poolu. Nasledne sa alokuje pole ukazovatelov, ktore sa inicializuju, ako ukazovatele do pamate blokov sukromneho pam.poolu.</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <param name="sign_block_size">Priznak velkosti bloku, ktore budu v sukromnom pamatovom poole struktury</param>
/// <param name="use_system_block">Priznak, ci pouzivat pre administraciu SYSTEM pamatovy blok. TRUE-pouzivat, FALSE-nepouzivat</param>
template <class T>
cArray2<T>::cArray2(cMemoryManager * mmanager, char sign_block_size, bool use_system_block)
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

	// inicializujem a naplnim arrays_blocks
	sizeof_arrays_blocks = items_per_block;															// nastavim pociatocnu velkost pola arrays_block
	arrays_blocks = new T* [sizeof_arrays_blocks];
	
	cMemoryBlock * tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position );
	for(unsigned int i = 0; i < sizeof_arrays_blocks; i++)
	{
		arrays_blocks[i] =  (T *)tmp_cMemoryBlock->GetMemAddressBegin()+i;
	}
	tmp_cMemoryBlock = 0;
}


/// <summary>Destruktor triedy vola metodu ClearAll</summary>
template <class T>
cArray2<T>::~cArray2(void)
{
	ClearAll();
}


/// <summary>Inicializacna metoda Init sluzi k inicializacii dat.struktury, ak bola vytvorena prazdnym konstruktorom. Metoda prijma jeden 
/// parameter (odkaz na spravcu pamate) a implicitne je sukromny pamatovy pool struktury tvoreny BIG blokmi.
/// Nasledne sa alokuje pole ukazovatelov, ktore sa inicializuju, ako ukazovatele do pamate blokov sukromneho pam.poolu.</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <returns>Vracia void</returns>
template<class T>
void cArray2<T>::Init(cMemoryManager * mmanager)
{
	mem_manager = mmanager;
	
	cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);

	amb_actual_position = 0;
	end_array_position = 0;
	type_range = sizeof(T);

	*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);

	max_block_size = mem_manager->GetSize_BIG() - type_range - 1;
	CountItemsPerBlock();

	// inicializujem a naplnim arrays_blocks
	sizeof_arrays_blocks = items_per_block;															// nastavim pociatocnu velkost pola arrays_block
	arrays_blocks = new T* [sizeof_arrays_blocks];

	cMemoryBlock * tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position );
	for(unsigned int i = 0; i < sizeof_arrays_blocks; i++)
	{
		arrays_blocks[i] =  (T *)tmp_cMemoryBlock->GetMemAddressBegin()+i;
	}
	tmp_cMemoryBlock = 0;
}


/// <summary>Inicializacna metoda Init sluzi k inicializacii dat.struktury, ak bola vytvorena prazdnym konstruktorom. Metoda prijma dva 
/// parametre (odkaz na spravcu pamate, priznak velkosti pam.bloku). Nasledne metoda alokuje pole ukazovatelov, ktore sa inicializuju, ako ukazovatele do pamate blokov sukromneho pam.poolu.</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <param name="sign_block_size">Priznak velkosti bloku, ktore budu v sukromnom pamatovom poole struktury (S, B)</param>
/// <returns>Vracia void</returns>
template<class T>
void cArray2<T>::Init(cMemoryManager * mmanager, char sign_block_size)
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

	// inicializujem a naplnim arrays_blocks
	sizeof_arrays_blocks = items_per_block;															// nastavim pociatocnu velkost pola arrays_block
	arrays_blocks = new T* [sizeof_arrays_blocks];
	
	cMemoryBlock * tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position );
	for(unsigned int i = 0; i < sizeof_arrays_blocks; i++)
	{
		arrays_blocks[i] =  (T *)tmp_cMemoryBlock->GetMemAddressBegin()+i;
	}
	tmp_cMemoryBlock = 0;
}


/// <summary>Inicializacna metoda Init sluzi k inicializacii dat.struktury, ak bola vytvorena prazdnym konstruktorom. Metoda prijma tri 
/// parametre (odkaz na spravcu pamate, priznak velkosti pam.bloku, info ci pouzivat systemovy blok). 
/// Nasledne metoda alokuje pole ukazovatelov, ktore sa inicializuju, ako ukazovatele do pamate blokov sukromneho pam.poolu.</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <param name="sign_block_size">Priznak velkosti bloku, ktore budu v sukromnom pamatovom poole strukturz (S, B)</param>
/// <param name="use_system_block">Priznak, ci pouzivat pre administraciu SYSTEM pamatovy blok. TRUE-pouzivat, FALSE-nepouzivat</param>
/// <returns>Vracia void</returns>
template <class T>
void cArray2<T>::Init(cMemoryManager * mmanager, char sign_block_size, bool use_system_block)
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

	// inicializujem a naplnim arrays_blocks
	sizeof_arrays_blocks = items_per_block;															// nastavim pociatocnu velkost pola arrays_block
	arrays_blocks = new T* [sizeof_arrays_blocks];
	
	cMemoryBlock * tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position );
	for(unsigned int i = 0; i < sizeof_arrays_blocks; i++)
	{
		arrays_blocks[i] =  (T *)tmp_cMemoryBlock->GetMemAddressBegin()+i;
	}
	tmp_cMemoryBlock = 0;
}


/// <summary>Metoda nastavi maximalny pocet poloziek, ktore bude mozne ulozit do jedneho bloku pamate.</summary>
/// <returns>Vracia void</returns>
template <class T>
void cArray2<T>::CountItemsPerBlock()
{
	if( (max_block_size % type_range) == 0 )														// v pripade ze je delitelne bezzvysku
	{
		items_per_block = (max_block_size / type_range) + 1;
	}
	else
	{
		items_per_block = (max_block_size / type_range);
	}
}


/// <summary>Metoda Reallocate ziada o dalsi blok pamate prislusnej velkosti a blok zaradi do sukromneho pamatoveho poolu. Nasledne
/// sa reallokuje stavajuce pole arrays_block</summary> 
/// <returns>Vracia void</returns>
template <class T>
void cArray2<T>::Reallocate()
{
	amb_actual_position++;																			// posuniem ukazovatel v poli cMemoryBlocks na dalsiu poziciu

	sizeof_arrays_blocks = items_per_block * (amb_actual_position+1);								// nastavim docasnu premennu, velkost pola tmp_arrays_blocks
	delete [] arrays_blocks;
	arrays_blocks = new T *[sizeof_arrays_blocks];													// vyalokujem nove pole ukazovatelov

	if(GetSignBlockSize() == 'B')
	{
		// ziadam o pridelenie noveho bloku pamate B
		*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);
	}
	else
	{
		// ziadam o pridelenie noveho bloku pamate S
		*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request(cMemoryManager::SMALL_SIZE);	
	}

	// inicializujem ukazatelov noveho tmp pola ukazovatelmi vsetkych blokov
	cMemoryBlock * tmp_cMemoryBlock = 0;
	int k = 0;
	for(unsigned int i = 0; i <= amb_actual_position; i++)
	{
		tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+i); 

		for(unsigned j = 0; j < items_per_block; j++, k++)
		{
			arrays_blocks[k] = (T *)tmp_cMemoryBlock->GetMemAddressBegin() + j;
		}
	}
	tmp_cMemoryBlock = 0;
}


/// <summary>Metoda Clear uvolni pamat na zaklade vstupnej premennej, ak sa vstupna premenna nezada, uvolnia sa vsetky bloky okrem prveho.</summary>
/// <param name="count">Definuje pocet prvkov, ktore maju zostat v datovej strukture</param>
/// <returns>Vracia void</returns>
template <class T>
void cArray2<T>::Clear(unsigned int count = 0)
{
	unsigned int count_of_keep_blocks = 0;											// 0 preto, lebo pt na bloki pamati zacinaju od 0 (ak je count_of_keep_blocks=0 ostava 1 blok)
	end_array_position = 0;															// nastavim size na zaciatok pola

	if( count > 0 && count < (items_per_block * (amb_actual_position+1) ) )
	{
		count_of_keep_blocks = count / items_per_block;	
		end_array_position = count;													// nastavim size na zaklade vstupneho parametru
	}

	// uvolnujem vsetku pamat mimo prvy blok
	for(unsigned int i = amb_actual_position; i > count_of_keep_blocks; i--)
	{
		mem_manager->Release_memory( *( (cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+i ) );
	}

	amb_actual_position = count_of_keep_blocks;										// nastavim aktualnu poziciu v poli blokov pamate

	sizeof_arrays_blocks = items_per_block * (amb_actual_position+1);				// nastavim pociatocnu velkost pola arrays_block
	delete [] arrays_blocks;
	arrays_blocks = new T *[sizeof_arrays_blocks];									// vyalokujem nove pole ukazovatelov

	// inicializujem ukazatelov noveho tmp pola ukazovatelmi vsetkych blokov
	cMemoryBlock * tmp_cMemoryBlock = 0;
	int k = 0;

	for(unsigned int i = 0; i <= amb_actual_position; i++)
	{
		tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+i);

		for(unsigned j = 0; j < items_per_block; j++, k++)
		{
			arrays_blocks[k] = (T *)tmp_cMemoryBlock->GetMemAddressBegin() + j;
		}
	}
	tmp_cMemoryBlock = 0;
}


/// <summary>Metoda ClearAll finalizuje objekt triedy. Vrati spravcovi vsetky pamatove bloky, ktorych bola vlastnikom. Vracia na haldu i 
/// pamat alokovanu pre pole ukayovatelov.</summary>
/// <returns>Vracia void</returns>
template <class T>
void cArray2<T>::ClearAll()
{
	// uvolnujem vsetku pamat
	for(unsigned int i = 0; i <= amb_actual_position; i++)
	{
		mem_manager->Release_memory( *( (cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+i ) );
	}

	mem_manager->Release_memory(cMemoryBlocks_managment);

	// rusim pole ukazatelov na char
	delete [] arrays_blocks;
}


/// <summary>Metoda Add vlozi prvok predany parametrom metody na koniec pola. V pripade, ze su bloky sukromneho pamatoveho poolu 
/// vsetky obsadene, zavola sa metoda Reallocate.</summary>
/// <param name="value">Hodnota vkladaneho prvku.</param>
/// <returns>Vracia void</returns>
template <class T>
void cArray2<T>::Add(const T &value)
{
	if( (end_array_position+2) >= sizeof_arrays_blocks )		// ak je momentalne alokovana pamat plna Realokujem. testujem +2, pretoze premenna end_array_position je ukazatel v poli - zacina od 0
	{
		Reallocate();
	}

	*arrays_blocks[end_array_position] = value;
	end_array_position++;
}


/// <summary>Metoda Add vlozi prvok predany prvym parametrom metody na poziciu v poli predanu parametrom druhym. V pripade, ze su bloky sukromneho pamatoveho poolu 
/// vsetky obsadene, zavola sa metoda Reallocate.</summary>
/// <param name="value">Hodnota vkladaneho prvku</param>
/// <param name="position">Pozicia na ktoru sa ma vlozit prvok</param>
/// <returns>Vracia void</returns>
template <class T>
void cArray2<T>::Add(const T &value, unsigned int position)
{
	if(position > 0)
	{
		if( (end_array_position+2) >= sizeof_arrays_blocks)		// ak je momentalne alokovana pamat plna Realokujem. testujem +2, pretoze premenna end_array_position je ukazatel v poli - zacina od 0
		{
			Reallocate();
		}

		for(unsigned int i = end_array_position; i >= position; i--)
		{
			*arrays_blocks[i] = *arrays_blocks[i-1];
		}
		*arrays_blocks[position] = value;
	}
}


/// <summary>Metoda Remove odstrani prvok na pozicii zadanej ako pramater metody.</summary>
/// <param name="position">Pozicia z ktorej sa ma prvok odstranit</param>
/// <returns>Vracia void</returns>
template <class T>
void cArray2<T>::Remove(unsigned int position)
{
	if(position >= 0)
	{
		for(unsigned int i = position; i < (end_array_position-2); i++)		// -2 pretoze posuvam polozky do lava a end_array_position ukazuje na prvu volnu polozku 
		{
			*arrays_blocks[i] = *arrays_blocks[i+1];
		}
		end_array_position--;									// posuniem ukazovatel dolava - zmensim pole
	}
}


/// <summary>Pretazeny operator pre pristu k prvkom pola. Metoda pristupuje k prvkom prostrednicvom pola ukazovatelov.</summary>
/// <param name="index">Pozicia prvku ku ktoremu pristupujeme.</param>
/// <returns>Vracia T&</returns>
template <class T>
T& cArray2<T>::operator[](unsigned int index)
{
	if(index <= sizeof_arrays_blocks)
	{
		return *arrays_blocks[index];
	}
	else
	{
		return *arrays_blocks[items_per_block];
	}
}


// Vlastnosti triedy


/// <summary>Metoda GetSize vracia velkost pola.</summary>
/// <returns>Vracia unsigned int</returns>
template <class T>
unsigned int& cArray2<T>::GetSize()
{
	return end_array_position;
}


/// <summary>Metoda GetCountOfUsedMemBlocks vracia pocet pamatovych blokov v sukromnom pamatovom poole datovej struktury.</summary>
/// <returns>Vracia unsigned long int</returns>
template<class T>
unsigned long int cArray2<T>::GetCountOfUsedMemBlocks()
{
	return (amb_actual_position+1);
}


/// <summary>Metoda GetSignBlockSize vracia priznak velkosti (S,B,T) pamatovych blokov, z ktorych je tvoreny sukromny pam.pool datovej struktury.</summary>
/// <returns>Vracia char</returns>
template<class T>
char cArray2<T>::GetSignBlockSize()
{
	cMemoryBlock * tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin() );

	return tmp_cMemoryBlock->Get_SignSize();
}
}}