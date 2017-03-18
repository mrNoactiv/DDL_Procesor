#include "cMapSearchAll.h"

cMapSearchAll::cMapSearchAll()
	:mStack(NULL)
{
	Init();
}

cMapSearchAll::~cMapSearchAll()
{
	Delete();
}

void cMapSearchAll::Delete()
{
	if (mStack == NULL)
	{
		delete mStack;
		mStack = NULL;
	}
	if (mResult == NULL)
	{
		delete mResult;
		mResult = NULL;
	}
}

void cMapSearchAll::Init()
{
	Delete();

	mStack = new cStack<unsigned int>(50);
	mResult = new cArray<unsigned int>();
	mResult->Resize(50);
}

/**
* Sub-grap searching
* \param root Parameter by reference. Root pair of the new mapping tree is returned in this variable.
* \param maxChangeValue Maximal change value acceptable.
* \param input Input graph.
* \return 
*	- Lowest change value founded if the lowest value if lower than maxChangeValue.
*	- cMapSearchInput::EMPTY_VALUE otherwise.
*/
unsigned short cMapSearchAll::FindMapping(
		cMapSearchPair* root, 
		unsigned short maxChangeValue,
		cMapSearchInput* input)
{
	mMinimalValue = (unsigned short)-1;
	mResult->ClearCount();
	FindMappingR(0, 0, maxChangeValue, input);

	//if (mMinimalValue != (unsigned short)-1)
	//{
	//	printf("Minimal mapping: %d\n", mMinimalValue);
	//	for (unsigned int i = 0; i < mResult->Count(); i++)
	//	{
	//		if (i == input->GetLeftNodeCount())
	//		{
	//			printf("\nEmpty mapping:");
	//		}
	//		printf("%d, ", mResult->GetRefItem(i));
	//	}
	//} else
	//{
	//	printf("minimal mapping not found\n");
	//}
	
	if (mMinimalValue != (unsigned short)-1)
	{
		//root->SetChildCount(root->GetChildCount() + mResult->Count());
		unsigned int rightCount = input->GetRightArray()->Count();

		cMapSearchPair* concatenation = root;
		unsigned int node = 0;
		unsigned int changeHasNextIndex = 0;
		unsigned int lastAmongSiblingsLeft = 0;
		unsigned int lastAmongSiblingsRight = 0;
		unsigned int lastPairLeft = 0;
		unsigned int lastPairRight = 0;
		bool changeHasNext = false;

		for (unsigned int i = 0; i < mResult->Count(); i++)
		{
			lastPairLeft = node;
			lastPairRight = mResult->GetRefItem(i);
			if (input->GetEdge(node, mResult->GetRefItem(i))->GetIsRightOptional())
			{
				input->GetEdge(node, mResult->GetRefItem(i))->SetRight(rightCount++);
				changeHasNext = true;
				lastAmongSiblingsLeft = node;
				lastAmongSiblingsRight = mResult->GetRefItem(i);
			} else
			{
				if (input->GetEdge(node, mResult->GetRefItem(i))->GetRight() == input->GetRightArray()->Count() - 1)
				{
					changeHasNextIndex = i;
				}
			}
			concatenation->GetRefLastChild()->SetNext(input->GetEdge(node, mResult->GetRefItem(i)));
			concatenation = input->GetEdge(node, mResult->GetRefItem(i));
			if (node < input->GetLeftNodeCount())
			{
				node++;
			}
		}
		if (changeHasNext)
		{
			input->GetEdge(changeHasNextIndex, mResult->GetRefItem(changeHasNextIndex))->SetChangeToHasNext(true);
			input->GetEdge(lastAmongSiblingsLeft, lastAmongSiblingsRight)->SetLastAmongSiblings(true);
		}
		input->GetEdge(lastPairLeft, lastPairRight)->SetIsLastInMapping(true);
		root->SetLastChild(concatenation->GetRefLastChild());
		return mMinimalValue;
	} else
	{
		return cMapSearchInput::EMPTY_VALUE;
	}
}

