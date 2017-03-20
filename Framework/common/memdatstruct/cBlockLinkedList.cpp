#include "common/memdatstruct/cBlockLinkedList.h"
#include "iostream"

using namespace common::memdatstruct;

/// <summary>Konstruktor triedz cBlockLinkedList prijma jeden parameter a inicializuje pole uzlov NODE. Metoda implementuje pamatovy pool
/// pomocou jednosmerneho zoznamu (v podobe fronty). Implementacia zoznamu je trochu odlisna od beznych implementacii, nakolko zoznam
/// nebude pocas behu programu alokovat ziadne objkety na halde. Jedina situacia, kedy dochadza k realokaacii je rozsirenie pam.poolu</summary>
/// <param name="count_of_blocks">Udava pocet uzlov, respektive pamatovych blokov, ktore sa budu inicializovat.</param>
cBlockLinkedList::cBlockLinkedList(const int &count_of_blocks)
{
	front = NULL;
	end = NULL;
	this->size = 0;

	count_of_pointers = count_of_blocks;
	//
	//pt_nodes = new NODE * [this->count_of_pointers];
	//for(int i = 0; i < count_of_pointers; i++)
	//{
	//	pt_nodes[i] = new NODE();
	//}
}


/// <summary>Destruktor triedy</summary>
cBlockLinkedList::~cBlockLinkedList(void)
{
	// uvolnujem vsetky ukazatela na NODE 
	//for(int i = 0; i < count_of_pointers; i++)
	//{
	//	delete pt_nodes[i];
	//}
	//delete [] pt_nodes;
}




/// <summary>Metoda ReallocateList sluzi za ucelom rozsirenia a realokacie pamatoveho poolu. Na halde sa alokuje nove pole uzlov a stavajuce
/// uzly sa skopiruju do noveho pola, ktore sa nastavi za aktualne. Metoda prijme jeden parameter.</summary>
/// <param name="count_of_extend_blocks">Pocet blokov o ktory sa bude zvacsovat pamatovy pool</param>
void cBlockLinkedList::ReallocateList(int count_of_extend_blocks)
{
	//// uvolnujem vsetky ukazatela na NODE
	//for(int i = 0; i < count_of_pointers; i++)
	//{
	//	delete pt_nodes[i];
	//}
	//// uvolnujem pole ukazatelov
	//delete [] pt_nodes;

	//// nastavim premenne triedy reprezentujce vel
	count_of_pointers = count_of_pointers + count_of_extend_blocks;

	//// alokujem nove pole ukazatelov
	//pt_nodes = new NODE * [count_of_pointers];
	//for(int i = 0; i < count_of_pointers; i++)
	//{
	//	pt_nodes[i] = new NODE();
	//}
}


/// <summary>Metoda ResetList sluzi k nulovaniu datovej struktury, kedy cely zoznam vyprazdni.</summary>
void cBlockLinkedList::ResetList()
{
	//NODE * temp = end;
	//while(temp != NULL)
	//{
	//	Node * temp2 = temp;
	//	temp = temp->previous;
	//	delete temp2;
	//}
	end = NULL;
	front = NULL;
	size = 0;
}


