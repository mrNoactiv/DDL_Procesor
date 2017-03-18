/**
*	\file cLinkedList.h
*	\author Peter Chovanec
*	\version 0.1
*	\date feb 2013
*	\brief Main memory structure simulating linked list.
*/

#include <mutex>
#include "common/thread/spinlock_mutex.h"

/*
	POUZITIE
	char* listBuffer = GetBuffer(size of buffer); // pocet itemov * velkost intu + dvoch pointerov
	cLinkedList<typ>* list = (cLinkedList<typ>*)GetBuffer(sizeof(cLinkedList<typ>));
	list->Init(listBuffer);


*/
#ifndef __cLinkedList_h__
#define __cLinkedList_h__

namespace common {
	namespace memorystructures {

template<class T>
class cLinkedListNode
{
public:
	cLinkedListNode<T> *Previous;
	cLinkedListNode<T> *Next;
	T Item;
};

template<class T>
class cLinkedList
{
private:
	unsigned int mItemCount;              // current number of items in the list
	unsigned int mItemCapacity;           // the maximal number of items in the list
	cLinkedListNode<T>* mHead;            // head of the linked list
	cLinkedListNode<T>* mTail;            // tail of the linked list, check if it works for all methods
	cLinkedListNode<T>* mArray;		       // the linked list

	std::mutex mGetDeleteMutex;
	// spinlock_mutex mGetDeleteMutex;
	std::mutex mAddMutex;
	// spinlock_mutex mAddMutex;

	static int Equal(const T a, const T b);

	static int COUNT;

	//static int intCompare(const void * a, const void * b);
public:
	static const unsigned int EMPTY_POINTER = 0xffffffff;

	cLinkedList();
	cLinkedList(unsigned int capacity);
	~cLinkedList();

	// ask Peter: where is the capacity?
	void Init(char* linkedList);
	void SetList(char* linkedList);
	void CopyList(char* linkedList, unsigned int byteSize);
	void Clear();

	inline unsigned int GetItemCount() const;
	inline unsigned int GetItemCapacity() const;	

	inline void AddItem(T& item);
	inline void DeleteItem(unsigned int order);
	inline void DeleteItem(T* data);
	inline cLinkedListNode<T>* GetDeleteHeadNode();
	inline void DeleteNode(cLinkedListNode<T>* node);
	inline void AddNode(cLinkedListNode<T>* pNode);

	inline T* GetItem(unsigned int order);
	inline T& GetRefItem(unsigned int order);
	inline T& GetRefLastItem() const				{ return mTail->Item; }
	inline T* GetLastItem()							{ return GetItem(mItemCount - 1); }
	
	// BUFFER SIZE
	inline int GetListBufferSize()						{ return sizeof(cLinkedList<T>); }
	inline int GetItemsBufferSize()						{ return mItemCapacity * + (sizeof(cLinkedListNode<T>)); }

	inline cLinkedListNode<T>* GetRefItem(cLinkedListNode<T>** nodeFrom, unsigned int &orderFrom, unsigned int order);

	inline void Sort(int order, int (*compare)(const T a, const T b));
	inline void Sort(int (*compare)(const T a, const T b));
	//inline void PhysicalSort();
	void QuickSortList(cLinkedListNode<T> *pLeft, cLinkedListNode<T> *pRight, int(*compare)(const T a, const T b));

