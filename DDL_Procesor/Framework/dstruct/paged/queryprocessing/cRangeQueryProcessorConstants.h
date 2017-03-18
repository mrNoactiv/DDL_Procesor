#ifndef __cRangeQueryProcessorConstants_h__
#define __cRangeQueryProcessorConstants_h__

namespace dstruct {
	namespace paged {

class cRangeQueryProcessorConstants
{
public:
	static const unsigned short RQ_BTREE_SEQ = 2;
	static const unsigned short RQ_BTREE_BIN = 3;
	static const unsigned short RQ_BTREE_BIN_LAST_ORDER = 4;
	static const unsigned short RQ_BTREE_L_LAST_BIN = 5;
	static const unsigned short RQ_BTREE_L0_SEQ = 6;
};

}}
#endif