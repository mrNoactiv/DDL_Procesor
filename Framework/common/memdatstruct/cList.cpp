#include "common/memdatstruct/cList.h"

namespace common {
	namespace memdatstruct {

/// <summary>Prazdny konstruktor triedy</summary>
template <class T>
cList<T>::cList() {}


/// <summary>Konstruktor triedy prijma jeden parameter, odkaz na spravcu pamate. Inicializuje premenne triedy, poziada o prvy blok pamate pre administraciu a 
/// prvy blok pamatoveho poolu. Implicitne pracuje datova strukura pri inicializacii tymto konstruktorom so SMALL pamatovymi blokmi.</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
template <class T>
cList<T>::cList(cMemoryManager * mmanager)
{
	mem_manager = mmanager;
	
	cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);
	
	end_array_position = 0;
	amb_actual_position = 0;				// aktualna pozicia v poli pridelenych blokov
	type_range = sizeof(MF_Node<T>);			// nastavim rozsah datoveho bloku (pocet blokov v pamati)
	
	
	*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request();
	max_block_size = mem_manager->GetSize_BIG() - type_range - 1;

	CountItemsPerBlock();

	// pre zoznam
	head = NULL;
	tail = NULL;
	size = 0;
}


/// <summary>Konstruktor triedy prijma dva parametere, odkaz na spravcu pamate a priznak velkosti bloku. Inicializuje premenne triedy, poziada o prvy blok pamate pre 
/// administraciu a prvy blok pamatoveho poolu.</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <param name="sign_block_size">Priznak velkosti bloku, ktore budu v sukromnom pamatovom poole strukturz</param>
template <class T>
cList<T>::cList(cMemoryManager * mmanager, char sign_block_size)
{
	mem_manager = mmanager;

	cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);
	
	end_array_position = 0;
	amb_actual_position = 0;		// aktualna pozicia v poli pridelenych blokov
	type_range = sizeof(MF_Node<T>);			// nastavim rozsah datoveho bloku (pocet blokov v pamati)

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

	// pre zoznam
	head = NULL;
	tail = NULL;
	size = 0;
}


/// <summary>Konstruktor triedy prijma tri parametre, odkaz na spravcu pamate, priznak velkosti bloku a info ci pouzivat systemovy blok. Inicializuje premenne 
/// triedy, poziada o prvy blok pamate pre administraciu a prvy blok pamatoveho poolu.</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <param name="sign_block_size">Priznak velkosti bloku, ktore budu v sukromnom pamatovom poole strukturz</param>
/// <param name="use_system_block">Priznak, ci pouzivat pre administraciu SYSTEM pamatovy blok. TRUE-pouzivat, FALSE-nepouzivat</param>
template <class T>
cList<T>::cList(cMemoryManager * mmanager, char sign_block_size, bool use_system_block)
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
	amb_actual_position = 0;				// aktualna pozicia v poli pridelenych blokov
	type_range = sizeof(MF_Node<T>);		// nastavim rozsah datoveho bloku (pocet blokov v pamati)

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

	// pre zoznam
	head = NULL;
	tail = NULL;
	size = 0;
}


/// <summary>Destruktor triedy vola metodu ClearAll</summary>
template <class T>
cList<T>::~cList(void)
{
	ClearAll();
}


/// <summary>Inicializacna metoda Init sluzi k inicializacii dat.struktury, ak bola vytvorena prazdnym konstruktorom. Metoda prijma jeden 
/// parameter (odkaz na spravcu pamate) a implicitne je sukromny pamatovy pool struktury tvoreny SMALL blokmi.</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <returns>Vracia void</returns>
template <class T>
void cList<T>::Init(cMemoryManager * mmanager)
{
	mem_manager = mmanager;
	
	cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);
	
	end_array_position = 0;
	amb_actual_position = 0;				// aktualna pozicia v poli pridelenych blokov
	type_range = sizeof(MF_Node<T>);			// nastavim rozsah datoveho bloku (pocet blokov v pamati)
	
	
	*((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position) = mem_manager->Memory_request();
	max_block_size = mem_manager->GetSize_BIG() - type_range - 1;

	CountItemsPerBlock();

	// pre zoznam
	head = NULL;
	tail = NULL;
	size = 0;
}


