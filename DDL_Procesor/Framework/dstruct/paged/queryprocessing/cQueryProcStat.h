/**
*	\file cRangeQueryProcessor.h
*	\author Radim Baca
*	\version 0.1
*	\date 2007
*	\brief Implement persistent B-tree
*/

#ifndef __cQueryProcStat_h__
#define __cQueryProcStat_h__

/**
*	Implement persistent B-tree.
* Parameters of the template:
*		- TNodeItem - Class for the inner node. Usually contains only key value. Class has to implement all methods from cObjTreeItem.
*		- TKey - Class for the leaf node. Usually contains key value and leaf value. Class has to implement all methods from cObjTreeItem.
*		- TKey - Type of the key value. The type has to inherit from the cBasicType.
*		- TLeafData - Type of the leaf value. The type has to inherit from the cBasicType.
*
*	\author Radim Baca
*	\version 0.1
*	\date may 2007
**/

#include <assert.h>
#include <stdio.h>

namespace dstruct {
	namespace paged {

class cQueryProcStat
{
private:
	const static unsigned int MAX_HEIGHT = 10;

	unsigned long long mNofQuery;          // the number queries
	unsigned long long mLarIn;             // logical access read inner node
	unsigned long long mLarInLevel[MAX_HEIGHT];    // logical access read inner node for individual levels
	unsigned long long mLarLn;             // logical access read leaf node;
	unsigned long long mComputCompare;     // the number of compare computations, e.g. cTuple::Equal
	unsigned long long mRelevantLn;        // the number of relevant leaf nodes - a leaf node containing at least one item
	unsigned long long mRelevantIn;        // the number of relevant inner nodes - a subtree containing at least one item
	unsigned long long mSiIn;			//Scan invocations for inner nodes. (Bulk Read)
	unsigned long long mSiLn;			//Scan invocations for leaf nodes. (Bulk Read)

	unsigned long long mSigLarInLevel[MAX_HEIGHT];    // logical access read signature chunks for individual levels
	unsigned long long mSigCTLarInLevel[MAX_HEIGHT];    // logical access read signature chunks for individual levels

	// the same but for the current query
	unsigned int mLarInQuery;
	unsigned int mLarInQueryLevel[MAX_HEIGHT];
	unsigned int mLarLnQuery;
	unsigned int mComputCompareQuery;
	unsigned int mRelevantLnQuery;
	unsigned int mRelevantInQuery;
	unsigned int mSiInQuery;
	unsigned int mSiLnQuery;

	unsigned int mSigLarInQueryLevel[MAX_HEIGHT];    // logical access read signature chunks for individual levels in one query
	unsigned int mSigCTLarInQueryLevel[MAX_HEIGHT];  // logical access read conversion table for individual levels in one query

public:
	cQueryProcStat();

	void Reset();
	void ResetQuery();

	inline void IncLarInQuery();
	inline void IncLarInQuery(unsigned int level);
	inline void AddLarInQuery(unsigned int count);
	inline void IncLarLnQuery();
	inline void AddLarLnQuery(unsigned int count);
	inline void IncRelevantLnQuery();
	inline void IncRelevantInQuery();
	inline void SetComputCompareQuery(unsigned int value);
	inline void IncComputCompareQuery(unsigned int value = 1);
	inline void AddQueryProcStat();
	inline void IncSiInQuery();
	inline void IncSiLnQuery();

	inline unsigned long long GetLarN() const;
	inline unsigned long long GetRelevantLn() const;
	inline unsigned long long GetRelevantIn() const;
	inline unsigned int GetLarNQuery() const;
	inline unsigned int GetRelevantLnQuery() const;
	inline unsigned int GetRelevantInQuery() const;
	inline unsigned long long GetSiN() const;
	inline unsigned long long GetSiNQuery() const;
	inline unsigned int GetLarInQuery() const;
	inline unsigned int GetLarLnQuery() const;
	inline unsigned int GetSiInQuery() const;
	inline unsigned int GetSiLnQuery() const;

	inline float GetLarNAvg() const;
	inline float GetLarInAvg() const;
	inline float GetLarLnAvg() const;
	inline float GetRelevantLnAvg() const;
	inline float GetRelevantInAvg() const;
	inline float GetComputCompareAvg() const;
	inline float GetSiNAvg() const;
	inline float GetSiInAvg() const;
	inline float GetSiLnAvg() const;

	inline float GetSigLarAvg(unsigned int level) const;
	inline void IncSigLarInQuery(unsigned int level);
	inline float GetSigCTLarAvg(unsigned int level) const;
	inline void IncSigCTLarInQuery(unsigned int level);

