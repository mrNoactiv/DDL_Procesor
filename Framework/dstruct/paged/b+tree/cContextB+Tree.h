/**
*	\file cContextB+Tree.h
*	\author Petr Lukas
*	\version 0.2
*	\date 2014
*	\brief Implement persistent B-tree
*/

#ifndef __cContextBpTree_h__
#define __cContextBpTree_h__

#include "dstruct/paged/b+tree/cB+TreeConst.h"
#include "dstruct/paged/b+tree/cCommonB+Tree.h"
#include "dstruct/paged/core/cDataStructureContext.h"
#include "dstruct/paged/core/cTreeNode.h"
#include "dstruct/paged/core/cTreeNode_SubNode.h"

/**
* Implements persistent B-tree with fixed size of key and leaf data.
* Parameters of the template:
*		- TKey - Type of the key value.
*
*	\author Petr Lukas
*	\version 0.2
*	\date nov 2014
**/
namespace dstruct {
	namespace paged {
		namespace bptree {

			/**
			* Context to iterate through a result of a range query.
			**/
			template <class TKey>
			class cBpTreeContext : public cDataStructureContext
			{
			private:
				cNodeBuffers<TKey>* mNodeBuffers;
				cTreeNode<TKey>* mCurrentNode;
				int mCurrentOrder;
				TKey* mKeyH;
				const char* mItem;
				bool mFirstPass;
				bool mReadFlag;

				void Null();
				void Init();
				void Delete();
			public:
				cBpTreeContext();
				~cBpTreeContext();

				inline cNodeBuffers<TKey>* GetNodeBuffers() { return mNodeBuffers; }

				inline cTreeNode<TKey>* GetCurrentNode() { return mCurrentNode; }
				inline void SetCurrentNode(cTreeNode<TKey>* node) { mCurrentNode = node; }

				inline int GetCurrentOrder() { return mCurrentOrder; }
				inline void SetCurrentOrder(const int order) { mCurrentOrder = order; }
				inline int IncrementCurrentOrder() { return ++mCurrentOrder; }

				inline TKey* GetKeyH() { return mKeyH; }
				inline void SetKeyH(TKey* key) { mKeyH = key; }

				inline const char* GetItem() { return mItem; }
				inline void SetItem(const char* item) { mItem = item; }

				inline bool GetFirstPass() { return mFirstPass; }
				inline void SetFirstPass(const bool value) { mFirstPass = value; }

				inline bool GetReadFlag() { return mReadFlag; }
				inline void SetReadFlag(bool readFlag) { mReadFlag = readFlag; }
			};

			/**
			* An implementation of B+tree where the range query does not materialize the result
			**/
			template <class TKey>
			class cContextBpTree : public cCommonBpTree<cTreeNode<TKey>, cTreeNode<TKey>, TKey>
			{
			private:
				void ShiftKeys(unsigned int nodeId, char* tempKey, void(*callback)(char*, char*), char* data);

			public:
				cContextBpTree();
				~cContextBpTree();

				// Returns true if the starting key has been found, so the context is prepared.
				bool RangeQuery(cBpTreeContext<TKey>* context, TKey* il, TKey* ih, bool readFlag = true);

				// Returns true if the context has been successfully advanced. Returns false when there are no more results. Use with "while" iteration.
				bool Advance(cBpTreeContext<TKey>* context);

				// Do not forget to close the context by this method.
				void CloseContext(cBpTreeContext<TKey>* context);
			};

			template <class TKey>
			cBpTreeContext<TKey>::cBpTreeContext()
			{
				Null();
				Init();
			}

			template <class TKey>
			cBpTreeContext<TKey>::~cBpTreeContext()
			{
				Delete();
			}
			
			template <class TKey>
			void cBpTreeContext<TKey>::Null()
			{
				mNodeBuffers = NULL;
			}

			template <class TKey>
			void cBpTreeContext<TKey>::Init()
			{
				mNodeBuffers = new cNodeBuffers<TKey>();
			}

			template <class TKey>
			void cBpTreeContext<TKey>::Delete()
			{
				if (mNodeBuffers != NULL)
				{
					delete mNodeBuffers;
					mNodeBuffers = NULL;
				}
			}

			template <class TKey>
			cContextBpTree<TKey>::cContextBpTree()
				: cCommonBpTree<cTreeNode<TKey>, cTreeNode<TKey>, TKey>()
			{
			}

			template <class TKey>
			cContextBpTree<TKey>::~cContextBpTree()
			{
			}