/// <summary>Inicializacna metoda Init sluzi k inicializacii dat.struktury, ak bola vytvorena prazdnym konstruktorom. Metoda prijma dva 
/// parametre (odkaz na spravcu pamate, priznak velkosti pam.bloku).</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <param name="sign_block_size">Priznak velkosti bloku, ktore budu v sukromnom pamatovom poole strukturz (S, B)</param>
/// <returns>Vracia void</returns>
template <class T>
void cList<T>::Init(cMemoryManager * mmanager, char sign_block_size)
{
	mem_manager = mmanager;

	cMemoryBlocks_managment = mem_manager->Memory_request(cMemoryManager::BIG_SIZE);
	
	end_array_position = 0;
	amb_actual_position = 0;			// aktualna pozicia v poli pridelenych blokov
	type_range = sizeof(MF_Node<T>);	// nastavim rozsah datoveho bloku (pocet blokov v pamati)

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

	// pre zoznam
	head = NULL;
	tail = NULL;
	size = 0;
}


/// <summary>Inicializacna metoda Init sluzi k inicializacii dat.struktury, ak bola vytvorena prazdnym konstruktorom. Metoda prijma tri 
/// parametre (odkaz na spravcu pamate, priznak velkosti pam.bloku, info ci pouzivat systemovy blok).</summary>
/// <param name="mmanager">Odkaz na spravcu pamate</param>
/// <param name="sign_block_size">Priznak velkosti bloku, ktore budu v sukromnom pamatovom poole strukturz (S, B)</param>
/// <param name="use_system_block">Priznak, ci pouzivat pre administraciu SYSTEM pamatovy blok. TRUE-pouzivat, FALSE-nepouzivat</param>
/// <returns>Vracia void</returns>
template <class T>
void cList<T>::Init(cMemoryManager * mmanager, char sign_block_size, bool use_system_block)
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
	amb_actual_position = 0;		// aktualna pozicia v poli pridelenych blokov
	type_range = sizeof(MF_Node<T>);			// nastavim rozsah datoveho bloku (pocet blokov v pamati)

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

	// pre zoznam
	head = NULL;
	tail = NULL;
	size = 0;
}





/// <summary>Metoda CountItemsPerBlock nastavi maximalny pocet poloziek, ktore bude mozne ulozit do jedneho bloku pamate.</summary>
/// <returns>Vracia void</returns>
template <class T>
void cList<T>::CountItemsPerBlock()
{
	if( (max_block_size % type_range) == 0 )														// v pripade ze je delitelne bezzvysku
	{
		items_per_block = (max_block_size / type_range) + 1;
	}
	else
	{
		items_per_block = (max_block_size / type_range) + 2;
	}
}


/// <summary>Metoda AllocateNextMemory ziada o dalsi blok pamate prislusnej velkosti a blok zaradi do sukromneho pamatoveho poolu.</summary>
/// <returns>Vracia void</returns>
template <class T>
void cList<T>::AllocateNextMemory()
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


/// <summary>Metoda ClearAll finalizuje objekt triedy. Vrati spravcovi vsetky pamatove bloky, ktorych bola vlastnikom.</summary>
/// <returns>Vracia void</returns>
template<class T>
void cList<T>::ClearAll()
{
	// uvolnujem vsetku pamat
	for(unsigned int i = 0; i <= amb_actual_position; i++)
	{
		mem_manager->Release_memory( *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+i ) );
	}

	mem_manager->Release_memory(cMemoryBlocks_managment);
}



/* ************************************************************************************************** */
/* ************************************* METODY PRE ZOZNAM ****************************************** */
/* ************************************************************************************************** */

/// <summary>Metoda Add vlozi prvok predany ako parameter na koniec zoznamu. V pripade, ze su bloky sukromneho pamatoveho poolu vsetky obsadene, zavola sa metoda AllocateNextMemory.</summary>
/// <param name="value">Hodnota vkladaneho prvku.</param>
/// <returns>Vracia void</returns>
template <class T>
void cList<T>::Add(const T &value)
{
	cMemoryBlock * tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position );

	if( ((MF_Node<T> *)tmp_cMemoryBlock->GetMemAddressBegin() + items_per_block) <= ((MF_Node<T> *)tmp_cMemoryBlock->GetMemAddressBegin() + end_array_position) )
	{
		AllocateNextMemory();
		tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+amb_actual_position );
	}

	// Vytvorim a zaradim dalsi uzol zoznamu
	MF_Node<T> * pt_node = (MF_Node<T> *)tmp_cMemoryBlock->GetMemAddressBegin() + end_array_position;
	pt_node->Init(value);

	if(tail == NULL)
	{
		tail = pt_node;
		head = pt_node;
	}
	else
	{
		tail->next = pt_node;
		pt_node->previous = tail;
		tail = pt_node;
	}

	size++;
	end_array_position++;
}