	void Print() const;
	void PrintLAR() const;
	void PrintSigLAR(unsigned int levelCount, bool* levelsEnabled) const;
	void Print2File(char* statFile) const;
};

inline void cQueryProcStat::AddQueryProcStat()
{
	mLarIn += mLarInQuery;
	mLarLn += mLarLnQuery;
	mRelevantLn += mRelevantLnQuery;
	mRelevantIn += mRelevantInQuery;
	mComputCompare += mComputCompareQuery;
	mSiIn += mSiInQuery;
	mSiLn += mSiLnQuery;
	for (unsigned int i = 0 ; i < MAX_HEIGHT ; i++)
	{
		mLarInLevel[i] += mLarInQueryLevel[i];
		mSigLarInLevel[i] += mSigLarInQueryLevel[i];
		mSigCTLarInLevel[i] += mSigCTLarInQueryLevel[i];
	}
	mNofQuery++;
	mComputCompareQuery = 0;
}

inline float cQueryProcStat::GetLarNAvg() const
{
	float value = 0.0;
	if (mNofQuery != 0)
	{
		value = (float)GetLarN() / mNofQuery;
	}
	return value;
}

inline float cQueryProcStat::GetLarInAvg() const
{
	float value = 0.0;
	if (mNofQuery != 0)
	{
		value = (float)mLarIn / mNofQuery;
	}
	return value;
}

inline float cQueryProcStat::GetSigLarAvg(unsigned int level) const
{
	float value = 0.0;
	if (mNofQuery != 0)
	{
		value = (float)mSigLarInLevel[level] / mNofQuery;
	}
	return value;
}

inline float cQueryProcStat::GetSigCTLarAvg(unsigned int level) const
{
	float value = 0.0;
	if (mNofQuery != 0)
	{
		value = (float)mSigCTLarInLevel[level] / mNofQuery;
	}
	return value;
}

inline float cQueryProcStat::GetLarLnAvg() const
{
	float value = 0.0;
	if (mNofQuery != 0)
	{
		value = (float)mLarLn / mNofQuery;
	}
	return value;
}

inline float cQueryProcStat::GetRelevantLnAvg() const
{
	float value = 0.0;
	if (mNofQuery != 0)
	{
		value = (float)mRelevantLn / mNofQuery;
	}
	return value;
}

inline float cQueryProcStat::GetRelevantInAvg() const
{
	float value = 0.0;
	if (mNofQuery != 0)
	{
		value = (float) mRelevantIn / mNofQuery;
	}
	return value;
}

inline float cQueryProcStat::GetComputCompareAvg() const
{
	float value = 0.0;
	if (mNofQuery != 0)
	{
		value = (float)mComputCompare / mNofQuery;
	}
	return value;
}

inline void cQueryProcStat::IncLarInQuery()
{
	mLarInQuery++;
}

inline void cQueryProcStat::IncLarInQuery(unsigned int level)
{
	assert(level < MAX_HEIGHT);
	mLarInQueryLevel[level]++;
	IncLarInQuery();
}

inline void cQueryProcStat::IncSigLarInQuery(unsigned int level)
{
	assert(level < MAX_HEIGHT);
	mSigLarInQueryLevel[level]++;
}

inline void cQueryProcStat::IncSigCTLarInQuery(unsigned int level)
{
	assert(level < MAX_HEIGHT);
	mSigCTLarInQueryLevel[level]++;
}

inline void cQueryProcStat::IncLarLnQuery()
{
	mLarLnQuery++;
}

inline void cQueryProcStat::IncRelevantLnQuery()
{
	mRelevantLnQuery++;
}

inline void cQueryProcStat::IncRelevantInQuery()
{
	mRelevantInQuery++;
}


inline void cQueryProcStat::SetComputCompareQuery(unsigned int value)
{
	mComputCompareQuery = value;
}

inline void cQueryProcStat::IncComputCompareQuery(unsigned int value)
{
	mComputCompareQuery += value;
}


inline unsigned long long cQueryProcStat::GetLarN() const
{
	return mLarIn + mLarLn;
}

inline unsigned long long cQueryProcStat::GetRelevantLn() const
{
	return mRelevantLn;
}

inline unsigned long long cQueryProcStat::GetRelevantIn() const
{
	return mRelevantIn;
}

inline unsigned int cQueryProcStat::GetLarNQuery() const
{
	return mLarInQuery + mLarLnQuery;
}

inline unsigned int cQueryProcStat::GetRelevantLnQuery() const
{
	return mRelevantLnQuery;
}

inline unsigned int cQueryProcStat::GetRelevantInQuery() const
{
	return mRelevantInQuery;
}

inline void cQueryProcStat::IncSiLnQuery()
{
	mSiLnQuery++;
}

inline void cQueryProcStat::IncSiInQuery()
{
	mSiInQuery++;
}

inline unsigned int cQueryProcStat::GetLarInQuery() const
{
	return mLarInQuery;
}

inline unsigned int cQueryProcStat::GetLarLnQuery() const
{
	return mLarLnQuery;
}

inline unsigned int cQueryProcStat::GetSiInQuery() const
{
	return mSiInQuery;
}

inline unsigned int cQueryProcStat::GetSiLnQuery() const
{
	return mSiLnQuery;
}

inline void cQueryProcStat::AddLarInQuery(unsigned int count)
{
	mLarInQuery += count;
}

inline void cQueryProcStat::AddLarLnQuery(unsigned int count)
{
	mLarLnQuery += count;
}

inline unsigned long long cQueryProcStat::GetSiN() const
{
	return mSiIn + mSiLn;
}

inline unsigned long long cQueryProcStat::GetSiNQuery() const
{
	return mSiInQuery + mSiLnQuery;
}

inline float cQueryProcStat::GetSiNAvg() const
{
	float value = 0.0;
	if (mNofQuery != 0)
	{
		value = GetSiN() / (float)mNofQuery;
	}
	return value;
}

inline float cQueryProcStat::GetSiLnAvg() const
{
	float value = 0.0;
	if (mNofQuery != 0)
	{
		value = mSiLn / (float)mNofQuery;
	}
	return value;
}

inline float cQueryProcStat::GetSiInAvg() const
{
	float value = 0.0;
	if (mNofQuery != 0)
	{
		value = mSiIn / (float)mNofQuery;
	}
	return value;
}
}}
#endif;