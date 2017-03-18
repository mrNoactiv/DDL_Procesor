#include "cMapSearchInput.h"

/// Constructor
cMapSearchInput::cMapSearchInput()
	:mRight(NULL), 
	mLeftToRightMapping(NULL)
{
	Init();
}

/// Destructor
cMapSearchInput::~cMapSearchInput()
{
	Delete();
}

void cMapSearchInput::Delete()
{
	if (mRight != NULL)
	{
		delete mRight;
		mRight = NULL;
	}
	if (mLeftToRightMapping != NULL)
	{
		delete mLeftToRightMapping;
		mLeftToRightMapping = NULL;
	}
}

void cMapSearchInput::Init()
{
	Delete();

	mRight = new cArray<unsigned short>();
	mRight->Resize(10);
	mLeftToRightMapping = new tLeftMappingType(true);	
	mLeftToRightMapping->Resize(10);

	mKeySizeInfo = new cSizeInfo<unsigned int>();
	mLeafSizeInfo = new cSizeInfo<cMapSearchPair*>();

}


/**
* Add new edge into potential mapping
* \param value Value of the edge
* \param head Mapping item. The order of the first sibling is 0.
*/
bool cMapSearchInput::AddEdge(unsigned int value, cMapSearchPair* head)
{
	if (head->GetLeft() != EMPTY_VALUE)
	{
		if (head->GetLeft() >= mLeftToRightMapping->Count())
		{
			mLeftNodeCount++;
			assert(head->GetLeft() == mLeftToRightMapping->Count());
			if (mLeftToRightMapping->Count() == mLeftToRightMapping->Size())
			{
				mLeftToRightMapping->AddDouble(NULL);
			} else
			{
				mLeftToRightMapping->SetCount(mLeftToRightMapping->Count() + 1);
			}
			if (*mLeftToRightMapping->GetLastItem() == NULL)
			{
				tSortedArray* sortedArray = new tSortedArray(mKeySizeInfo, mLeafSizeInfo, true, 10);
				*mLeftToRightMapping->GetLastItem() = sortedArray;
			} else
			{
				(*mLeftToRightMapping->GetLastItem())->Clear();
			}
		}
		if (!head->GetIsRightOptional())
		{
			while (head->GetRight() >= mRight->Count())
			{			
				mRight->AddDouble(EMPTY_VALUE);
			}

			if (mRight->GetRefItem(head->GetRight()) != EMPTY_VALUE)
			{
				return false;
			}
			if (value == 0 && !head->GetIsRightSubtreeOptional())
			{
				*mRight->GetItem(head->GetRight()) = head->GetLeft();
				(*mLeftToRightMapping->GetItem(head->GetLeft()))->Clear();
				(*mLeftToRightMapping->GetItem(head->GetLeft()))->Insert(value, head);
				mRightOccupiedCount++;
				return true;
			}
		}
		(*mLeftToRightMapping->GetItem(head->GetLeft()))->Insert(value, head);
	} else
	{
		if (mLeftToRightMapping->Count() == 0 || (*mLeftToRightMapping->GetLastItem())->GetItemCount() == 0 ||
			head->GetLeft() != (*mLeftToRightMapping->GetLastItem())->GetRefLeaf(0)->GetLeft())
		{
			if (mLeftToRightMapping->Count() == mLeftToRightMapping->Size())
			{
				mLeftToRightMapping->AddDouble(NULL);
			} else
			{
				mLeftToRightMapping->SetCount(mLeftToRightMapping->Count() + 1);
			}
			if (*mLeftToRightMapping->GetLastItem() == NULL)
			{
				tSortedArray* sortedArray = new tSortedArray(mKeySizeInfo, mLeafSizeInfo, true, 10);
				*mLeftToRightMapping->GetLastItem() = sortedArray;
			} else
			{
				(*mLeftToRightMapping->GetLastItem())->Clear();
			}
		}
		while (head->GetRight() >= mRight->Count())
		{			
			mRight->AddDouble(EMPTY_VALUE);
		}
		(*mLeftToRightMapping->GetLastItem())->Insert(value, head);
	}
	return true;
}

/**
* Print whole content of this object.
*/
void cMapSearchInput::Print()
{
	printf("Number of coved right siblings: %d\n", mRightOccupiedCount);
	printf("Number of left siblings: %d\n", GetLeftNodeCount());
	printf("mRight array: ");
	for (unsigned int i = 0; i < mRight->Count() - 1; i++)
	{
		printf("%d, ", mRight->GetRefItem(i));
	}
	printf("%d\n", mRight->GetRefItem(mRight->Count() - 1));
	for (unsigned int i = 0; i < mLeftToRightMapping->Count(); i++)
	{
		printf("From %d\n", i);
		for (unsigned int j = 0; j < (*mLeftToRightMapping->GetItem(i))->GetItemCount(); j++)
		{
			printf("\tValue: %d, ", *(*mLeftToRightMapping->GetItem(i))->GetSortedItem(j));
			(*mLeftToRightMapping->GetItem(i))->GetRefSortedLeaf(j)->Print("\n");
		}
	}
}