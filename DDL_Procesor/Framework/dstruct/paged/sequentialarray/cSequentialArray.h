/**
*	\file cSequentialArray.h
*	\author Radim Baca
*	\version 0.1
*	\date jul 2011
*	\brief Persistent array of items with variable length.
*/


#ifndef __cSequentialArray_h__
#define __cSequentialArray_h__

#include "common/stream/cStream.h"
#include "common/stream/cCharStream.h"

#include "dstruct/paged/core/cQuickDB.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayHeader.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayContext.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayNode.h"
#include "dstruct/paged/core/cNodeCache.h"

using namespace dstruct::paged::core;

namespace dstruct {
	namespace paged {
		namespace sqarray {
/**
* Persistent array of items with variable length.
* Data structure support only basic operations (such as get and add). 
* We can add only to the end of the array and clear the whole array.
* Currently we can add items only after create of the data structure (not after open).
*
* Template parameters:
*	- TItem - type of the item inherited from the cBasicType.
*	- cSequentialArrayNode<TItem> - class representing the node of array
*
*	\author Radim Baca
*	\version 0.1
*	\date jul 2011
**/
template <class TItem>
class cSequentialArray
{
private:

	// cEnhancedCache<cSequentialArrayNode<TItem>, cSequentialArrayHeader<TItem>>*		mCache;
	cNodeCache*		mCache;
	cQuickDB*		mQuickDB;
	cSequentialArrayHeader<TItem>* mHeader;  // header of the sequential array
	
	cStream*		mStream;
	cCharStream*	mCharStream;
	bool			mDebug;
	bool			mReadOnly;

	unsigned int	mNodeHeaderId;  // id of the node header

	static const unsigned int POOL_COUNT = 2;
public:
	cSequentialArray();
	~cSequentialArray();

	void Null();
	void Init();
	void Delete();
	inline void Flush();

	bool Create(cSequentialArrayHeader<TItem>* header, cQuickDB* quickDB);
	bool Create(cSequentialArrayHeader<TItem>* header, cNodeCache* nodeCache);
	bool Open(cSequentialArrayHeader<TItem>* header, cQuickDB* quickDB);
	//bool Open(char* fileName, bool readOnly, unsigned int cacheSize, unsigned int cacheType = cBucketRecordStorage::SORTED_ARRAY_TYPE);
	bool Close();
	void Clear();
	void Clear(cSequentialArrayNode<TItem>* node);
	void Preload();

	inline cSequentialArrayNode<TItem>* ReadNewNode();
	inline cSequentialArrayNode<TItem>* ReadNodeR(unsigned int index);
	inline cSequentialArrayNode<TItem>* ReadNodeW(unsigned int index);

	inline void UnlockR(cSequentialArrayNode<TItem>* node);
	inline void UnlockW(cSequentialArrayNode<TItem>* node);
	inline void UnlockR(cSequentialArrayNode<TItem>** node);
	inline void UnlockW(cSequentialArrayNode<TItem>** node);

	inline cCacheStatistics* GetCacheStatistics() { return mCache->GetCacheStatistics(); }
	inline cSequentialArrayHeader<TItem>* GetHeader() { return mHeader; }
	inline cNodeCache* GetCache() { return mCache; }

	bool OpenFirstContext(cSequentialArrayContext<TItem>* context);
	bool OpenContext(unsigned int nodeIndex, unsigned int position, cSequentialArrayContext<TItem>* context, bool readFlag = true);
	void CloseContext(cSequentialArrayContext<TItem>* context);
	bool Advance(cSequentialArrayContext<TItem>* context);

	bool GetItem(unsigned int nodeIndex, unsigned int position, char* dest);

	bool AddItem(unsigned int& index, unsigned int& position, const TItem & item);
	bool AddMaxItem(unsigned int& index, unsigned int& position, const TItem & item);

	inline unsigned int AddItem_NoLock(const TItem & item, unsigned int itemSize);
	bool UpdateItem(unsigned int index, unsigned int position, const TItem & item);
	bool UpdateActualItem(cSequentialArrayContext<TItem>* context);
	bool UpdateNextItem(cSequentialArrayContext<TItem>* context);
	bool Find(unsigned int nodeIndex, const TItem &item, cSequentialArrayContext<TItem>* context);

