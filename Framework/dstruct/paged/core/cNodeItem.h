/**
*	\file cNodeHeader.h
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.2
*	\date jul 2011
*	\brief A common item of the paged node
*/

#ifndef __cNodeItem_h__
#define __cNodeItem_h__

#include "cNodeHeader.h"

namespace dstruct {
  namespace paged {
	namespace core {

/**
*	A common item of the paged node
*
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.2
*	\date jul 2011
**/
class cNodeItem
{
public:
	static inline void Copy(char* dst, const char* src, const cNodeHeader *header);
	static inline void CopyData(char* dst, const char* src, const cTreeNodeHeader* header);
	static inline void Copy(char* dst, const char* src, const unsigned int size);
	static inline char* GetItem(const char* item, unsigned int offset);
	static inline const char* GetConstItem(const char* item, unsigned int offset);

	static inline int GetItemInt(const char* item, unsigned int keySize, unsigned int order);
	static inline int GetDataInt(const char* data, unsigned int order);
	static inline void SetDataInt(const char* data, unsigned int order, int value);
};


inline void cNodeItem::Copy(char* dst, const char* src, const cNodeHeader *header)
{
	memcpy(dst, (char*)src, header->GetItemSize());
}

inline void cNodeItem::CopyData(char* dst, const char* src, const cTreeNodeHeader* header)
{
	memcpy(dst, (char*)src, header->GetDataSize());
}

inline void cNodeItem::Copy(char* dst, const char* src, const unsigned int size)
{
	memcpy(dst, (char*)src, size);
}

inline const char* cNodeItem::GetConstItem(const char* item, unsigned int offset)
{
	return (char*)(item + offset);
}

inline char* cNodeItem::GetItem(const char* item, unsigned int offset)
{
	return (char*)(item + offset);
}

/**
 * Return order-th int of the item after the key.
 */
inline int cNodeItem::GetItemInt(const char* item, unsigned int keySize, unsigned int order)
{
	return *(((int*)(item + keySize)) + order);
}

/**
 * Return order-th int of the data.
 */
inline int cNodeItem::GetDataInt(const char* data, unsigned int order)
{
	return *(((int*)data) + order);
}

/**
 * Set order-th int of the data.
 */
inline void cNodeItem::SetDataInt(const char* data, unsigned int order, int value)
{
	*(((int*)data) + order) = value;
}
}}}
#endif