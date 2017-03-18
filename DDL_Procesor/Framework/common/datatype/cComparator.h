/**
 *	\file cComparator.h
 *	\author Michal Kratky
 *	\version 0.1
 *	\date jan 2015
 *	\brief A comparator of arrays.
 */

#ifndef __cComparator_h__
#define __cComparator_h__

/**
* A comparator of arrays.
*
*
* \author Michal Kratky
* \version 0.1
* \date jan 2015
**/

namespace common {
	namespace datatype {

template <class T>
class cComparator
{
public:
	static unsigned int CountCompare;
	static inline int Compare(const T* t1, const T* t2, uint length);
};

template <class T> uint cComparator<T>::CountCompare = 0;

template <class T>
inline int cComparator<T>::Compare(const T* t1, const T* t2, uint length)
{
	int ret = 0;
	T* pt1 = (T*)t1;
	T* pt2 = (T*)t2;

	for (uint i = 0 ; i < length ; i++)
	{
		T v1 = *pt1;
		T v2 = *pt2;

		//val644 - start - increment countCompare
		//cTuple::countCompare++;
		//cTuple::countCompareLevel[cTuple::levelTree]++;
		//val644 - end - increment countCompare
		
		cComparator<T>::CountCompare++;

		if (v1 < v2)
		{
			ret = -1;
			break;
		}
		else if (v1 > v2)
		{
			ret = 1;
			break;
		}
		pt1++;
		pt2++;
	}
	return ret;
}
}}
#endif