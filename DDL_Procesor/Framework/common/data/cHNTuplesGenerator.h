#ifndef __cHNTuplesGenerator_h__
#define __cHNTuplesGenerator_h__

#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cNSpaceDescriptor.h"
#include "common/datatype/tuple/cHNTuple.h"

#include "common/cNumber.h"

using namespace common::datatype::tuple;

typedef cHNTuple tKey;
typedef cNTuple tKeyPom;
typedef cUInt tDomain;
typedef cUInt tLeafData;

namespace common {
	namespace data {

class cHNTuplesGenerator
{
private:
	unsigned int atoui ( char* a );

	char ax[2048];

protected:
    unsigned int mDimension; 
    unsigned int mLowBound;  // low bound of domain
    unsigned int mHighBound; // high bound of domain
    unsigned int mTuplesCount; 
    char* mFileName;

	int mStatus;
	bool mIsOpen;
	bool mReadOnly;
	cStream* mStream;
	cStream* mStreamQ;
	FILE* mTextFile;

	cHNTuple *mTuple;
	cNTuple *datapom;							// vnitrni tuple

	cSpaceDescriptor* mSpaceDescriptor; 		// Descriptor pro HLAVNI TUPLE
	cSpaceDescriptor* vsd;						// Descriptor pro vnitrni TUPLE (2.souradnice)
public:
	static const int TREE_FILE_OPEN = 1;	
	static const int TREE_FILE_NOT_OPEN = 2;	

    cHNTuplesGenerator();
    cHNTuplesGenerator(char* paFileName, cSpaceDescriptor* paSpaceDescriptor);
    ~cHNTuplesGenerator();

    inline void SetDimension(unsigned int paDimension);
    inline unsigned int GetDimension() const;
    inline void SetLowBound(unsigned int paLowBound);
    inline unsigned int GetLowBound() const;
    inline void SetHighBound(unsigned int paHighBound);
    inline unsigned int GetHighBound() const;
    inline void SetTuplesCount(unsigned int paTuplesCount);
    inline unsigned int GetTuplesCount() const;
    inline void SetFileName(char* paFileName);
    inline char* GetFileName() const;

	unsigned int** CreateTuples(cSpaceDescriptor *spaceDescriptor, unsigned int QueriesCount, bool doTextFile);
	void CreateTuplesAndQueries(cSpaceDescriptor *spaceDescriptor,unsigned int queriesCount, bool doTextFile);

	bool FileOpen();
	void FileClose();
	cHNTuple* GetNextTuple();

	bool TextFileOpen();
	void TextFileClose();
	cHNTuple* GetNextTupleFromTextFile();
	cHNTuple* GetNextTupleFromTextFileXML();
	
	static void SplitXMLFile(unsigned int pdimenze);

	inline cSpaceDescriptor* GetSpaceDescriptor() const;

};

inline void cHNTuplesGenerator::SetDimension(unsigned int paDimension)
{
	mDimension = paDimension;
}

inline unsigned int cHNTuplesGenerator::GetDimension() const
{
	return mDimension;
}

inline void cHNTuplesGenerator::SetLowBound(unsigned int paLowBound)
{
	mLowBound = paLowBound;
}

inline unsigned int cHNTuplesGenerator::GetLowBound() const
{
	return mLowBound;
}

inline void cHNTuplesGenerator::SetHighBound(unsigned int paHighBound)
{
	mHighBound = paHighBound;
}

inline unsigned int cHNTuplesGenerator::GetHighBound() const
{
	return mHighBound;
}

inline void cHNTuplesGenerator::SetTuplesCount(unsigned int paTuplesCount)
{
	mTuplesCount = paTuplesCount;
}

inline unsigned int cHNTuplesGenerator::GetTuplesCount() const
{
	return mTuplesCount;
}

inline void cHNTuplesGenerator::SetFileName(char* paFileName)
{
	mFileName = paFileName;
}

inline char* cHNTuplesGenerator::GetFileName() const
{
	return mFileName;
}

inline cSpaceDescriptor* cHNTuplesGenerator::GetSpaceDescriptor() const
{	
	return mSpaceDescriptor;
}

}}
#endif