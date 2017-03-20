/**
*	\file cXDGKey.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
*	\brief Represents range keying scheme
*/

#ifndef __cXDGKey_h__
#define __cXDGKey_h__

#include "cStream.h"
#include "cTreeSpaceDescriptor.h"
#include "dstruct/mmemory/dom/extendedDom/type/cXDGKeySpaceDescriptor.h"
#include "cTreeTuple.h"
#include "cXPathLex.h"

/**
*	Represents key in the extended DataGuide. Put a semantic to a vector with two items.
*
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
**/
class cXDGKey: public cTreeTuple
{
	static const unsigned int KEY = 0;
	static const unsigned int ORDER = 1;

	static const unsigned int NEXT_MASK = 0x80000000;
public:

	cXDGKey();
	cXDGKey(cTreeSpaceDescriptor *mTreeSpaceDescr);
	
	inline unsigned int GetKey() const			{ return GetUInt(KEY); }
	inline unsigned int GetOrder() const		{ return GetUInt(ORDER) & (~NEXT_MASK); }
	inline bool HasNext() const					{ return (GetUInt(ORDER) & NEXT_MASK) != 0; }

	inline void SetKey(unsigned int key)		{ SetValue(KEY, key); }
	inline void SetOrder(unsigned int order);
	inline void HasNext(bool next);


	inline cXDGKeySpaceDescriptor *GetRangeLabelingSpaceDescriptor() const;

	inline int Equal(const cXDGKey& key) const;

	inline void operator = (const cTreeTuple &tuple);
	inline void operator = (const cXDGKey &key);
	inline bool operator > (const cXDGKey &key) const;
	inline bool operator < (const cXDGKey &key) const;
	inline bool operator == (const cXDGKey &key) const;
	inline bool operator != (const cXDGKey &key) const;

	void Print(const char *string) const;
};

cXDGKeySpaceDescriptor *cXDGKey::GetRangeLabelingSpaceDescriptor() const
{
	return (cXDGKeySpaceDescriptor *)mTreeSpaceDescriptor;
}

/// Set the order of value in this key.
/// \param order New order value
void cXDGKey::SetOrder(unsigned int order)	
{ 
	if (HasNext())
	{
		SetValue(ORDER, order | NEXT_MASK); 
	} else
	{
		SetValue(ORDER, order); 
	}
}

/// \return
///		- true if the key has the sibling with the same key value,
///		- false otherwise.
void cXDGKey::HasNext(bool next)
{
	if (next)
	{
		SetValue(ORDER, GetUInt(ORDER) | NEXT_MASK);
	} else
	{
		SetValue(ORDER, GetUInt(ORDER) & ~NEXT_MASK);
	}
}

/// Compare keys. Compare key and order values
/// \param key XDGKey
/// \return
///		- -1 if the this key is smaller than the key in the parameter
///		- 0 if the keys are the same
///		- 1 if the parameter key is bigger than this key
int cXDGKey::Equal(const cXDGKey& key) const
{
	assert(GetTreeSpaceDescriptor());
	assert(key.GetTreeSpaceDescriptor());

	if (GetKey() == key.GetKey())
	{
		if (GetOrder() < key.GetOrder())
		{
			return -1;
		} else if (GetOrder() == key.GetOrder())
		{
			return 0;
		} else
		{
			return 1;
		}
	} else
	{
		if (GetKey() < key.GetKey())
		{
			return -1;
		} else
		{
			return 1;
		}
	}
}

void cXDGKey::operator = (const cTreeTuple &tuple)
{
	assert(mTreeSpaceDescriptor->GetDimension() == tuple.GetTreeSpaceDescriptor()->GetDimension());

	CopyMemory(mData, tuple.GetData(), mTreeSpaceDescriptor->GetByteSize());
}

void cXDGKey::operator = (const cXDGKey &key)
{
	CopyMemory(mData, key.GetData(), mTreeSpaceDescriptor->GetByteSize());
}

/// Compare values in this key and another key. Compare also Order and ChildCount
/// \return true if this key is greater then the key in argument
bool cXDGKey::operator > (const cXDGKey &key) const
{
	assert(GetTreeSpaceDescriptor());
	assert(key.GetTreeSpaceDescriptor());
	assert(key.GetTreeSpaceDescriptor()->GetByteSize() == GetTreeSpaceDescriptor()->GetByteSize());

	if (GetKey() == key.GetKey())
	{
		if (GetOrder() < key.GetOrder())
		{
			return false;
		} else 
		{
			return true;
		}
	} else
	{
		if (GetKey() < key.GetKey())
		{
			return false;
		} else
		{
			return true;
		}
	}
}

/// Compare values in this key and another key
/// \return true if the second key is greater then this key in all dimension
inline bool cXDGKey::operator < (const cXDGKey &key) const
{
	assert(GetTreeSpaceDescriptor());
	assert(key.GetTreeSpaceDescriptor());
	assert(key.GetTreeSpaceDescriptor()->GetByteSize() == GetTreeSpaceDescriptor()->GetByteSize());

	if (GetKey() == key.GetKey())
	{
		if (GetOrder() > key.GetOrder())
		{
			return false;
		} else 
		{
			return true;
		}
	} else
	{
		if (GetKey() > key.GetKey())
		{
			return false;
		} else
		{
			return true;
		}
	}
}

/// Compare values in this key and another key
/// \return true if the this key is the same as the key in the parameter.
inline bool cXDGKey::operator == (const cXDGKey &key) const
{
	assert(GetTreeSpaceDescriptor());
	assert(key.GetTreeSpaceDescriptor());
	assert(key.GetTreeSpaceDescriptor()->GetByteSize() == GetTreeSpaceDescriptor()->GetByteSize());

	if (GetKey() == key.GetKey())
	{
		return GetOrder() == key.GetOrder();
	} else
	{
		return false;
	}

}

/// Compare values in this key and another key
/// \return false if the this key is not the same as the key in the parameter.
inline bool cXDGKey::operator != (const cXDGKey &key) const
{
	assert(GetTreeSpaceDescriptor());
	assert(key.GetTreeSpaceDescriptor());
	assert(key.GetTreeSpaceDescriptor()->GetByteSize() == GetTreeSpaceDescriptor()->GetByteSize());

	if (GetKey() == key.GetKey())
	{
		return GetOrder() != key.GetOrder();
	} else
	{
		return true;
	}

}

#endif