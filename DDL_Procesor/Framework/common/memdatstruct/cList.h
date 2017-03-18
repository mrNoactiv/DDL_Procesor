#pragma once

#include "common/memdatstruct/cMemoryManager.h"
#include "common/memdatstruct/cMemoryBlock.h"

namespace common {
	namespace memdatstruct {

template<typename T>
struct MF_Node
{
	/*struct MF_Node(T * data)
	{
		this->data = data;
		previous = NULL;
		next = NULL;
	}*/

	void Init(T value)
	{
		this->data = value;
		previous = NULL;
		next = NULL;
	}

	T data;
	struct MF_Node * previous;
	struct MF_Node * next;
};

template <class T>
class cList
{
public:
	cList();
	cList(cMemoryManager * mmanager);													// konstruktor prijme odkaz na hlavny objekt mmanageru, default si ziada objekt o BIG blok pamata
	cList(cMemoryManager * mmanager, char sign_block_size);							// konstruktor prijme odkaz na hlavny objekt mmanageru a uzivatel moze stanovit pozadovanu velkost blokov pamata (S-small,B-big)
	cList(cMemoryManager * mmanager, char sign_block_size, bool use_system_block);		// konstruktor prijme odkaz na hlavny objekt mmanageru, umozni nasatvit velkost system_block size
	~cList(void);

	void Init(cMemoryManager * mmanager);												// inicializacna metoda nahradzuje v pripade potreby konstruktor
	void Init(cMemoryManager * mmanager, char sign_block_size);							// inicializacna metoda nahradzuje v pripade potreby konstruktor
	void Init(cMemoryManager * mmanager, char sign_block_size, bool use_system_block);	// inicializacna metoda nahradzuje v pripade potreby konstruktor

	// Metody pre pracu so zoznamom
	void Add(const T &item);										// vlozenie prvku do zoznamu - vklada na koniec
	//void AddInPosition(const T &item, int position);				// vlozenie prvku na konkretnu poziciu
	T& GetHeadNodesData(void);										// vyber dat prveho uzlu
	T GetHeadNodesDataAndRemove(void);							    // vyber dat prveho uzlu a uzol odstrani, vyuzitie metody u realizacie fronty
	T& GetTailNodesData(void);										// vyber dat posledneho uzlu
	T& GetNodesData(int position);									// vyber dat uzlu na zadanej pozicii
	void EraseOnPosition(int position);								// odstranenie prvku konkretnej napozicii
	void EraseOnPosition(int position_from, int position_to);		// odstranenie prvkov na poziciacj od-do
	void Erase(T data);												// odstranenie vsetkych prvkov, ktore obsahuju zadane data
	void Clear(unsigned int count = 0);
	void ClearAll(void);
	bool IsEmpty();
	int GetSize();

	unsigned long int GetCountOfUsedMemBlocks(void);										// metoda vrati pocet pouzitych (vzhradenych) memory blokov
	char GetSignBlockSize(void);															// metoda vrati priznak, ci struktura pouziva SMALL alebo BIG bloky

	void ShowNodes(void);

private:
	cMemoryManager * mem_manager;
	cMemoryBlock * cMemoryBlocks_managment;

	void CountItemsPerBlock(void);

	unsigned int max_block_size;
	unsigned int items_per_block;

	unsigned int type_range;				// pocet bytov, ktrory zabera aktualny datovy typ
	unsigned int amb_actual_position;		// pozicia v poli pts_array
	unsigned int end_array_position;		// premenna nesie udaj o tom, kde sa nachadza posledna vlozena hodnota

	void AllocateNextMemory(void);

	void Erase(MF_Node<T> * pt_node);

	// Premenne a funkcie zoznamu
	MF_Node<T> * head;
	MF_Node<T> * tail;
	int size;
};
}}
