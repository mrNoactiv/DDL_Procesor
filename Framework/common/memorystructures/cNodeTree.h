/**
*	\file cNodeTree.h
*	\author R.Baca
*	\version 0.1
*	\date jan 2006
*	\brief In memory data structure implementing binary tree
*/
#ifndef __cNodeTree_h__
#define __cNodeTree_h__

#include "cTreeTuple.h"
#include "cTreeSpaceDescriptor.h"

// mohlo by se predalokovat
struct TupleChain {
	cTreeTuple item;
	struct TupleChain *next;
};

template <class TKEY>
class NODE
{
public:
	cTreeTuple *item;
	struct TupleChain *next;
	TKEY key;
	NODE<TKEY> *r;
	NODE<TKEY> *l;
	unsigned int N;

	void operator = (NODE<TKEY> *node);
};

template <class TKEY>
void NODE<TKEY>::operator = (NODE<TKEY> *node)
{
	cTreeTuple *pomitem;

	pomitem = item;
	item = node->item;
	node->item = pomitem;

	next = node->next;
	key = node->key;
	N = node->N;
}

//*****************************  cNodeTree ******************************

template <class TKEY>
class cNodeTree
{
	NODE<TKEY> *head;
	NODE<TKEY> *z;
	unsigned int sort_item;
	cTreeTuple **sorted;
	NODE<TKEY> *heap;
	NODE<TKEY> *backupheap;
	unsigned int node_count_max;	// maximalni pocet prvku na heap
	unsigned int node_count;		// pocet prvku na heap
	cTreeSpaceDescriptor mTreeSpaceDescriptor;

	NODE<TKEY> *newItem(cTreeTuple *item, TKEY key, NODE<TKEY> *l, NODE<TKEY> *r, unsigned int N);
	NODE<TKEY> *insertR(NODE<TKEY> *h, cTreeTuple *item, TKEY key);
	//	cTreeTuple *searchR(NODE<TKEY> * h, unsigned int pre);
	NODE<TKEY> *deleteR(NODE<TKEY> *h, unsigned int k);
	NODE<TKEY> *selectR(NODE<TKEY> *h, unsigned int k);
	NODE<TKEY> *insertT(NODE<TKEY> *h, cTreeTuple *item, TKEY key);
	NODE<TKEY> *rotR(NODE<TKEY> *h);
	NODE<TKEY> *rotL(NODE<TKEY> *h);
	NODE<TKEY> *partR(NODE<TKEY> *h, unsigned int k);
	NODE<TKEY> *balanceR(NODE<TKEY> *h);
	void sortR(NODE<TKEY> *h);
	void printR(NODE<TKEY> *h);
	void printSpatialR(NODE<TKEY> *h, int deep);
	void copyR(NODE<TKEY> *n, NODE<TKEY> *newheap);
	void copy(NODE<TKEY> *newheap);
public:

	cNodeTree(cTreeSpaceDescriptor *sd, unsigned int element_count); // predava se celkovy pocet uzlu v XML stromu
	inline unsigned int count() const;
	void insert(cTreeTuple *item, TKEY key);
	//	cTreeTuple *search(unsigned int pre);
	void deleteBranch(unsigned int k);
	TKEY selectNode(unsigned int k);
	void balance();
	cTreeTuple **sort(unsigned int *count);
	bool isFull();
	void clear();
	void Print();
	void PrintSpatial();
	~cNodeTree();
};

template <class TKEY>
cNodeTree<TKEY>::cNodeTree(cTreeSpaceDescriptor *sd, unsigned int element_count) 
{
	mTreeSpaceDescriptor = *sd;
	node_count_max = element_count;

	heap = new NODE<TKEY>[node_count_max];
	backupheap = new NODE<TKEY>[node_count_max];
	for (unsigned int i = 0; i < node_count_max; i++) {
		heap[i].item = new cTreeTuple(&mTreeSpaceDescriptor);
		backupheap[i].item = new cTreeTuple(&mTreeSpaceDescriptor);
	}

	node_count = 0;
	//z = newItem(NULL, 0, NULL, NULL, 0);

	z = new NODE<TKEY>();
	z->item = NULL;
	z->key = 0;
	z->l = NULL;
	z->r = NULL;
	z->N = 0;
	z->next = NULL;

	head = z;
}


template <class TKEY>
cNodeTree<TKEY>::~cNodeTree() 
{
	for (unsigned int i = 0; i < node_count_max; i++)
		delete heap[i].item;

	delete heap;
}

template <class TKEY>
bool cNodeTree<TKEY>::isFull()
{
	return count() == (node_count_max - 1);
}

