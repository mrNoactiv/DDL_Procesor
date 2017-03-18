/**
 *	\file cDTDescriptor.h
 *	\author Michal Kratky
 *	\version 0.1
 *	\date jun 2011
 *	\brief DataType Descriptor, the root of similar classes.
 */

#ifndef __cDTDescriptor_h__
#define __cDTDescriptor_h__

/**
*	DataType Descriptor, the root of similar classes.
*
*	\author Michal Kratky
*	\version 0.1
*	\date jun 2011
**/
namespace common {
	namespace datatype {

class cDTDescriptor
{
protected:
	unsigned int mDimension;

public:	
	cDTDescriptor(void);
	~cDTDescriptor(void);
	inline unsigned int GetDimension() const;
};
	
inline unsigned int cDTDescriptor::GetDimension() const
{
	return mDimension;
}
}}
#endif