			template <class TKey>
			bool cContextBpTree<TKey>::RangeQuery(cBpTreeContext<TKey>* context, TKey* il, TKey* ih, bool readFlag)
			{
				context->SetReadFlag(readFlag);

				tNodeIndex nodeIndex = parent::mHeader->GetRootIndex();
				cTreeNode<TKey> *currentNode = NULL;
				bool leaf;
				int order;

				RangeQuery_pre(context->GetNodeBuffers());

				for (;;)
				{
					if ((leaf = cTreeNode<TKey>::IsLeaf(nodeIndex)))
					{
						if (context->GetReadFlag())
						{
							currentNode = parent::ReadLeafNodeR(nodeIndex);
						}
						else
						{
							currentNode = parent::ReadLeafNodeW(nodeIndex);
						}
					}
					else
					{
						if (context->GetReadFlag())
						{
							currentNode = parent::ReadInnerNodeR(nodeIndex);
						}
						else
						{
							currentNode = parent::ReadInnerNodeW(nodeIndex);
						}
					}

					order = currentNode->FindOrder(*il, cTreeNode<TKey>::FIND_SBE, &context->GetNodeBuffers()->itemBuffer);

					if (order == cTreeNode<TKey>::FIND_NOTEXIST)
					{
						if (context->GetReadFlag())
						{
							parent::mSharedCache->UnlockR(currentNode);
						}
						else
						{
							parent::mSharedCache->UnlockW(currentNode);
						}

						RangeQuery_post(context->GetNodeBuffers());
						return false;
					}

					if (!leaf)
					{
						nodeIndex = currentNode->GetLink(order);
						if (context->GetReadFlag())
						{
							parent::mSharedCache->UnlockR(currentNode);
						}
						else
						{
							parent::mSharedCache->UnlockW(currentNode);
						}
					}
					else
					{
						context->SetCurrentNode(currentNode);
						context->SetCurrentOrder(order);
						context->SetKeyH(ih);
						context->SetFirstPass(true);
						context->Open();
						return true;
					}
				}
			}

			template <class TKey>
			bool cContextBpTree<TKey>::Advance(cBpTreeContext<TKey>* context)
			{
				if (!context->GetFirstPass())
				{
					if (context->IncrementCurrentOrder() == context->GetCurrentNode()->GetItemCount())
					{
						if (context->GetReadFlag())
						{
							parent::mSharedCache->UnlockR(context->GetCurrentNode());
						}
						else
						{
							parent::mSharedCache->UnlockW(context->GetCurrentNode());
						}

						context->SetCurrentOrder(0);
						if (context->GetCurrentNode()->GetExtraLink(1) == cTreeNode<TKey>::EMPTY_LINK)
						{
							context->SetCurrentNode(NULL);
							RangeQuery_post(context->GetNodeBuffers());
							context->Close();
							return false;
						}
						else
						{
							if (context->GetReadFlag())
							{
								context->SetCurrentNode(parent::ReadLeafNodeR(context->GetCurrentNode()->GetExtraLink(1)));
							}
							else
							{
								context->SetCurrentNode(parent::ReadLeafNodeW(context->GetCurrentNode()->GetExtraLink(1)));
							}
						}
					}
				}
				context->SetFirstPass(false);

				const char* data = context->GetCurrentNode()->GetCKey(context->GetCurrentOrder(), &context->GetNodeBuffers()->itemBuffer);
				TKey* key = context->GetKeyH();
				int cmp = key->Compare(data, GetKeyDescriptor());

				if (cmp >= 0)
				{
					context->SetItem(context->GetCurrentNode()->GetCItem(context->GetCurrentOrder(), &context->GetNodeBuffers()->itemBuffer));
					return true;
				}
				else
				{
					if (context->GetReadFlag())
					{
						parent::mSharedCache->UnlockR(context->GetCurrentNode());
					}
					else
					{
						parent::mSharedCache->UnlockW(context->GetCurrentNode());
					}

					context->SetCurrentNode(NULL);
					RangeQuery_post(context->GetNodeBuffers());
					context->Close();
					return false;
				}
			}

			template <class TKey>
			void cContextBpTree<TKey>::CloseContext(cBpTreeContext<TKey>* context)
			{
				if (context->IsOpen())
				{
					if (context->GetCurrentNode() != NULL)
					{
						if (context->GetReadFlag())
						{
							parent::mSharedCache->UnlockR(context->GetCurrentNode());
						}
						else
						{
							parent::mSharedCache->UnlockW(context->GetCurrentNode());
						}
						context->SetCurrentNode(NULL);
					}

					RangeQuery_post(context->GetNodeBuffers());

					context->Close();
				}
			}
		}
	}
}
#endif