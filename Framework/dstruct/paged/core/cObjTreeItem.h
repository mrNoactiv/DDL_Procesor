/**
*	\file cFCTreeLeafNodeHeader.h
*	\authorMichal Krátký
*	\version 0.1
*	\date nov 2001
*	\brief Object tree item - interface
*/

#ifndef __cObjTreeItem_h__
#define __cObjTreeItem_h__

#include "cStream.h"
#include "cString.h"
#include "cTreeHeader.h"
#include "cSizeInfo.h"

namespace dstruct {
  namespace paged {
	namespace core {

/**
*	Object tree item - interface
*
*	\author Michal Krátký
*	\version 0.1
*	\date nov 2001
**/
template<class TItem>
class cObjTreeItem
{
protected:
	// typedef typename TItem::Type TItem;

public:
	//static const unsigned int STRING_VALUE_SIZE = 30;

	cObjTreeItem()													{ }
	cObjTreeItem(const cTreeHeader *header)							{ UNREFERENCED_PARAMETER(header); }

	inline void Clear()												{ printf("cObjTreeItem (destructor) - Should not be called\n"); }
	void Resize(cTreeHeader *header)								{ UNREFERENCED_PARAMETER(header); printf("cObjTreeItem - Should not be called\n"); }

	inline unsigned int GetSerialSize() const						{ printf("cObjTreeItem::GetSerialSize() - Should not be called\n"); }
	inline static unsigned int GetSerialSize(const cTreeHeader *header)	{ UNREFERENCED_PARAMETER(header); printf("cObjTreeItem::GetSize() - Should not be called\n"); }
	static inline unsigned int GetInMemSize(char* data) { printf("cObjTreeItem::GetStaticInMemSize() - Should not be called\n"); return 0; }
	static inline unsigned int GetStaticSerialSize() { printf("cObjTreeItem::GetStaticSerialSize() - Should not be called\n"); return 0; }

	/// Method compare this tree item with an item in the first parameter
	/// \param item Item with the same type as this class.
	/// \param compareOnlyKey If true only item's keys are compared.
	/// \return
	///		- -1 if this item is lower than the item in the parameter
	///		- 0 if this item and the item in the parameter are the same
	///		- 1 if this item is higher than the item in the parameter
	inline int Equal(const cObjTreeItem &item, bool compareOnlyKey) const
	{ 
		UNREFERENCED_PARAMETER(item); 
		UNREFERENCED_PARAMETER(compareOnlyKey); 
		printf("cObjTreeItem::Equal() - Should not be called\n"); 
		return -2;
	}
	static int Equal(const char* item1, const char* item2);

	/// Copy one item into another	
	void operator = (const cObjTreeItem &item)						{ UNREFERENCED_PARAMETER(item); printf("cObjTreeItem::operator = - Should not be called\n");  };

	inline bool Write(cStream *stream)								{ UNREFERENCED_PARAMETER(stream); printf("cObjTreeItem::Write() - Should not be called\n"); }
	inline bool Read(cStream *stream)								{ UNREFERENCED_PARAMETER(stream); printf("cObjTreeItem::Read() - Should not be called\n"); }
};


/**
* Static method which compare two items
* \param item1 The first item.
* \param item2 The second item.
* \return
*		- -1 if the first item is lower than the second item.
*		- 0 if the items are equal
*		- 1 if the first item is higher than the second item.
*/
template <class TKey>
int cObjTreeItem<TKey>::Equal(const char* item1, const char* item2)
{
	return TKey::Compare(item1, item2);
}

}}}
#endif