template <class TKEY>
NODE<TKEY> *cNodeTree<TKEY>::newItem(cTreeTuple *item, TKEY key, NODE<TKEY> * l, NODE<TKEY> * r, unsigned int N)
{
	NODE<TKEY> *x;

	if (node_count >= node_count_max)
	{
		printf("cNodeTree overflow!\n");
		return NULL;
	} 

	x = &heap[node_count];
	node_count++;

	if (item != NULL)
	{
		*(x->item) = *item;
	}

	x->key = key;
	x->l = l;
	x->r = r;
	x->N = N;
	x->next = NULL;
	return x;
}

template <class TKEY>
inline unsigned int cNodeTree<TKEY>::count() const
{
	return head->N;
}

template <class TKEY>
void cNodeTree<TKEY>::insert(cTreeTuple *item, TKEY key)
{
	bool debug = false;

	if (node_count >= node_count_max)
	{
		NODE<TKEY> *switchheap;

		copy(backupheap);

		if (node_count >= node_count_max)
		{
			printf("insert::cNodeTree overflow!\n");
		}

		switchheap = heap;
		heap = backupheap;
		backupheap = switchheap;
	}

	head = insertR(head, item, key);
}

template <class TKEY>
NODE<TKEY> *cNodeTree<TKEY>::insertR(NODE<TKEY> *h, cTreeTuple *item, TKEY key)
{
	TupleChain *pom;

	if ( h == z) 
	{
		return newItem(item, key, z, z, 1);
	}

	TKEY t = h->key;

	if ( t == key )
	{
		pom = new struct TupleChain();
		pom->item.Resize(&mTreeSpaceDescriptor);
		pom->item = *item;
		pom->next = h->next;
		h->next = pom;
		h->N++;
		return h;
	}

	if ((unsigned int)rand() < RAND_MAX / (h->N + 1))
	{
		return insertT(h, item, key);
	}

	if ( key < t )
	{
		h->l = insertR( h->l, item, key);
	} else 
	{
		h->r = insertR( h->r, item, key);
	}

	h->N++;

	return h;
}

template <class TKEY>
NODE<TKEY> *cNodeTree<TKEY>::rotR(NODE<TKEY> * h) 
{
	NODE<TKEY> *x = h->l;
	int hN, xN;

	hN = h->N - h->l->N - h->r->N;
	xN = x->N - x->l->N - x->r->N;

	h->l = x->r;
	x->r = h;

	h->N = h->r->N + h->l->N + hN;
	x->N = x->r->N + x->l->N + xN;
	return x;
}

template <class TKEY>
NODE<TKEY> *cNodeTree<TKEY>::rotL(NODE<TKEY> * h) 
{
	NODE<TKEY> *x = h->r;
	int hN, xN;

	hN = h->N - h->l->N - h->r->N;
	xN = x->N - x->l->N - x->r->N;

	h->r = x->l;
	x->l = h;

	h->N = h->r->N + h->l->N + hN;
	x->N = x->r->N + x->l->N + xN;
	return x;
}

// vkladani do korene
template <class TKEY>
NODE<TKEY> *cNodeTree<TKEY>::insertT(NODE<TKEY> *h, cTreeTuple *item, TKEY key)
{
	TupleChain *pom;
	

	if ( h == z) 
	{
		return newItem(item, key, z, z, 1);
	}

	TKEY t = h->key;

	if ( t == key )
	{
		pom = new struct TupleChain();
		pom->item.Resize(&mTreeSpaceDescriptor);
		pom->item = *item;
		pom->next = h->next;
		h->next = pom;
		h->N++;
		return h;
	}

	if ( key < t ) 
	{
		h->l = insertT( h->l, item, key);
		h->N++;
		h = rotR(h);
	} else 
	{
		h->r = insertT( h->r, item, key);
		h->N++;
		h = rotL(h);
	}
	return h;
}

// leave in tree first k items
template <class TKEY>
NODE<TKEY> *cNodeTree<TKEY>::deleteR(NODE<TKEY> * h, unsigned int k)
{
	unsigned int left_tree_size, right_tree_size;
	int hN;

	if ( h == z) 
	{
		return h;
	}

	left_tree_size = ( h->l == z ) ? 0 : h->l->N;
	right_tree_size = ( h->r == z ) ? 0 : h->r->N;

	if ( left_tree_size == k )
	{
		return h->l;
	} 
	else if ( left_tree_size > k)
	{
		return deleteR( h->l, k);
	} 
	else if ( h->N - right_tree_size <= k) 
	{
		hN = h->N - h->l->N - h->r->N;
		h->r = deleteR( h->r, k - (h->N - right_tree_size));
		h->N = h->l->N + h->r->N + hN;
	} 
	else 
	{
		hN = h->N - h->l->N - h->r->N;
		h->r = z;
		h->N = h->l->N + h->r->N + hN;
	}

	return h;
}

template <class TKEY>
void cNodeTree<TKEY>::deleteBranch(unsigned int k)
{
	if (k < head->N)
	{
		head = deleteR(head, k);
	}


}

