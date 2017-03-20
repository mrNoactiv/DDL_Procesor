/**
*	\file cDStructHeader.h
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.2
*	\date jul 2011
*	\brief Header of a paged data structures
*/

#ifndef __cDStructHeader_h__
#define __cDStructHeader_h__

namespace dstruct {
  namespace paged {
	 namespace paged {
class cDStructHeader;
class cNodeHeader;
}}}

class cMemoryManagerCuda;

#include "dstruct/paged/core/cNodeHeader.h"

#include "common/memdatstruct/cMemoryManager.h"
#ifdef CUDA_ENABLED
#include "dstruct/paged/cuda/cMemoryManagerCuda.h"
#endif

#include "common/stream/cStream.h"
#include "common/cMemory.h"
#include "dstruct/paged/core/cDStructConst.h"
#include "dstruct/paged/core/cMemoryPool.h"

using namespace common::memdatstruct;

/**
* Header of a paged data structure. 
* Store just basic informations regarding the data structure.
*
*	\version 0.2
*	\date jul 2011
**/
namespace dstruct {
  namespace paged {
	namespace core {

class cDStructHeader
{
public:
	static const unsigned int TITLE_SIZE = 128;
	static const unsigned int HEADER_PATHSIZE = 64;
	static const unsigned int EXTEND_SIZE = 65536;
	static const unsigned int DEFAULT_BLOCK_SIZE = 8192;

protected:
	// serialized attributes
	char mUniqueName[HEADER_PATHSIZE]; // Unique name of the data structure. Name must be unique among the other data structures using the same cache.
	char mTitle[TITLE_SIZE];		// Title of the data structure (B-tree and so on)
	float mVersion;
	unsigned int mBuild;
	unsigned int mNodeHeaderCount;	// Size of mNodeHeaders array.
	unsigned int* mNodeIds;			// Array of node headers' id.
	cNodeHeader** mNodeHeaders;		// Array of node headers (this array is nt serialized) 
	bool mDuplicates;					// Are duplicate keys valid?
	uint mDStructCode;

	// TODO: remove
	unsigned int mRealHeaderSize;	// Serialized size of the header in bytes. This value represent serialized size of the header on the secondary storage.
	unsigned int mHeaderSize;		// Serialized size of the header in bytes rounded on BLOCK_SIZE multiple; TODO: pokud se bude serializovat v cache, je nutná tato proměnná?

	bool mMeasureTime;
	bool mMeasureCount;
	bool mCacheMeasureTime;
	bool mCacheMeasureCount;

public:
	
	static const unsigned int BLOCK_SIZE = 2048;
	static const unsigned int MAGIC_NUMBER = 0x12340602;

	cDStructHeader();
	cDStructHeader(const cDStructHeader &header);
	~cDStructHeader();

	inline void Null();
	inline virtual void Init();
	inline virtual void Delete();
	inline virtual bool Write(cStream *stream);
	inline virtual bool Read(cStream *stream);

	inline void SetTitle(const char *treeTitle);
	inline void SetVersion(float treeVersion);
	inline void SetBuild(unsigned int treeBuild);
	inline char* GetTitle() const;
	inline float GetVersion() const;
	inline unsigned int GetBuild() const;
	inline void SetDStructMode(unsigned int dsMode);
	inline unsigned int GetDStructMode() const;
	inline unsigned int GetDStructCode() const;
	inline unsigned int GetCodeType() const;
	inline bool DuplicatesAllowed() const;
	inline void DuplicatesAllowed(bool status);

	inline void SetDStructCode(uint dsCode);

	inline void AddHeaderSize(unsigned int userHeaderSerSize);
	inline unsigned int GetSize() const;

	// inline void SetNodeSize(unsigned int nodeSize);
	// inline void SetNodeSizeAsMulPageSize(unsigned int mul);
	inline void SetMeasureTime(bool value);
	inline void SetMeasureCount(bool value);
	inline void SetCacheMeasureTime(bool value);
	inline void SetCacheMeasureCount(bool value);

	// inline unsigned int GetNodeSize() const;
	inline bool GetMeasureTime() const;
	inline bool GetMeasureCount() const;
	inline bool GetCacheMeasureCount() const;
	inline bool GetCacheMeasureTime() const;
	inline virtual unsigned int GetNodeCount() const { return 0; };