	unsigned int Check() const;
	void Print() const;
};

template<class T> int cLinkedList<T>::COUNT = 0;

template<class T>
cLinkedList<T>::cLinkedList(): mItemCount(0), mItemCapacity(0), mHead(NULL), mTail(NULL), mArray(NULL)
{
}

template<class T>
cLinkedList<T>::cLinkedList(unsigned int capacity) : mItemCount(0), mHead(NULL), mTail(NULL), mArray(NULL)
{
	mArray = new cLinkedListNode<T>[capacity];
	mItemCapacity = capacity;
}

template<class T>
cLinkedList<T>::~cLinkedList()
{
	if (mArray !=NULL)
	{
		delete mArray;
		mArray = NULL;
	}
}

template<class T>
void cLinkedList<T>::SetList(char* linkedList)
{
	mArray = (cLinkedListNode<T>*)linkedList;	
}

template<class T>
void cLinkedList<T>::CopyList(char* linkedList, unsigned int byteSize)
{
	memcpy(mArray, linkedList, byteSize);
}


template<class T>
inline void cLinkedList<T>::Init(char* linkedList)
{
	mItemCount = 0;
	mHead = NULL;
	SetList(linkedList);
}

/*
 * bas064. Clear the list, head is set to the first item of the array.
 */
template<class T>
inline void cLinkedList<T>::Clear()
{
	mHead = &mArray[0];
	mHead->Previous = NULL;
	mHead->Next = NULL;
	mTail = mHead;
	mItemCount = 0;
}

template<class T>
inline unsigned int cLinkedList<T>::GetItemCount() const
{
	return mItemCount;
}

template<class T>
inline unsigned int cLinkedList<T>::GetItemCapacity() const
{
	return mItemCapacity;
}

/*
 * It delete the first node, it means the head is set to the next node.
 * previous and next pointers of the node get are unrelevant
 * Is is expected to call AddNode setting the node as the last node.
 */
template<class T>
cLinkedListNode<T>* cLinkedList<T>::GetDeleteHeadNode()
{
	cLinkedListNode<T>* oldHeadNode;

	mGetDeleteMutex.lock();

	if (mHead == mTail)
	{
		throw("cLinkedList<T>::DeleteNode(): mHead = mTail.");
	}

	oldHeadNode = mHead;         // get the current head node
	mHead = oldHeadNode->Next;   // set the new head node
	mHead->Previous = NULL;      // new head has no previous node
	mItemCount--;

	mGetDeleteMutex.unlock();

	// the node has been deleted from the queue: both previous and next pointers are NULL
	oldHeadNode->Previous = NULL;
	oldHeadNode->Next = NULL;

	return oldHeadNode;
}

/*
 * Similar to GetDeleteHeadeNode but for a general node.
 * Performance Issue: It is necessary to put mutex into each node.
 */
template<class T>
void cLinkedList<T>::DeleteNode(cLinkedListNode<T>* node)
{
	COUNT++;
	//printf("COUNT: %d  \r", COUNT);

	if (COUNT == 69550)
	{
		int breakpoint = 0;
	}

	mGetDeleteMutex.lock();

	if (node->Next != NULL)
	{
		if (node->Previous != NULL)
		{
			// the node is in the middle of the queue, remove the node from the queue
			node->Previous->Next = node->Next;
			node->Next->Previous = node->Previous;
			node->Previous = NULL;
			node->Next = NULL;
		}
		else
		{
			// the node is the head node
			mHead = node->Next;   // set the new head node
			mHead->Previous = NULL;      // new head has no previous node
			node->Next = NULL;
		}
		mItemCount--;
	}
	else if (node->Previous != NULL)
	{
		// it is the last node of the queue
		mTail = node->Previous;
		node->Previous->Next = NULL;
		mItemCount--;
		node->Previous = NULL;
	}
	// the last configuration means the the node is not in the queue

	mGetDeleteMutex.unlock();
}

/*
 * The methods can be invocated after GetDeleteHeadNode. If the node is not in the queue (previous and next
 * pointers are NULL), the node is added in the tail. If the node in the queue, the node is moved in the tail
 * Performance bottleneck: 
 *   - It is necessary tu put mutexes into each node, especially for the move operation.
 *   - The node is moved although it is in the midlle of the queue - it is inapropriate, it is necessary
 *       only if it is near to the head, but how is it possible to detect?
 */
template<class T>
void cLinkedList<T>::AddNode(cLinkedListNode<T>* node)
{
	bool nodeIsTail = false;
	cLinkedListNode<T>* oldTailNode;

	mAddMutex.lock();

	node->Next = NULL;
	oldTailNode = mTail;   // get the current tail node
	mTail = node;          // set the new tail node
	oldTailNode->Next = mTail;     // the new tail node is the new next node of the old tail node
	mTail->Previous = oldTailNode; // the old tail node is the previous node of the new tail node

	if (mItemCount == 1)  // mHead == mTail
	{
		mHead = mTail->Previous;
		mHead->Next = mTail;
		mHead->Previous = NULL;
	}

	mItemCount++;

	mAddMutex.unlock();
}

/**
 * Add the item into the list as the new tail.
 */
template<class T>
inline void cLinkedList<T>::AddItem(T& item)
{
	cLinkedListNode<T> /**front,*/ *newTail;

	newTail = mArray + mItemCount;// * sizeof(cLinkedListNode<T>));  mk: ???
	newTail->Next = NULL;
	newTail->Item = item;

	if (mItemCount == 0)
	{
		newTail->Previous = NULL;
		mHead = newTail;
	}
    else
    {
		cLinkedListNode<T> *oldTail = mTail;
		mTail->Next = newTail;
		newTail->Previous = oldTail;
    }

	mTail = newTail;
	mItemCount++;
}

template<class T>
inline void cLinkedList<T>::DeleteItem(T* data)
{
	cLinkedListNode<T> *front = mHead;
	unsigned int order = 0;

	for (unsigned int i = 0; i < mItemCount; i++)
	{
		if (GetItem(i) == data)
		{
			break;
		}
		
		order++;
		front = front->Next;
	}

	if ((order > 0) && (order < mItemCount - 1))
	{
		front->Previous->Next = front->Next;
		front->Next->Previous = front->Previous;
	}
	else if (order > 0)
	{
		front->Previous->Next = front->Next;
	}
	else
	{
		front->Next->Previous = front->Previous;
		mHead = front->Next;
	}

	mItemCount--;
}

template<class T>
inline void cLinkedList<T>::DeleteItem(unsigned int order)
{
	assert(mItemCount > 0 && order < mItemCount);

	cLinkedListNode<T> *front = mHead;

	for (unsigned int i = 0; i < order; i++)
	{
		front = front->Next;
	}

	if ((order > 0) && (order < mItemCount - 1))
	{
		front->Previous->Next = front->Next;
		front->Next->Previous = front->Previous;
	}
	else if (order > 0)
	{
		front->Previous->Next = front->Next;
	}
	else if (mItemCount > 1) // PCH - Added 5.2.2014 for avoiding the delete only one item in the list
	{
		front->Next->Previous = front->Previous;
		mHead = front->Next;
	}

	mItemCount--;
}


template<class T>
inline T* cLinkedList<T>::GetItem(unsigned int order)
{
	assert(order < mItemCount);

	cLinkedListNode<T> *front = mHead;

	for (unsigned int i = 0; i < order; i++)
      front = front->Next;

	return &(front->Item);
}

template<class T>
inline T& cLinkedList<T>::GetRefItem(unsigned int order)
{
	assert(order < mItemCount);

	cLinkedListNode<T> *front = mHead;

	for (unsigned int i = 0; i < order; i++)
	{
		front = front->Next;
	}

	return front->Item;
}

/*
 * bas064. Scan the list - find a node with the order from a node and order.
 */
template<class T>
inline cLinkedListNode<T>* cLinkedList<T>::GetRefItem(cLinkedListNode<T>** nodeFrom, unsigned int &orderFrom, unsigned int order)
{
	assert(order < mItemCount && orderFrom <= order);

	cLinkedListNode<T> *front;

	if (*nodeFrom == NULL)
	{
		front = mHead;
		orderFrom = 0;
	}
	else
	{
		front = *nodeFrom;
	}

	for (unsigned int i = orderFrom; i < order; i++)
	{
		front = front->Next;
	}

	*nodeFrom = front;
	orderFrom = order;

	return *nodeFrom;
}


template<class T>
void cLinkedList<T>::Sort(int order, int (*compare)(const T a, const T b))
{
	cLinkedListNode<T> *tail = mHead;
	cLinkedListNode<T> *head = mHead;

	for (unsigned int i = 0; i < mItemCount - 1; i++)
	{
		if (order == i)
			head = tail;

		tail = tail->Next;
	}

	QuickSortList(head, tail, compare);
}


template<class T>
void cLinkedList<T>::Sort(int (*compare)(const T a, const T b))
{
	cLinkedListNode<T> *tail = mHead;

	for (unsigned int i = 0; i < mItemCount - 1; i++)
      tail = tail->Next;

	QuickSortList(mHead, tail, compare);
}

template<class T>
void cLinkedList<T>::QuickSortList(cLinkedListNode<T> *pLeft, cLinkedListNode<T> *pRight, int (*compare)(const T a, const T b))
{
	cLinkedListNode<T> *pStart;
	cLinkedListNode<T> *pCurrent; 
	T tmpData;

	// If the left and right pointers are the same, then return
	if (pLeft == pRight) return;

	// Set the Start and the Current item pointers
	pStart = pLeft;
	pCurrent = pStart->Next;

	// Loop forever (well until we get to the right)
	while (1)
	{
		// If the start item is less then the right
		if (compare(pStart->Item, pCurrent->Item) == -1)
		//if (pStart->data < pCurrent->data)
		{
			// Swap the items
			tmpData = pCurrent->Item;
			pCurrent->Item = pStart->Item;
			pStart->Item = tmpData;
		}	
		
		// Check if we have reached the right end
		if (pCurrent == pRight) break;

		// Move to the next item in the list
		pCurrent = pCurrent->Next;
	}

	// Swap the First and Current items
	tmpData = pLeft->Item;
	pLeft->Item = pCurrent->Item;
	pCurrent->Item = tmpData;

	// Save this Current item
	cLinkedListNode<T> *pOldCurrent = pCurrent;

	// Check if we need to sort the left hand size of the Current point
	pCurrent = pCurrent->Previous;
	if (pCurrent != NULL)
	{
		if ((pLeft->Previous != pCurrent) && (pCurrent->Next != pLeft))
			QuickSortList(pLeft, pCurrent, compare);
	}

	// Check if we need to sort the right hand size of the Current point
	pCurrent = pOldCurrent;
	pCurrent = pCurrent->Next;
	if (pCurrent != NULL)
	{
		if ((pCurrent->Previous != pRight) && (pRight->Next != pCurrent))
			QuickSortList(pCurrent, pRight, compare);
	}
}

template<class T>
unsigned int cLinkedList<T>::Check() const
{
	uint count1 = 0, count2 = 0;
	cLinkedListNode<T> *node = mHead;

	while (node != NULL)
	{
		if (node->Item > mItemCapacity)
		{
			int err = 1;
		}
		
		node = node->Next;
		count1++;

		if (count1 > mItemCount)
		{
			break;
		}
	}

	node = mTail;
	while (node != NULL)
	{
		if (node->Item > mItemCapacity)
		{
			int err = 1;
		}
		
		node = node->Previous;
		count2++;

		if (count2 > mItemCount)
		{
			break;
		}
	}

	if (count1 != count2 || mItemCount != count1)
	{
		printf("Error: cLinkedList<T>::Check(): count1: %u, count2: %d, mItemCount\n", count1, count2, mItemCount);
		Print();
	}

	return count1;
}


template<class T>
void cLinkedList<T>::Print() const
{
	uint count = 0;
	cLinkedListNode<T> *node = mHead;

	printf("\n");

	while (node != NULL)
	{
		printf("%u, ", node->Item);
		node = node->Next;
		count++;
	}
	printf("\n#Nodes: %u\n", count);

	count = 0;
	node = mTail;
	while (node != NULL)
	{
		printf("%u, ", node->Item);
		node = node->Previous;
		count++;
	}
	printf("\n#Nodes: %u\n", count);
}
}}
#endif