// cBlockLinkedList
#pragma once

#include "common/memdatstruct/cMemoryBlock.h"
#include <stdlib.h>

namespace common {
	namespace memdatstruct {

class cBlockLinkedList
{
	private:
		cMemoryBlock * front;
		cMemoryBlock * end;
		int size;
		int count_of_pointers;

	public:
		cBlockLinkedList(const int &count_of_blocks);
		~cBlockLinkedList(void);

		inline void AppendNode(cMemoryBlock * data);
		inline void AppendNode(cMemoryBlock * startBlock,cMemoryBlock * endBlock, unsigned int blockCount);
		inline cMemoryBlock * GetFirstNode();
		void ReallocateList(int extend_size);
		void ResetList();
		inline bool IsEmpty();
		inline int GetSize();
		inline int GetCountOfPointers(void);
};

/// <summary>Metoda AppendNode sluzi k zaradeniu pam.bloku na koniec zoznamu (fronty). Metoda prijma jeden parameter.</summary>
/// <param name="data">Ukazovatel na blok pamate, ktory sa bude zaradovat do zoznamu (fronty)</param>
void cBlockLinkedList::AppendNode(cMemoryBlock * block)
{
	block->Set_is_free(true);

	if(end == NULL)
	{
		block->SetPrevious(NULL);
		end = block;
		front = block;
	}
	else
	{
		block->SetPrevious(end);
		end = block;
	}
	size++;
}

void cBlockLinkedList::AppendNode(cMemoryBlock * startBlock,cMemoryBlock * endBlock, unsigned int blockCount)
{
    // POZN - po probehnuti teto metody nemusi byt vsechny bloky ve fronte free ...
	startBlock->Set_is_free(true);
	endBlock->Set_is_free(true);

	if(end == NULL)
	{
		front = endBlock;
		end = startBlock;
	}
	else
	{
		endBlock->SetPrevious(end);
		end = startBlock;
	}
	size += blockCount;
}


/// <summary>Metoda GetFirstNode vrati a odstrani prvy volny pamatovy blok zo zoznamu (fronty). V pripade, ze je fronta prazdna vrati NULL.</summary>
/// <returns>Vracia cMemoryBlock *</returns>
cMemoryBlock * cBlockLinkedList::GetFirstNode()
{
	if( IsEmpty() )
	{
		return NULL;
	}

	//cMemoryBlock* return_block = front->data;
	//NODE * temp = front;

	//if (front->next == NULL)
	//{
	//	front = NULL;
	//	end = NULL;
	//}
	//else
	//{
	//	front = front->next;
	//	front->previous = NULL;
	//}

	cMemoryBlock* return_block = end;
	cMemoryBlock* temp = end;

	if (end->GetPrevious() == NULL)
	{
		front = NULL;
		end = NULL;
	}
	else
	{
		end = end->GetPrevious();
		//end->next = NULL;
	}


	size--;
	
	return_block->Set_is_free(false);
	return return_block;
}



/// <summary>Metoda IsEmpty vracia informaciu o stave pamatoveho poolu (zoznamue). TRUE-zoznam je prazdny, FALSE-zoznam nie je prazdny</summary>
/// <returns>Vracia bool</returns>
bool cBlockLinkedList::IsEmpty()
{
	if(front == NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/// <summary>Metoda GetSize vracia aktualnu velkost zoznamu (front). Jedna sa o pocet uzlov (pamatovych blokov) zoznamu.</summary>
/// <returns>Vracia int</returns>
int cBlockLinkedList::GetSize()
{
	return size;
}


/// <summary>Metoda GetCountOfPointers vracia informaciu o velkosti pola typu NODE, ktore je v aktualnom stave vyalokovane na halde.
/// Pole sa pouziva k zrychleniu prace s uzlami zoznamu (fronty).</summary>
/// <returns>Vracia int</returns>
int cBlockLinkedList::GetCountOfPointers(void)
{
	return count_of_pointers;
}



}}