	//inline virtual void WriteNode(const cMemoryBlock &node, cStream *stream);
	inline void SetNodeHeaderCount(unsigned int count);
	inline void SetNodeHeader(unsigned int order, cNodeHeader* nodeHeader);
	inline cNodeHeader* SetNodeHeader(unsigned int order) const;
	inline void SetNodeType(unsigned int order, unsigned int id);
	inline unsigned int GetNodeHeaderCount() const;
	inline cNodeHeader** GetNodeHeader() const;
	inline cNodeHeader* GetNodeHeader(unsigned int i) const;
	inline unsigned int GetNodeType(unsigned int order) const;
	inline unsigned int* GetNodeIds() const;
	inline void SetMemoryPool(cMemoryPool* pMemPool);
	inline void SetMemoryManager(cMemoryManager* pMemManager);
#ifdef CUDA_ENABLED
 	inline void SetMemoryManagerCuda(cMemoryManagerCuda* pMemManagerCuda);
#endif
	inline void SetCodeType(unsigned int codeType);

	inline void SetName(const char *uniqueName);
	inline char* GetName() const;

	inline void Print() const;
};

inline void cDStructHeader::Null()
{
	mNodeHeaders = NULL;
	mNodeIds = NULL;
}

inline void cDStructHeader::Init()
{
	memset(mTitle, 0, sizeof(mTitle));
	mVersion = (float)0.2;
	mRealHeaderSize = TITLE_SIZE * sizeof(char) + sizeof(float) + 2 * sizeof(unsigned int);
	mHeaderSize = BLOCK_SIZE;
	mMeasureTime = false;
	mMeasureCount = true;
	mCacheMeasureTime = false;
	mCacheMeasureCount = true;
	mDuplicates = false;

	// mNodeSize = cObject::UNONDEFINED;
	
	//SetNodeHeaderCount(2);		// bed119
}

inline void cDStructHeader::Delete()
{
	if (mNodeHeaders != NULL)
	{
		for (uint i = 0 ; i < mNodeHeaderCount ; i++)
		{
			if (mNodeHeaders[i] != NULL)
			{
				delete mNodeHeaders[i];
				mNodeHeaders[i] = NULL;
			}
		}
		delete[] mNodeHeaders;
		mNodeHeaders = NULL;
	}
	if (mNodeIds != NULL)
	{
		delete[] mNodeIds;
		mNodeIds = NULL;
	}
}

/**
* Extend the serialized size of the header. This method is called by the class inherited from this class, 
* which add some attributes into the header.
*/
inline void cDStructHeader::AddHeaderSize(unsigned int userHeaderSerSize) 
{
	mRealHeaderSize += userHeaderSerSize;
	if (mRealHeaderSize > mHeaderSize)
	{
		unsigned int mul = mRealHeaderSize / BLOCK_SIZE;
		if ((mRealHeaderSize % BLOCK_SIZE) != 0)
		{
			mul++;
		}
		mHeaderSize += mul * BLOCK_SIZE;
	}
}

bool cDStructHeader::Write(cStream *stream)
{
	bool ok;

	ok = stream->Write(mUniqueName, HEADER_PATHSIZE);   // have to be written first since it is expected to be first by the cNodeCache
	ok &= stream->Write(mTitle, sizeof(char)*TITLE_SIZE);
	ok &= stream->Write((char*)&mVersion, sizeof(float));
	ok &= stream->Write((char*)&mBuild, sizeof(unsigned int));
	// ok &= stream->Write((char*)&mNodeSize, sizeof(unsigned int));
	ok &= stream->Write((char*)&mDuplicates, sizeof(bool));
	ok &= stream->Write((char*) &mNodeHeaderCount, sizeof(unsigned int));
	for (unsigned int i = 0; i < mNodeHeaderCount; i++)
	{
		ok &= stream->Write((char*) &mNodeIds[i], sizeof(unsigned int));
		ok &= mNodeHeaders[i]->Write(stream);
	}
	return ok;
}

bool cDStructHeader::Read(cStream *stream) 
{
	bool ok; 

	ok = stream->Read(mUniqueName, HEADER_PATHSIZE);
	ok &= stream->Read(mTitle, sizeof(char)*TITLE_SIZE);
	ok &= stream->Read((char*)&mVersion, sizeof(float));
	ok &= stream->Read((char*)&mBuild, sizeof(unsigned int));
	// ok &= stream->Read((char*)&mNodeSize, sizeof(unsigned int));
	ok &= stream->Read((char*)&mDuplicates, sizeof(bool));
	ok &= stream->Read((char*) &mNodeHeaderCount, sizeof(unsigned int));
	for (unsigned int i = 0; i < mNodeHeaderCount; i++)
	{
		ok &= stream->Read((char*) &mNodeIds[i], sizeof(unsigned int));
		ok &= mNodeHeaders[i]->Read(stream);
	}
	return ok;
}

//********************* SET ********************************

inline void cDStructHeader::SetTitle(const char *treeTitle)
{ 
	strncpy(mTitle, treeTitle, strlen(treeTitle));
	/*uint dsId = cDStructConst::DS_UNKNOWN;

	if (strcmp(mTitle, "B-tree") == 0)
	{
		dsId = cDStructConst::BTREE;
	}
	else if (strcmp(mTitle, "R-tree") == 0)
	{
		dsId = cDStructConst::RTREE;
	}

	for (unsigned int i = 0 ; i < mNodeHeaderCount ; i++)
	{
		mNodeHeaders[i]->SetDStructId(dsId);
	}*/
}

inline void cDStructHeader::SetDStructCode(uint dsCode)
{
	for (unsigned int i = 0; i < mNodeHeaderCount; i++)
	{
		mNodeHeaders[i]->SetDStructCode(dsCode);
	}
}

inline void cDStructHeader::SetVersion(float treeVersion)
{ 
	mVersion = treeVersion; 
}

inline void cDStructHeader::SetBuild(unsigned int treeBuild)
{ 
	mBuild = treeBuild; 
}

inline void cDStructHeader::SetDStructMode(unsigned int dsMode)
{ 
	for (unsigned int i = 0 ; i < mNodeHeaderCount ; i++)
	{
		mNodeHeaders[i]->SetDStructMode(dsMode);
	}
}

inline void cDStructHeader::SetCodeType(unsigned int codeType)
{ 
	for (unsigned int i = 0 ; i < mNodeHeaderCount ; i++)
	{
		mNodeHeaders[i]->SetCodeType(codeType);
	}
}

inline void cDStructHeader::DuplicatesAllowed(bool status)		
{ 
	mDuplicates = status; 
	for (unsigned int i = 0; i < mNodeHeaderCount; i++)
	{
		mNodeHeaders[i]->DuplicatesAllowed(status);
	}
}

inline void cDStructHeader::SetMeasureTime(bool value)
{ 
	mMeasureTime = value; 
}
inline void cDStructHeader::SetMeasureCount(bool value){ 
	mMeasureCount = value; 
}
inline void cDStructHeader::SetCacheMeasureTime(bool value)
{ 
	mCacheMeasureTime = value; 
}
inline void cDStructHeader::SetCacheMeasureCount(bool value){ 
	mCacheMeasureCount = value; 
}

//********************* GET ********************************


inline unsigned int cDStructHeader::GetSize() const 
{ 
	return mHeaderSize; 
}
inline char* cDStructHeader::GetTitle() const
{ 
	return (char *)mTitle; 
}
inline float cDStructHeader::GetVersion() const
{
	return mVersion; 
}
inline unsigned int cDStructHeader::GetBuild() const
{ 
	return mBuild; 
}

inline unsigned int cDStructHeader::GetDStructMode() const
{
	unsigned int dsMode = cDStructConst::DSMODE_DEFAULT;

	if (mNodeHeaders[0] != NULL)
	{
		dsMode = mNodeHeaders[0]->GetDStructMode();
	}
	return dsMode;
}

inline unsigned int cDStructHeader::GetDStructCode() const
{
	unsigned int dsCode = cDStructConst::BTREE;

	if (mNodeHeaders[0] != NULL)
	{
		dsCode = mNodeHeaders[0]->GetDStructCode();
	}
	return dsCode;
}

inline unsigned int cDStructHeader::GetCodeType() const
{
	return mNodeHeaders[0]->GetCodeType();
}

inline bool cDStructHeader::DuplicatesAllowed() const				
{ 
	return mDuplicates; 
}

inline bool cDStructHeader::GetMeasureTime() const
{ 
	return mMeasureTime; 
}
inline bool cDStructHeader::GetMeasureCount() const
{ 
	return mMeasureCount; 
}
inline bool cDStructHeader::GetCacheMeasureTime() const
{ 
	return mCacheMeasureTime; 
}
inline bool cDStructHeader::GetCacheMeasureCount() const
{ 
	return mCacheMeasureCount; 
}

inline void cDStructHeader::SetNodeHeaderCount(unsigned int count)
{
	mNodeHeaderCount = count;
	if (count == 0)
	{
		mNodeHeaders = NULL;
		mNodeIds = NULL;
	}else
	{
		mNodeHeaders = new cNodeHeader*[mNodeHeaderCount];
		mNodeIds = new unsigned int[mNodeHeaderCount];
	}
	for (unsigned int i = 0; i < mNodeHeaderCount; i++)
	{
		mNodeHeaders[i] = NULL;
		mNodeIds[i] = 0;			// bed119
	}
}

inline void cDStructHeader::SetNodeHeader(unsigned int order, cNodeHeader* nodeHeader)
{
	assert(order < mNodeHeaderCount);
	assert(mNodeHeaders[order] == NULL);
	mNodeHeaders[order] = nodeHeader;
}


inline void cDStructHeader::SetNodeType(unsigned int order, unsigned int id)
{
	mNodeIds[order] = id;
}

inline unsigned int cDStructHeader::GetNodeHeaderCount() const
{
	return mNodeHeaderCount;
}

inline cNodeHeader** cDStructHeader::GetNodeHeader() const
{
	return mNodeHeaders;
}

inline cNodeHeader* cDStructHeader::GetNodeHeader(unsigned int order) const
{
	return mNodeHeaders[order];
}

inline void cDStructHeader::SetMemoryPool(cMemoryPool* pMemPool)
{
	for (unsigned int i = 0 ; i < mNodeHeaderCount ; i++)
	{
		mNodeHeaders[i]->SetMemoryPool(pMemPool);
	}
}

inline void cDStructHeader::SetMemoryManager(cMemoryManager* pMemManager)
{
	for (unsigned int i = 0 ; i < mNodeHeaderCount ; i++)
	{
		mNodeHeaders[i]->SetMemoryManager(pMemManager);
	}
}

#ifdef CUDA_ENABLED
inline void cDStructHeader::SetMemoryManagerCuda(cMemoryManagerCuda* pMemManagerCuda)
{
	for (unsigned int i = 0 ; i < mNodeHeaderCount ; i++)
	{
		mNodeHeaders[i]->SetMemoryManagerCuda(pMemManagerCuda);
	}
}
#endif

inline cNodeHeader* cDStructHeader::SetNodeHeader(unsigned int order) const
{
	return mNodeHeaders[order];
}

inline unsigned int* cDStructHeader::GetNodeIds() const
{
	return mNodeIds;
}

inline unsigned int cDStructHeader::GetNodeType(unsigned int order) const
{
	return mNodeIds[order];
}

/**
* Set unique name of the data structure.
* \param uniqueName Unique name of the data structure
*/
inline void cDStructHeader::SetName(const char* uniqueName)
{ 
	assert(strlen(uniqueName) < HEADER_PATHSIZE);
	strcpy(mUniqueName, uniqueName);
}

inline char* cDStructHeader::GetName() const
{ 
	return (char *)mUniqueName;
}

void cDStructHeader::Print() const
{
	printf("***************** DS Header ***********************\n");
	printf("DS name: %s\n", mUniqueName);
	printf("Node header count: %d\n", mNodeHeaderCount);
	//for (unsigned int i = 0; i < mNodeHeaderCount; i++)
	//{
	//	printf("*** %d Node header details ***\n", i);
	//	mNodeHeaders[i]->Print();
	//}
}

}}}
#endif