/// <summary>Metoda GetHeadNodesData vracia hodnotu prveho(head) uzlu zoznamu.</summary>
/// <returns>T&</returns>
template <class T>
T& cList<T>::GetHeadNodesData()
{
	return head->data;
}


/// <summary>Metoda GetHeadNodesDataAndRemove vracia hodnotu prveho(head) uzlu zoznamu a zaroven uzol odobera zo zoznamu. Pouzitie metody je u realizacie fronty.</summary>
/// <returns>T&</returns>
template <class T>
T cList<T>::GetHeadNodesDataAndRemove()
{
	T tmp = head->data;
	Erase(head);
	return tmp;
}


/// <summary>Metoda GetTailNodesData vracia hodnotu posledneho(tail) uzlu zoznamu.</summary>
/// <returns>T&</returns>
template <class T>
T& cList<T>::GetTailNodesData()
{
	return tail->data;
}


/// <summary>Metoda GetNodesData vracia hodnotu uzlu zoznamu na pozicii zadanej parametrom metody.</summary>
/// <param name="position">Urcuje poziciu uzlu v zozname</param>
/// <returns>T&</returns>
template <class T>
T& cList<T>::GetNodesData(int position)
{
	MF_Node<T> * pt_tmp_node = head;
	int i = 1;										// 2 preto, lebo v pt_tmp_node uz sa nachadza 1 prvok. Ak by sme brali cislovanie prvkov zozname, kde 1.prvok bude 0, tak je nutne zmenit int i = 1

	while(i < position && pt_tmp_node != NULL)
	{
		pt_tmp_node = pt_tmp_node->next;
		i++;
	}
	
	return pt_tmp_node->data;
}


/// <summary>Metoda odstrani uzol zoznamu zadaneho ako parameter metody.</summary>
/// <param name="pt_node">Urcuje uzol, ktory ma by odstraneny zo zoznamu</param>
/// <returns>Vracia void</returns>
template <class T>
void cList<T>::Erase(MF_Node<T> * pt_node)
{
	if(pt_node->previous == NULL && pt_node->next == NULL)
	{
		head = NULL;
		tail = NULL;
	}
	else if(pt_node->previous == NULL)
	{
		head = pt_node->next;
		pt_node->next->previous = NULL;
	}
	else if(pt_node->next == NULL)
	{
		tail = pt_node->previous;
		pt_node->previous->next = NULL;
	}
	else
	{
		pt_node->previous->next = pt_node->next;
		pt_node->next->previous = pt_node->previous;
	}
	size--;
}


/// <summary>Metoda odstrani uzol zoznamu na pozicii zadanej ako parameter metody.</summary>
/// <param name="position">Urcuje poziciu uzlu, ktory ma by odstraneny zo zoznamu</param>
/// <returns>Vracia void</returns>
template <class T>
void cList<T>::EraseOnPosition(int position)
{
	if(position < 0 || position > this->size-1)
	{
		return;
	}

	MF_Node<T> * pt_tmp_node = head;
	for(int i = 1; i < position; i++)
	{
		pt_tmp_node = pt_tmp_node->next;
	}
	Erase(pt_tmp_node);
}


/// <summary>Metoda odstrani uzly zoznamu na poziciach od-do zadanychako ako parameter metody.</summary>
/// <param name="position_from">Urcuje poziciu uzlu od, ktory ma by odstraneny zo zoznamu</param>
/// <param name="position_from">Urcuje poziciu uzlu o,o ktory ma by odstraneny zo zoznamu</param>
/// <returns>Vracia void</returns>
template <class T>
void cList<T>::EraseOnPosition(int position_from, int position_to)
{
	if( position_from < 0 || position_to > this->size-1 || position_from <= position_to)
	{
		return;
	}

	MF_Node<T> * pt_tmp_node = head;
	int i = 0;
	while(i < position_to && pt_tmp_node != NULL)
	{
		if( (i >= position_from-1) && pt_tmp_node->previous != NULL )
		{
			MF_Node<T> * pt_tmp_node2 = pt_tmp_node;
			pt_tmp_node = pt_tmp_node->previous;

			Erase(pt_tmp_node2);
			position_to--;
		}
		else if((i >= position_from-1) && pt_tmp_node->previous == NULL)
		{
			MF_Node<T> * pt_tmp_node2 = pt_tmp_node;
			Erase(pt_tmp_node2);
			position_to--;

			pt_tmp_node = head;
		}
		else
		{
			pt_tmp_node = pt_tmp_node->next;
		}

		i++;
	}
}