	/**
	* \return Number of items in the array
	*/
	inline unsigned int GetItemCount()	const { return mHeader->GetItemCount(); }
	void GetArrayEnd(unsigned int &nodeId, unsigned int &position);
	void SetArrayEnd(unsigned int nodeId, unsigned int position);

	void SetDebug(bool debug) { mDebug = debug; }
	void Print() const;
	void PrintInfo() const;
	void PrintLocks() const { mCache->PrintLocks(); }	
	void PrintCache() const;

	uint GetIndexSize(uint blockSize);
	float GetIndexSizeMB(uint blockSize);

	inline const cDTDescriptor* GetKeyDescriptor() { return mHeader->GetNodeHeader(0)->GetKeyDescriptor(); }
};

	
template <class TItem>
cSequentialArray<TItem>::cSequentialArray()
{
	mReadOnly = false;
}

template <class TItem>
cSequentialArray<TItem>::~cSequentialArray()
{
	Delete();
}

/**
* NULL all variables in the class. This method has to be called before Init() method, 
* if the constructor has been skiped during the creation (It happen when we create object from the char array).
*/
template <class TItem>
void cSequentialArray<TItem>::Null()
{
	mCache = NULL;
	mHeader = NULL;
}

/**
* Create all objects and initialize default values. Method first delete old objects (if there are any).
*/
template <class TItem>
void cSequentialArray<TItem>::Init()
{
	Delete();

	mDebug =  false;
}

/**
* Delete all objects in this persistent array.
*/
template <class TItem>
void cSequentialArray<TItem>::Delete()
{

}

/**
* Create the persistent array with existing cache.
* \param fileName Name of the file.
* \param nodeCache
*/
template <class TItem>
bool cSequentialArray<TItem>::Create(cSequentialArrayHeader<TItem>* header, cNodeCache* nodeCache)
{
	mHeader = header;

	mQuickDB = NULL;
	mCache = nodeCache;
	mCache->Register(mHeader);
	mNodeHeaderId = mHeader->GetNodeType(0);

	cSequentialArrayHeader<TItem> **p = &header;
	//mCache->CreateHeadersForRows(*p);

	mHeader->SetRewriting(false);
	mHeader->SetNodeCount(1);

	cSequentialArrayNode<TItem>* node = ReadNewNode();
	mHeader->SetFirstNodeIndex(node->GetIndex());
	mHeader->SetLastNodeIndex(mHeader->GetFirstNodeIndex());
	mCache->UnlockW(node);

	return true;
}

/**
* Create the persistent array with existing cache.
* \param fileName Name of the file.
* \param cache
*/
template <class TItem>
bool cSequentialArray<TItem>::Create(cSequentialArrayHeader<TItem>* header, cQuickDB* quickDB)
{
	mHeader = header;

	mQuickDB = quickDB;
	mCache = quickDB->GetNodeCache();
	mCache->Register(mHeader);
	mNodeHeaderId = mHeader->GetNodeType(0);

	cSequentialArrayHeader<TItem> **p = &header;
	//mCache->CreateHeadersForRows(*p);

	mHeader->SetRewriting(false);
	mHeader->SetNodeCount(1);

	cSequentialArrayNode<TItem>* node = ReadNewNode();
	mHeader->SetFirstNodeIndex(node->GetIndex());
	mHeader->SetLastNodeIndex(mHeader->GetFirstNodeIndex());
	mCache->UnlockW(node);

	return true;
}


/**
* Open existing array.
* \param header Array header. All values will be read from the secondary storage. Only the node headers has to be preallocated and the data structure name has to be properly set.
* \param sharedCache Opened cache.
*/
template <class TItem>
bool cSequentialArray<TItem>::Open(cSequentialArrayHeader<TItem>* header, cQuickDB* quickDB)
{
	bool ret;

	mQuickDB = quickDB;
	mCache = quickDB->GetNodeCache();
	mHeader = header;

	ret = mCache->LookForHeader(mHeader);

	mNodeHeaderId = mHeader->GetNodeType(0);

	cSequentialArrayHeader<TItem> **p = &header;
	//mCache->CreateHeadersForRows(*p);

	return ret;
}

//
///**
//* Open existing persistent array.
//* \param fileName Name of the file.
//* \param readOnly Array is opened as a read only.
//* \param cacheSize Number of cache nodes used by this data structure.
//*/
//template <class TItem>
//bool cSequentialArray<TItem>::Open(
//		char* fileName, 
//		bool readOnly, 
//		unsigned int cacheSize, 
//		unsigned int cacheType = cBucketRecordStorage::SORTED_ARRAY_TYPE)
//{
//	bool ret = false;
//	mReadOnly = false;
//
//	if (!mStream->Open(fileName, OPEN_ALWAYS))
//	{
//		throw cExceptionDataStructureOpen(fileName);
//	}
//	else
//	{
//		// TODO tady bude problem se sdilenou cache, jelikoz se nacte hlavicka, ktera byla pouzita pro vytvoreni cache a ta nemusi byt identicka s hlavickou datove struktury. Tim padem bude blbe first node a last node id.
//		mStream->Seek(0);
//		ret = mCharStream->Read(mStream, mHeader->GetSize());
//		mCharStream->Seek(0);
//		ret &= mHeader->Read(mCharStream);
//	}
//
//	mCache->Open(cacheSize, mHeader, mStream, mReadOnly, cacheType); // open and resize cache
//
//	return ret;
//}

/**
* Close persistent array.
*/
template <class TItem>
bool cSequentialArray<TItem>::Close()
{
	/*
	mCache->Close();
	if (mStream != NULL)
	{
		//if (!mReadOnly)
		//{		
			// write header onto secondary storage
			mCharStream->Seek(0);
			mHeader->Write(mCharStream);

			mStream->Seek(0);
			mCharStream->Write(mStream, mHeader->GetSize());
		//}

		mStream->Close();
	}	mk: 20130129 */

	return true;
}

/**
* Clear the array.
*/
template <class TItem>
void cSequentialArray<TItem>::Clear()
{
	// TODO projit pole a nastavit u uzlu v cache atribut mModified na false
	mHeader->SetLastNodeIndex(mHeader->GetFirstNodeIndex());
	mHeader->SetNodeCount(1);
	//mHeader->SetItemCount(0);
	cSequentialArrayNode<TItem>* node = ReadNodeW(mHeader->GetFirstNodeIndex());
	node->Clear();
	mCache->UnlockW(node);
}

/**
* Clear the array and set the first node
* \param node Node which should be the first node of the array.
*/
template <class TItem>
void cSequentialArray<TItem>::Clear(cSequentialArrayNode<TItem>* node)
{
	// TODO projit pole a nastavit u uzlu v cache atribut mModified na false
	mHeader->SetFirstNodeIndex(node->GetIndex());
	mHeader->SetLastNodeIndex(mHeader->GetFirstNodeIndex());
	mHeader->SetNodeCount(1);
	mHeader->SetItemCount(0);
	node->Clear();
}

/**
* Store all modified nodes in main memory onto secondary storage.
*/
template <class TItem>
void cSequentialArray<TItem>::Flush()
{
	if (!mReadOnly)
	{		
		mCharStream->Seek(0);
		mHeader->Write(mCharStream);
		mStream->Seek(0);
		mCharStream->Write(mStream, mHeader->GetSize());
		
		mCache->Flush();
	}
}

template <class TItem>
void cSequentialArray<TItem>::Preload()
{
	uint nodeIndex = mHeader->GetFirstNodeIndex();
	for (uint i = 0; i < mHeader->GetNodeCount(); i++)
	{
		cSequentialArrayNode<TItem> *node = ReadNodeR(nodeIndex);
		nodeIndex = node->GetNextNodeIndex();
		mCache->UnlockR(node);
	}
}

/**
* Open context and set it on the first item
*/
template <class TItem>
bool cSequentialArray<TItem>
	::OpenFirstContext(cSequentialArrayContext<TItem>* context)
{
	return OpenContext(mHeader->GetFirstNodeIndex(), 0, context);
}

/**
* Open context.
* \param nodeIndex Index of the node where the context is opened.
* \param position Position within the node (in bytes).
* \param context Contex where the item is decoded and position information is stored. 
*        This context have to be closed when this method is called.
* \return true If the context was succesfully opened
*/
template <class TItem>
bool cSequentialArray<TItem>::OpenContext(unsigned int nodeIndex, 
	unsigned int position, cSequentialArrayContext<TItem>* context, bool readFlag)
{
	//if (nodeIndex >= mHeader->GetNodeCount())
	//{
	//	return false;
	//}
	assert(nodeIndex > 0);

	context->Open();
	context->SetReadFlag(readFlag);
	if (readFlag)
	{ 
		context->SetNode(ReadNodeR(nodeIndex));
	}
	else
	{
		context->SetNode(ReadNodeW(nodeIndex));
	}

	if (position >= ((cSequentialArrayNode<TItem> *)context->GetRefNode())->GetUsedSpace())
	{
		context->SetPosition(((cSequentialArrayNode<TItem>*)context->GetRefNode())->GetNextNodeIndex());
		if (readFlag)
		{
			mCache->UnlockR((cSequentialArrayNode<TItem>*)context->GetRefNode());
		}
		else
		{
			mCache->UnlockW((cSequentialArrayNode<TItem>*)context->GetRefNode());
		}
		context->SetNode(NULL);
		context->Close();
		return false;
	}
	context->SetPosition(position);

	context->SetItem(((cSequentialArrayNode<TItem>*)context->GetRefNode())->GetItem(position));
	context->SetPosition(position + TItem::GetSize(context->GetItem(), GetKeyDescriptor()));

	return true;
}

/**
* Close context.
* \param context Context which is closed. Have to opened when the method is called.
*/
template <class TItem>
void cSequentialArray<TItem>::CloseContext(cSequentialArrayContext<TItem>* context)
{
	if (context->GetRefNode() != NULL)
	{
		if (context->GetReadFlag())
		{
			mCache->UnlockR((cSequentialArrayNode<TItem>*)context->GetRefNode());
		}
		else
		{
			mCache->UnlockW((cSequentialArrayNode<TItem>*)context->GetRefNode());
		}
		context->SetNode(NULL);
	}
	context->Close();
}

/**
* Move the context cursor to the next item in the array
* \param context Current context.
* \return false is we have reached the end of the array.
*/
template <class TItem>
bool cSequentialArray<TItem>::Advance(cSequentialArrayContext<TItem>* context)
{
	cSequentialArrayNode<TItem> *node = (cSequentialArrayNode<TItem> *)context->GetRefNode();

	assert(context->GetPosition() <= node->GetUsedSpace());
	//if (context->GetPosition() > node->GetUsedSpace())
	//{
	//	printf("*");
	//}

	if (context->GetPosition() == node->GetUsedSpace())
	{
		// in the case that we have to read next node
		if (node->GetIndex() == mHeader->GetLastNodeIndex())
		{
			// we are at the end of the array
			if (context->GetReadFlag())
			{
				mCache->UnlockR(node);
			}
			else
			{
				mCache->UnlockW(node);
			}
			context->SetNode(NULL);
			return false;
		}
		tNodeIndex nodeIndex = node->GetNextNodeIndex();
		if (context->GetReadFlag())
		{
			mCache->UnlockR(node);
		}
		else
		{
			mCache->UnlockW(node);
		}
		if (context->GetReadFlag())
		{
			context->SetNode(ReadNodeR(nodeIndex));
		}
		else
		{
			context->SetNode(ReadNodeW(nodeIndex));
		}
		node = (cSequentialArrayNode<TItem> *)context->GetRefNode();
		context->SetPosition(0);
	} 

	context->SetItem(node->GetItem(context->GetPosition()));
	context->SetPosition(context->GetPosition() + TItem::GetSize(context->GetItem(), GetKeyDescriptor()));
	return true;
}

/**
* Get item from the persitent array and copy it into dest memory. nodeIndex=0 and
* position=0 means that the searchich starts in the start of the array.
* \param nodeIndex Index of the node
* \param position Position within the node.
* \param dest Destination memory where the item is stored.
* \return true If the item was succesfully readed
*/
template <class TItem>
bool cSequentialArray<TItem>::GetItem(unsigned int nodeIndex, 
										unsigned int position, 
										char* dest)
{
	cSequentialArrayNode<TItem> *node = ReadNodeR(nodeIndex);
	if (position >= node->GetUsedSpace())
	{
		mCache->UnlockR(node);
		return false;
	}

	const char* data = node->GetCItem(position);
	memcpy(dest, data, TItem::GetSize(data, GetKeyDescriptor()));

	mCache->UnlockR(node);

	return true;
}

/**
 * Find the item.
 *
 * \param nodeIndex Index of the node where the context is opened.
 * \item  item Item to be found
 * \param context Contex where the item is decoded and position information is stored. 
 *        This context have to be closed when this method is called.
 * \return true If the item is found
 *
 * \author Michal Kratky
 * \date jul 2009
 */
template <class TItem>
bool cSequentialArray<TItem>::Find(unsigned int nodeIndex,
       const TItem &item, cSequentialArrayContext<TItem>* context)
{
	bool ret = false;
	assert(nodeIndex < mHeader->GetNodeCount());

	OpenContext(nodeIndex, 0, context);
	do
	{
		if (TItem::Equals(context->GetRefItem(), item))
		{
			ret = true;
			break;
		}
		
	} while(Advance(context));

	CloseContext(context);

	return ret;
}

/**
* Warning - this method will not work in a parallel environment!
* Insert item at the end of the array.
* \param item Item to be inserted.
* \return Node index - this value can be harmed 
*/
template <class TItem>
unsigned int dstruct::paged::sqarray::cSequentialArray<TItem>::AddItem_NoLock(const TItem & item, unsigned int itemSize)
{
	cSequentialArrayNode<TItem>* node;

	node = ReadNodeW(mHeader->GetLastNodeIndex());
	//if (node->TestAddItem(item, mHeader->GetRefBuffer(), mHeader->GetBufferSize()))
	if (node->GetNodeFreeSpace() >= cSequentialArrayNode<TItem>::GetNodeExtraSize() + itemSize)
	{
		// in the case that the item fits into the current node
		node->AddItem(item);
		mCache->UnlockW(node);
	} else
	{
		// set the reference from the current node to the next node
		if (node->GetNextNodeIndex() != cNode::EMPTY_INDEX)
		{
			unsigned int newIndex = node->GetNextNodeIndex();
			mCache->UnlockW(node);
			node = ReadNodeW(newIndex);
			node->Clear();
		} else
		{
			cSequentialArrayNode<TItem>* newNode = ReadNewNode();
			newNode->SetNextNodeIndex(cNode::EMPTY_INDEX);
			node->SetNextNodeIndex(newNode->GetIndex());
			mCache->UnlockW(node);
			node = newNode;
		}

		// in the case that the item does not fit into the last node
		mHeader->IncNodeCount();
		mHeader->SetLastNodeIndex(node->GetIndex());

		node->AddItem(item);
		mCache->UnlockW(node);
	}

	return node->GetIndex(); // warning - this value can be different, because we read after unlock!
}

/**
* Insert item at the end of the array.
* \param index Return parameter; Node index, where the item was inserted.
* \param position Return parameter; Position in node (in bytes), where the item start.
* \param item Item to be inserted.
*/
template <class TItem>
bool cSequentialArray<TItem>::AddItem(unsigned int& index, unsigned int& position, const TItem & item)
{
	cSequentialArrayNode<TItem>* node;

	node = ReadNodeW(mHeader->GetLastNodeIndex());
	//if (node->TestAddItem(item, mHeader->GetRefBuffer(), mHeader->GetBufferSize()))
	if (node->GetNodeFreeSpace() >= cSequentialArrayNode<TItem>::GetNodeExtraSize() + item.GetSize(GetKeyDescriptor()))
	{
		// in the case that the item fits into the current node
		position = node->GetUsedSpace();
		node->AddItem(item);
		mCache->UnlockW(node);
	} else
	{
		// set the reference from the current node to the next node
		if (node->GetNextNodeIndex() != cNode::EMPTY_INDEX)
		{
			unsigned int newIndex = node->GetNextNodeIndex();
			mCache->UnlockW(node);
			node = ReadNodeW(newIndex);
			node->Clear();
		} else
		{
			cSequentialArrayNode<TItem>* newNode = ReadNewNode();
			newNode->SetNextNodeIndex(cNode::EMPTY_INDEX);
			node->SetNextNodeIndex(newNode->GetIndex());
			mCache->UnlockW(node);
			node = newNode;
		}

		// in the case that the item does not fit into the last node
		mHeader->IncNodeCount();
		mHeader->SetLastNodeIndex(node->GetIndex());
		position = 0;

		node->AddItem(item);
		mCache->UnlockW(node);
	}
	index = mHeader->GetLastNodeIndex();
	mHeader->IncItemCount();

	return true;
}


/**
* Insert item at the end of the array.
* \param index Return parameter; Node index, where the item was inserted.
* \param position Return parameter; Position in node (in bytes), where the item start.
* \param item Item to be inserted.
*/
template <class TItem>
bool cSequentialArray<TItem>::AddMaxItem(unsigned int& index, unsigned int& position, const TItem & item)
{
	cSequentialArrayNode<TItem>* node;

	node = ReadNodeW(mHeader->GetLastNodeIndex());
	//if (node->TestAddItem(item, mHeader->GetRefBuffer(), mHeader->GetBufferSize()))
	if (node->GetNodeFreeSpace() >= cSequentialArrayNode<TItem>::GetNodeExtraSize() + item.GetMaxSize(GetKeyDescriptor()))
	{
		// in the case that the item fits into the current node
		position = node->GetUsedSpace();
		node->AddMaxItem(item);
		mCache->UnlockW(node);
	}
	else
	{
		// set the reference from the current node to the next node
		if (node->GetNextNodeIndex() != cNode::EMPTY_INDEX)
		{
			unsigned int newIndex = node->GetNextNodeIndex();
			mCache->UnlockW(node);
			node = ReadNodeW(newIndex);
			node->Clear();
		}
		else
		{
			cSequentialArrayNode<TItem>* newNode = ReadNewNode();
			newNode->SetNextNodeIndex(cNode::EMPTY_INDEX);
			node->SetNextNodeIndex(newNode->GetIndex());
			mCache->UnlockW(node);
			node = newNode;
		}

		// in the case that the item does not fit into the last node
		mHeader->IncNodeCount();
		mHeader->SetLastNodeIndex(node->GetIndex());
		position = 0;

		node->AddMaxItem(item);
		mCache->UnlockW(node);
	}
	index = mHeader->GetLastNodeIndex();
	mHeader->IncItemCount();

	return true;
}


/**
* TODO - not finished for variable size items!
* Update item in the middle of the array.
* \param index Node index, where the item was inserted.
* \param position Position in node (in bytes), where the item start.
* \param item Item to be inserted.
*/
template <class TItem>
bool cSequentialArray<TItem>::UpdateItem(unsigned int index, unsigned int position, const TItem & item)
{
	cSequentialArrayNode<TItem>* node;

	node = ReadNodeW(index);
	//if (node->TestAddItem(item, mHeader->GetRefBuffer(), mHeader->GetBufferSize()))
	if (position + item.GetSize(GetKeyDescriptor()) <= node->GetUsedSpace())
	{
		// in the case that the item fits into the current node
		node->RewriteItem(position, item);
		mCache->UnlockW(node);
	} else
	{
		// set the reference from the current node to the next node		
		mCache->UnlockW(node);
		return false;
	}

	return true;
}

/**
* Method update the mItem in the context. It should be used when we
* are updating the context created by the OpenContext method.
* It suppose that the size of encoded item does not change.
* It does not check if it is true, therefore, be carefull with this method otherwise you can rewrite
* following items.
*/
template <class TItem>
bool cSequentialArray<TItem>::UpdateActualItem(cSequentialArrayContext<TItem>* context)
{
	((cSequentialArrayNode<TItem>*)context->GetRefNode())->RewriteItem(context->GetStartPosition(), context->GetItem());
	((cSequentialArrayNode<TItem>*)context->GetRefNode())->SetModified(true);
	return true;
}

/**
* Method update the item FOLLOWING the actual mPosition in the context!
* It suppose that the size of encoded item does not change.
* It does not check if it is true, therefore, be carefull with this method otherwise you can rewrite
* following items.
*/
template <class TItem>
bool cSequentialArray<TItem>::UpdateNextItem(cSequentialArrayContext<TItem>* context)
{
	((cSequentialArrayNode<TItem>*)context->GetRefNode())->RewriteItem(context->GetPosition(), *context->GetItem());
	((cSequentialArrayNode<TItem>*)context->GetRefNode())->SetModified(true);
	return true;
}


/**
* return the last nodeId and the position of the end in the node.
*/
template <class TItem>
void cSequentialArray<TItem>
	::GetArrayEnd(unsigned int &nodeId, unsigned int &position)
{
	assert(mHeader->GetLastNodeIndex() != cObject::UNONDEFINED);

	cSequentialArrayNode<TItem>* node = ReadNodeR(mHeader->GetLastNodeIndex());
	nodeId = mHeader->GetLastNodeIndex();
	position = node->GetUsedSpace();
	mCache->UnlockR(node);
}

/**
* Allows user to shorten existing array. This method must not be used to extend existing array!
* \param nodeId Node id of the last node in the array. Has to be lower then mHeader->GetLastNodeIndex().
* \param position New position within the array.
*/
template <class TItem>
void cSequentialArray<TItem>
	::SetArrayEnd(unsigned int nodeId, unsigned int position)
{
	assert(mHeader->GetLastNodeIndex() != cObject::UNONDEFINED);
	assert(nodeId <= mHeader->GetLastNodeIndex());

	mHeader->SetLastNodeIndex(nodeId);
	cSequentialArrayNode<TItem>* node = ReadNodeR(mHeader->GetLastNodeIndex());
	node->SetUsedSpace(position);
	//node->SetNextNodeIndex(cNode::EMPTY_INDEX);
	mCache->UnlockR(node);
}

/**
* Read new leaf node
*/
template <class TItem>
cSequentialArrayNode<TItem>* cSequentialArray<TItem>::ReadNewNode()
{
	cSequentialArrayNode<TItem>* node = (cSequentialArrayNode<TItem>*)mCache->ReadNew(mNodeHeaderId);
	node->Clear();
	return node;
}

template <class TItem>
cSequentialArrayNode<TItem>* cSequentialArray<TItem>::ReadNodeW(unsigned int index)
{
	return (cSequentialArrayNode<TItem>*)mCache->ReadW(index, mNodeHeaderId);
}

template <class TItem>
cSequentialArrayNode<TItem>* cSequentialArray<TItem>::ReadNodeR(unsigned int index)
{
	return (cSequentialArrayNode<TItem>*)mCache->ReadR(index, mNodeHeaderId);
}

template <class TItem>
void cSequentialArray<TItem>::UnlockR(cSequentialArrayNode<TItem>* node)
{
	mCache->UnlockR(node);
}

template <class TItem>
void cSequentialArray<TItem>::UnlockW(cSequentialArrayNode<TItem>* node)
{
	mCache->UnlockW(node);
}

template <class TItem>
void cSequentialArray<TItem>::UnlockR(cSequentialArrayNode<TItem>** node)
{
	if (*node == NULL)
		return;

	mCache->UnlockR(*node);

	*node = NULL;
}

template <class TItem>
void cSequentialArray<TItem>::UnlockW(cSequentialArrayNode<TItem>** node)
{
	if (*node == NULL)
		return;

	mCache->UnlockW(*node);

	*node = NULL;
}


/**
* Print array content
*/
template <class TItem>
void cSequentialArray<TItem>::Print() const
{
}

/**
* Print data structure statistics
*/
template <class TItem>
void cSequentialArray<TItem>::PrintInfo() const
{
	printf("****************************** Data statistics: *******************************\n");
	printf("Item Count:             %d\n", mHeader->GetItemCount());
//	printf("Block size:             %d\t Block capacity:            %d\n",mHeader->GetNodeSize(),mHeader->GetNodeCapacity());
//	printf("Item size:              %d\t Block dependent data size: %d\n",mHeader->GetItemSize(),mHeader->GetNodeExtraSize());
	printf("Block count:            %d\n", mHeader->GetNodeCount());
	printf("Cache size [nodes]:    %d\n", mCache->GetCacheNodeSize());

}

/**
* Print array content
*/
template <class TItem>
void cSequentialArray<TItem>::PrintCache() const
{
	mCache->Print();
}


/*
* Return the size of the index in bytes.
*/
template <class TItem>
uint cSequentialArray<TItem>::GetIndexSize(uint blockSize)
{
	return mHeader->GetNodeCount() * blockSize;
}

/*
* Return the size of the index in MB.
*/
template <class TItem>
float cSequentialArray<TItem>::GetIndexSizeMB(uint blockSize)
{
	const uint mb = 1024 * 1024;
	return (float) GetIndexSize(blockSize) / mb;
}
}}}
#endif