/**
* Searching of a minimal value for one left node.
* \param leftNodeOrder Order of the node.
* \param change Minimal change founded so far.
* \param maxChangeValue Maximal change value acceptable.
* \param input Input graph.
*/
void cMapSearchAll::FindMappingR(
		unsigned int leftNodeOrder,
		unsigned short change, 
		unsigned short maxChangeValue, 
		cMapSearchInput* input)
{
	unsigned int edgeOrder = -1;
	bool last = input->GetLeftNodeCount() == (leftNodeOrder + 1);
	bool doNotReset = false;

	while(++edgeOrder < input->GetEdgeCount(leftNodeOrder))
	{
		unsigned int right = input->GetEdge(leftNodeOrder, edgeOrder)->GetRight();
		bool isRightOptional = input->GetEdge(leftNodeOrder, edgeOrder)->GetIsRightOptional();
		if (!isRightOptional)
		{
			if (input->GetRightArray()->GetRefItem(right) == cMapSearchInput::EMPTY_VALUE)
				//||	input->GetEdge(leftNodeOrder, edgeOrder)->GetIsRightSubtreeOptional())
			{
				//if (!input->GetEdge(leftNodeOrder, edgeOrder)->GetIsRightSubtreeOptional())
				//{
					*input->GetRightArray()->GetItem(right) = leftNodeOrder;
				//} else
				//{
				//	(*input->GetRightArray()->GetItem(right))++;
				//}
			} else
			{
				if (input->GetRightArray()->GetRefItem(right) != leftNodeOrder)
				{
					continue;
				} else
				{
					doNotReset = true;
				}
			}
		}

		mStack->Push(edgeOrder);
		unsigned int newChange = change + input->GetEdgeValue(leftNodeOrder, edgeOrder);

		if (newChange > maxChangeValue)
		{
			mStack->Pop();
			if (!isRightOptional && !doNotReset)
			{
				//if (!input->GetEdge(leftNodeOrder, edgeOrder)->GetIsRightSubtreeOptional())
				//{
					assert(input->GetRightArray()->GetRefItem(right) == leftNodeOrder);
					*input->GetRightArray()->GetItem(right) = cMapSearchInput::EMPTY_VALUE;
				//} else
				//{
				//	(*input->GetRightArray()->GetItem(right))--;
				//}
			}
			break;
		}
		if (last)
		{
			ResolveRest(newChange, maxChangeValue, input);			
		} else
		{
			FindMappingR(leftNodeOrder + 1, newChange, maxChangeValue, input);
		}
		mStack->Pop();

		if (!isRightOptional && !doNotReset)
		{
			//if (!input->GetEdge(leftNodeOrder, edgeOrder)->GetIsRightSubtreeOptional())
			//{
				assert(input->GetRightArray()->GetRefItem(right) == leftNodeOrder);
				*input->GetRightArray()->GetItem(right) = cMapSearchInput::EMPTY_VALUE;
			//} else
			//{
			//	(*input->GetRightArray()->GetItem(right))--;
			//}
		}
	}
}

/**
* Check if this configuration is minimal. If yes, the edge order is stored.
* \param change Minimal change founded so far.
* \param maxChangeValue Maximal change value acceptable.
* \param input Input graph.
*/
void cMapSearchAll::ResolveRest(
		unsigned short change, 
		unsigned short maxChangeValue, 
		cMapSearchInput* input)
{
	unsigned int pushCounter = 0;
	unsigned int leftNodeCount = input->GetLeftNodeCount();

	bool debug = false;
	if (debug)
	{
		input->Print();
	}

	if (input->GetLeftMappingNodesCount() > leftNodeCount)
	{
		for (unsigned int i = 0; i < input->GetEdgeCount(leftNodeCount); i++)
		{
			if (input->GetRightArray()->GetRefItem(input->GetEdge(leftNodeCount, i)->GetRight()) == cMapSearchInput::EMPTY_VALUE)
			{
				pushCounter++;
				mStack->Push(i);
				change += (unsigned short)input->GetEdgeValue(leftNodeCount, i);
			}
		}
	}

	if (change < mMinimalValue && change <= maxChangeValue)
	{
		mResult->ClearCount();
		mMinimalValue = change;
		for (unsigned int i = 0; i < mStack->Count(); i++)
		{
			mResult->AddDouble(mStack->GetItem(i));
		}
	}

	for (unsigned int i = 0; i < pushCounter; i++)
	{
		mStack->Pop();
	}
}