/// <summary>Metoda odstrani uzly zo zoznamu na zaklade ich dat zadanych ako ako parameter metody.</summary>
/// <param name="data">Urcuje data uzlu, ktore maju byt odstranene zo zoznamu</param>
/// <returns>Vracia void</returns>
template <class T>
void cList<T>::Erase(T data)
{
	MF_Node<T> * pt_tmp_node = head;

	while(pt_tmp_node != NULL)
	{
		if(pt_tmp_node->data == data)
		{
			MF_Node<T> * pt_tmp_node2 = pt_tmp_node;
			pt_tmp_node = pt_tmp_node->previous;

			Erase(pt_tmp_node2);
		}
		else
		{
			pt_tmp_node = pt_tmp_node->next;
		}
	}
}



/// <summary>Metoda Clear uvolni pamat na zaklade vstupnej premennej, ak sa vstupna premenna nezada, uvolnia sa vsetky bloky okrem prveho.</summary>
/// <param name="count">Definuje pocet uzlov, ktore maju zostat v datovej strukture.</param>
/// <returns>Vracia void</returns>
template <class T>
void cList<T>::Clear(unsigned int count = 0)
{
	unsigned int count_of_keep_blocks = 0;													// 0 preto, lebo pt na bloki pamati zacinaju od 0 (ak je count_of_keep_blocks=0 ostava 1 blok)
	end_array_position = 0;																	// nastavim size na zaciatok

	if( count > 0 && count < (items_per_block * (amb_actual_position+1) ) )
	{
		count_of_keep_blocks = count / items_per_block;
		end_array_position = count - (count_of_keep_blocks * items_per_block);

		int position = size;

		while(pt_tmp_node != NULL && position >= count)
		{
			Erase(tail);
			position--;
		}
	}
	else
	{
		head = NULL;
		tail = NULL;
		size = 0;
	}

	// uvolnujem vsetku pamat mimo prvy blok
	for(unsigned int i = amb_actual_position; i > count_of_keep_blocks; i--)
	{
		mem_manager->Release_memory( *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin()+i ) );
	}
																				
	amb_actual_position = count_of_keep_blocks;				
}


/// <summary>Metoda GetSize vracia velkost zoznamu.</summary>
/// <returns>Vracia unsigned int</returns>
template <class T>
int cList<T>::GetSize()
{
	return this->size;
}


/// <summary>Metoda IsEmpty vracia logicku hodnotu, ci je zoznam prazdny alebo nie.</summary>
/// <returns>Vracia bool. TRUE-je prazdny, FALSE-nie je prazdny</returns>
template <class T>
bool cList<T>::IsEmpty()
{
	if(size == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/// <summary>Metoda GetCountOfUsedMemBlocks vracia pocet pamatovych blokov v sukromnom pamatovom poole datovej struktury.</summary>
/// <returns>Vracia unsigned long int</returns>
template<class T>
unsigned long int cList<T>::GetCountOfUsedMemBlocks()
{
	return (amb_actual_position+1);
}


/// <summary>Metoda GetSignBlockSize vracia priznak velkosti (S,B,T) pamatovych blokov, z ktorych je tvoreny sukromny pam.pool datovej struktury.</summary>
/// <returns>Vracia char</returns>
template<class T>
char cList<T>::GetSignBlockSize()
{
	cMemoryBlock * tmp_cMemoryBlock = *((cMemoryBlock**)cMemoryBlocks_managment->GetMemAddressBegin() );

	return tmp_cMemoryBlock->Get_SignSize();
}



/// <summary>Metoda ShowNodes vypise hodnoty uzlov zoznamu.</summary>
/// <returns>Vracia void</returns>
template <class T>
void cList<T>::ShowNodes()
{
	std::cout << "Vypis prvkov zoznamu:\n";

	MF_Node<T> * pt_tmp_node = head;
	int position = 1;
	while(pt_tmp_node != NULL)
	{
		std::cout << "Pozicia " << position << ": " << pt_tmp_node->data << "\n";
		pt_tmp_node = pt_tmp_node->next;
		
		position++;
	}
}
}}