/**
*	\file cBpContextTree.h
*	\author Lubomir Turcik
*	\version 0.1
*	\date 2012
*	\brief Implement persistent B-tree
*/

#ifndef __cBpContextTree_h__
#define __cBpContextTree_h__
#pragma warning(disable : 4505)

#include "dstruct/paged/b+tree/cB+TreeConst.h"
#include "dstruct/paged/b+tree/cCommonB+Tree.h"
#include "dstruct/paged/core/cTreeNode.h"
#include "dstruct/paged/core/cTreeNode_SubNode.h"
#include "xdb/xmldocumentstorage/cXMLDocumentStorageContext.h"
#include "xdb/queryprocessing/cQueryStat.h"

#include "xdb/nodelabel/range/cRangeLabeling.h"

/**
* Implement persistent B-tree with fixed size of key and leaf data.
* Parameters of the template:
*		- TKey - Type of the key value.
*
*	\author Lubomír Turcik
*	\version 0.1
*	\date jan 2012
**/
namespace dstruct {
	namespace paged {
		namespace bptree {

			using namespace xdb::queryprocessing;

			template <class TKey> 
			class cBpContextTree: public cCommonBpTree<cTreeNode<TKey>,cTreeNode<TKey>, TKey>
			{
				cQueryStat*								mQueryStat;
			public:
				cBpContextTree();	
				~cBpContextTree();

				cTreeItemStream<TKey>* RangeQuery(const TKey & il, const TKey & ih, unsigned int finishResultSize = 0);
				bool RangeQuery(cXMLDocumentStorageContext<TKey>* context, unsigned int finishResultSize = 0);
				void UnlockNode(cXMLDocumentStorageContext<TKey>* context);

				void SetQueryStat(cQueryStat* stat)		{ mQueryStat = stat; }
			};  

			template <class TKey> 
			cBpContextTree<TKey>::cBpContextTree():cCommonBpTree<cTreeNode<TKey>,cTreeNode<TKey>, TKey>()
			{
			}

			template <class TKey> 
			cBpContextTree<TKey>::~cBpContextTree()
			{
			}

			/**
			* Range query on the btree. Search for keys which lie within interval <il, ih>.
			* \param il Lower key of the interval.
			* \param ih Higher key of the interval.
			* \return Result set with (key,data) pairs
			*/
			template <class TKey>
			cTreeItemStream<TKey>* cBpContextTree<TKey>
				::RangeQuery(const TKey & il, const TKey & ih, unsigned int finishResultSize)
			{

				tNodeIndex nodeIndex = this->mHeader->GetRootIndex();
				unsigned int chld;
				bool leaf;
				//char* buffer = NULL; // it is for coding and ri DsMode
				//cMemoryBlock* bufferMemBlock = NULL;
				cTreeNode<TKey> *currentNode = NULL;
				cTreeNode<TKey> *currentLeafNode = NULL;
				cTreeNode<TKey> *tmpLeaf = NULL;
				cTreeItemStream<TKey>* itemStream = (cTreeItemStream<TKey>*)this->mQuickDB->GetResultSet();
				cNodeBuffers<TKey> nodeBuffers;

				itemStream->SetNodeHeader(this->mHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE));

				RangeQuery_pre(&nodeBuffers);
				//RangeQuery_pre(&bufferMemBlock, &buffer);

				int order;
				bool cont = true;

				if (this->mDebug)
				{
					il.Print("\n", GetKeyDescriptor());
					ih.Print("\n", GetKeyDescriptor());
				}

				if (this->mDebug && this->mSharedCache->CheckLocks())
				{
					printf("Range query:\n");
					this->mSharedCache->PrintLocks();
				}