template <class TKEY>
NODE<TKEY> *cNodeTree<TKEY>::selectR(NODE<TKEY> *h, unsigned int k)
{
	unsigned int left_tree_size, right_tree_size;

	if ( h->l == z && h->r == z) 
	{
		return h;
	}

	left_tree_size = ( h->l == z ) ? 0 : h->l->N;
	right_tree_size = ( h->r == z ) ? 0 : h->r->N;

	if ( left_tree_size == k )
	{
		return h->l;
	} 
	else if ( left_tree_size > k)
	{
		return selectR( h->l, k);
	} 
	else if ( h->N - right_tree_size < k) 
	{
		return selectR( h->r, k - (h->N - right_tree_size));
	} else 
	{
		return h;
	}

}

template <class TKEY>
TKEY cNodeTree<TKEY>::selectNode(unsigned int k)
{
	return selectR(head, k)->key;
}

// deleni B stromu. Umisti k-ty nejmensi prvek ke koreni
template <class TKEY>
NODE<TKEY> *cNodeTree<TKEY>::partR(NODE<TKEY> *h, unsigned int k)
{
	unsigned int t = h->l->N;
	if(t > k)
	{
		h->l = partR(h->l, k);
		h = rotR(h);
	}
	if (t < k)
	{
		h->r = partR(h->r, k - t -1);
		h = rotL(h);
	}
	return h;
}

template <class TKEY>
void cNodeTree<TKEY>::sortR(NODE<TKEY> *h)
{
	TupleChain *next;

	if (h == z) return;
	sortR(h->l);
	sorted[sort_item] = h->item;
	sort_item++;
	next = h->next;
	while ( next != NULL)
	{
		sorted[sort_item] = &next->item;
		sort_item++;
		next = next->next;
	}
	sortR(h->r);
}


template <class TKEY>
cTreeTuple **cNodeTree<TKEY>::sort(unsigned int *c)
{
	sort_item = 0;
	sorted = new cTreeTuple*[count()];
	sortR(head);
	*c = sort_item;

	return sorted;
}

// vyvazeni B-stromu (casove narocne)
template <class TKEY>
NODE<TKEY> *cNodeTree<TKEY>::balanceR(NODE<TKEY> *h)
{
	if (h->N < 2)
		return h;
	h = partR(h, h->N / 2);
	h->l = balanceR(h->l);
	h->r = balanceR(h->r);
	return h;
}

template <class TKEY>
void cNodeTree<TKEY>::balance()
{
	head = balanceR(head);
}

template <class TKEY>
void cNodeTree<TKEY>::copyR(NODE<TKEY> *h, NODE<TKEY> *newheap)
{
	//TupleChain *next;
	unsigned int current = node_count;

	//if (h == z) return;
	
	newheap[current] = h;
	
	node_count++;
	if (h->l == z)
	{
		newheap[current].l = z;
	} else
	{
		newheap[current].l = &newheap[node_count];
		copyR(h->l, newheap);
	}

	if (h->r == z)
	{
		newheap[current].r = z;
	} else
	{
		newheap[current].r = &newheap[node_count];
		copyR(h->r, newheap);
	}
}

template <class TKEY>
void cNodeTree<TKEY>::copy(NODE<TKEY> *newheap)
{
	node_count = 0;
	if (head != z)
	{
		copyR(head, newheap);
		head = newheap;
	} else
	{
		head = z;
	}
}


template <class TKEY>
void cNodeTree<TKEY>::clear()
{
	head = z;
	node_count = 0;
}

template <class TKEY>
void cNodeTree<TKEY>::printR(NODE<TKEY> *h)
{
	TupleChain *next;

	if (h == z) return;
	printR(h->l);
	printf("%d : ",h->key);
	h->item->Print("\n");
	next = h->next;
	while ( next != NULL)
	{			
		next->item.Print("\n");
		next = next->next;
	}
	printR(h->r);
}

template <class TKEY>
void cNodeTree<TKEY>::Print()
{
	printR(head);
}

template <class TKEY>
void cNodeTree<TKEY>::printSpatialR(NODE<TKEY> *h, int deep)
{
	//TupleChain *next;

	if (h == z) return;
	printSpatialR(h->l, deep + 2);

	for (unsigned int i = 0; i < deep; i++)
	{
		printf(" ");
	}
	printf("%d\n", h->N);
	//h->item->Print(mode, "\n");
	//next = h->	next;
	//while ( next != NULL)
	//{
	//	next->item.Print(mode, "\n");
	//	next = next->next;
	//}
	printSpatialR(h->r, deep + 2);
}

template <class TKEY>
void cNodeTree<TKEY>::PrintSpatial()
{
	printf("\nTree occupation:\n");
	printSpatialR(head, 0);
}

#endif