				for ( ; ; )
				{
					if ((leaf = cTreeNode<TKey>::IsLeaf(nodeIndex)) == true)
					{
						currentLeafNode = ReadLeafNodeR(nodeIndex);
						if (this->mDebug) {currentLeafNode->Print();}
					}
					else if (nodeIndex == this->mHeader->GetRootIndex() && (this->mHeader->GetHeight()==0))
					{
						leaf = true;
						currentLeafNode = ReadLeafNodeR(nodeIndex);
						if (this->mDebug) {currentLeafNode->Print();}
					}
					else
					{
						currentNode = ReadInnerNodeR(nodeIndex);
						if (this->mDebug) {currentNode->Print();}
					}

					if (leaf)
					{

						order=currentLeafNode->FindOrder(il,currentLeafNode->FIND_SBE, &nodeBuffers.itemBuffer);
						if (order == cTreeNode<TKey>::FIND_NOTEXIST) 
						{
							this->mSharedCache->UnlockR(currentLeafNode);
							return itemStream;
						}

						while(ih.Compare(currentLeafNode->GetCItem(order), GetKeyDescriptor()) >= 0)
						{
							itemStream->Add(currentLeafNode->GetCItem(order, &nodeBuffers.itemBuffer));
							if (++order == currentLeafNode->GetItemCount()) 
							{
								order = 0;
								if (currentLeafNode->GetExtraLink(1)==currentLeafNode->EMPTY_LINK) 
								{
									break;
								}
								this->mSharedCache->UnlockR(currentLeafNode);
								currentLeafNode = ReadLeafNodeR(currentLeafNode->GetExtraLink(1));
								if (this->mDebug) {currentLeafNode->Print();}				
							}
							if (finishResultSize > 0 && itemStream->GetItemCount() == finishResultSize)
							{
								break;
							}
						} 

						this->mSharedCache->UnlockR(currentLeafNode);
						itemStream->FinishWrite();
						return itemStream;
					}
					else 
					{
						if ((chld = currentNode->FindOrder(il, cTreeNode<TKey>::FIND_SBE, &nodeBuffers.itemBuffer)) != cTreeNode<TKey>::FIND_NOTEXIST)
						{
							if (this->mHeader->DuplicatesAllowed())
							{
								//while (chld >= 0 && currentNode->GetItem(chld)->Equal(il, true) >= 0)
								while (chld >= 0 && il.Compare(currentNode->GetCItem(chld, &nodeBuffers.itemBuffer), GetKeyDescriptor()) < 0)
								{
									if (chld-- == 0) {break;}
								}
								chld++;
							}
							nodeIndex = currentNode->GetLink(chld);
							this->mSharedCache->UnlockR(currentNode);
						}
						else
						{
							//printf("*** RangeQuery: Critical Error - inner node item not found! ***\n");
							//if (this->mDebug) 
							//{ 
							//	currentNode->Print(cObject::MODE_BIN); 
							//	printf("\nBorders: <");
							//	//key.Print();
							//	//end.Print();
							//	printf(">\n");
							//}
							//exit(0);
							this->mSharedCache->UnlockR(currentNode);
							break;
						}
					}
				}

				/*if (buffer != NULL)
				{
					this->mQuickDB->GetMemoryManager()->ReleaseMem(bufferMemBlock);
				}*/
				RangeQuery_post(&nodeBuffers);

				if (this->mDebug && this->mSharedCache->CheckLocks())
				{
					printf("Range query:\n");
					this->mSharedCache->PrintLocks();
				}

				itemStream->FinishWrite();
				return itemStream;
			}

			/**
			* Range query on the btree. Search for keys which lie within interval <il, ih>.
			* \param il Lower key of the interval.
			* \param ih Higher key of the interval.
			* \return Result set with (key,data) pairs
			*/
			template <class TKey>
			bool cBpContextTree<TKey>
				::RangeQuery(cXMLDocumentStorageContext<TKey>* context, unsigned int finishResultSize)
			{
				const char *il = context->GetStartKey();
				const char *ih = context->GetEndKey();
				tNodeIndex nodeIndex = this->mHeader->GetRootIndex();
				unsigned int chld;
				bool leaf;
				cTreeNode<TKey> *currentNode = NULL;
				cTreeNode<TKey> *currentLeafNode = NULL;
				cTreeNode<TKey> *tmpLeaf = NULL;
				int order;
				bool cont = true;
				unsigned short term;
				unsigned int arrayIndex;
				unsigned int arrayPosition;
				int i = 0;
				cNodeBuffers<TKey> nodeBuffers;
				//char* buffer = NULL; // it is for coding and ri DsMode
				//cMemoryBlock* bufferMemBlock = NULL;

				//RangeQuery_pre(&bufferMemBlock, &buffer);
				RangeQuery_pre(&nodeBuffers);

				
				if (false)
				{
					printf("******************************\n");
					TKey::Print(il, "\n", GetKeyDescriptor());
					TKey::Print(ih, "\n", GetKeyDescriptor());
				}

				if (this->mDebug && this->mSharedCache->CheckLocks())
				{
					printf("Range query:\n");
					this->mSharedCache->PrintLocks();
				}

				//****************************************************************
				if (context->GetContinue()) 
				{
					nodeIndex = context->GetNodeIndex();		//read node index from context
				}

				for ( ; ; )
				{
					if (!context->GetContinue())
					{
						if ((leaf = cTreeNode<TKey>::IsLeaf(nodeIndex)) == true)
						{
							currentLeafNode = ReadLeafNodeR(nodeIndex);
							if (this->mDebug) {currentLeafNode->Print();}
						}
						else if (nodeIndex == this->mHeader->GetRootIndex() && (this->mHeader->GetHeight()==0))
						{
							leaf = true;
							currentLeafNode = ReadLeafNodeR(nodeIndex);
							if (this->mDebug) {currentLeafNode->Print();}
						}
						else
						{
							currentNode = ReadInnerNodeR(nodeIndex);

							if (this->mDebug) {currentNode->Print();}
						}
					}
					else 
					{
						leaf = true;
						//currentLeafNode = ReadLeafNodeR(nodeIndex);	
						currentLeafNode = context->GetNode();	
					}

					if (leaf)
					{
						//*********************************************************
						if (context->GetContinue()) 
						{
							order = context->GetNodePosition();
						} else 
						{
							mQueryStat->IncrementXmlDocStorageSeeks();
							order = currentLeafNode->FindOrder(il, currentLeafNode->FIND_SBE, &nodeBuffers.itemBuffer);
						}

						if (order == cTreeNode<TKey>::FIND_NOTEXIST) 
						{
							this->mSharedCache->UnlockR(currentLeafNode);
							return false;
						}

						while (TKey::Compare(ih, currentLeafNode->GetCItem(order, &nodeBuffers.itemBuffer), GetKeyDescriptor()) >= 0)
						{
							//*********************************************************
							if (!context->GetContinue())
							{
								//context->Print();
								mQueryStat->IncrementXmlDocStorageMoves();
								term = *(((unsigned short*) currentLeafNode->GetData(order, &nodeBuffers.itemBuffer)) + 3);	// TODO v datech je od 8 byte termId daneho elementu; přidat alespoň konstantu
								if (this->mDebug)
								{
									xdb::cRangeLabeling::Print(currentLeafNode->GetItemPtr(order), "\n");
									printf("Nalezeny term : %d\n", term);
								}
								bool found = false;
								for (i = 0; i < context->GetCountTerms(); i++)
								{
									if ((*(context->GetFindTerms()+i) == term) || 
										(*context->GetFindTerms() == cXMLDocumentStorageContext<TKey>::TERM_NOTSPEC)) 
									{
										context->SetNodeIndex(currentLeafNode->GetIndex());
										context->SetTreeItem((char*) currentLeafNode->GetCItem(order, &nodeBuffers.itemBuffer));
										context->SetNodePosition(order);
										context->SetNode(currentLeafNode);
										//context->SetContinue(true);
										//context->Print();
										if (context->GetReadArrayItem())
										{
											context->SetArrayIndex(*(((unsigned int*) currentLeafNode->GetData(order, &nodeBuffers.itemBuffer))));	//nutná úprava 
											context->SetArrayPosition(*(((unsigned int*) currentLeafNode->GetData(order, &nodeBuffers.itemBuffer)) + 1));	//nutná úprava 
										}
										context->SetCurrentTerm(i);
										context->SetContinue(true);
										found = true;
										break;
									}
								}
								if (found)
								{
									break;
								}
								//if ((*(context->GetFindTerms()+i) == term) || 
								//	(*context->GetFindTerms() == cXMLDocumentStorageContext<TKey>::TERM_NOTSPEC)) 
								//{
								//		break;
								//}
								//Vymyslet lepsi reseni
							} else 
							{
								context->SetContinue(false);
							}
							//itemStream->Add(currentLeafNode->GetCItem(order));

							if (++order == currentLeafNode->GetItemCount()) 
							{
								order = 0;
								if (currentLeafNode->GetExtraLink(1)==currentLeafNode->EMPTY_LINK) 
								{
									break;
								}
								this->mSharedCache->UnlockR(currentLeafNode);
								currentLeafNode = ReadLeafNodeR(currentLeafNode->GetExtraLink(1));
								if (this->mDebug) {currentLeafNode->Print();}				
							}

						} 

						//this->mSharedCache->UnlockR(currentLeafNode);
						if (context->GetContinue()) return true;
						this->mSharedCache->UnlockR(currentLeafNode);
						return false;
					}
					else 
					{
						if ((chld = currentNode->FindOrder(il, cTreeNode<TKey>::FIND_SBE, &nodeBuffers.itemBuffer)) != cTreeNode<TKey>::FIND_NOTEXIST)
						{
							if (this->mHeader->DuplicatesAllowed())
							{
								//while (chld >= 0 && currentNode->GetItem(chld)->Equal(il, true) >= 0)
								while (chld >= 0 && TKey::Compare(il, currentNode->GetCItem(chld, &nodeBuffers.itemBuffer), GetKeyDescriptor()) < 0)
								{
									if (chld-- == 0) {break;}
								}
								chld++;
							}
							nodeIndex = currentNode->GetLink(chld);
							this->mSharedCache->UnlockR(currentNode);
						}
						else
						{
							//printf("*** RangeQuery: Critical Error - inner node item not found! ***\n");
							//if (this->mDebug) 
							//{ 
							//	currentNode->Print(cObject::MODE_BIN); 
							//	printf("\nBorders: <");
							//	//key.Print();
							//	//end.Print();
							//	printf(">\n");
							//}
							//exit(0);
							this->mSharedCache->UnlockR(currentNode);
							break;
						}
					}
				}

				/*if (buffer != NULL)
				{
					this->mQuickDB->GetMemoryManager()->ReleaseMem(bufferMemBlock);
				}*/
				RangeQuery_post(&nodeBuffers);

				if (this->mDebug && this->mSharedCache->CheckLocks())
				{
					printf("Range query:\n");
					this->mSharedCache->PrintLocks();
				}

				return false;
			}

			/**
			* Range query on the btree. Search for keys which lie within interval <il, ih>.
			* \param il Lower key of the interval.
			* \param ih Higher key of the interval.
			* \return Result set with (key,data) pairs
			*/
			template <class TKey>
			void cBpContextTree<TKey>
				::UnlockNode(cXMLDocumentStorageContext<TKey>* context)
			{
				this->mSharedCache->UnlockR(context->GetNode());
			}




		}}}
#endif