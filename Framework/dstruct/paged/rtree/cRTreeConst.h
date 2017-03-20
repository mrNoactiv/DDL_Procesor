/**************************************************************************}
{                                                                          }
{    cRTreeConst.h                                 		      						   }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001 - 2003	   			    Michal Kratky                  }
{                                                                          }
{    VERSION: 0.01													DATE 18/11/2003                }
{                                                                          }
{    following functionality:                                              }
{      constants of the R-tree                                             }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cRTreeConst_h__
#define __cRTreeConst_h__

#define __QUERY_WITH_DECOMPRESSION__  // queries with leaf item decomp

#define RTREE_QPROC
class cQueryType
{
public:
	typedef enum QueryType { INDEX_VALIDATION = 0, RANGE_QUERY = 1 };
};
class Find
{
  public: 
	typedef enum FindMBR { MINIMAL_VOLUME = 0, MINIMAL_INTERSECTION = 1, MINMAX_TAXIDIST = 2};
};

class Compression
{
  public: 
	typedef enum Comp { eELIAS_DELTA = 3, eELIAS_FIBONACCI = 11, eFIBONACCI_2 = 14, eFIBONACCI_3 = 15, eFIXED = 19, eRLE = 99};
};

class Reduction
{
  public: 
	typedef enum Reduce { NONE = 0, REF_ITEM = 1, RELATIVE_MBR = 2, RELATIVE_SIZE_MBR = 3};
};

class Split
{
  public: 
	typedef enum Splits { COMPUTE_VOLUME = 0, HILBERT_ORDERING = 1, Z_ORDERING = 2, TAXI_ORDERING = 3, CUT_LONGEST = 4};
};

class cRTreeConst
{
public:
	static const int INSERT_NO = -1;
	static const int INSERT_YES = 1;
	static const int INSERT_DUPLICATE = 0;

	// compression algorithm
	static const int MATRIX_COMPRESSION = 0;
	static const int DIFF_COMPRESSION = 1;
	static const int DIFF_COMPRESSION2 = 2;

	// counter and timer count for query statistics
	static const unsigned int GroupCounterCount = 11;
	static const unsigned int CounterCount = 12;
	static const unsigned int TimerCount = 5;

	static const unsigned int Counter_resultSize = 0;
	static const unsigned int Counter_searchedRegions = 1;
	static const unsigned int Counter_intersection = 2;
	static const unsigned int Counter_relevantRegions = 3;
	static const unsigned int Counter_searchedOverLeafs = 4;
	static const unsigned int Counter_relevantOverLeafs = 5;
	static const unsigned int Counter_sizeLeafSignatures = 6;
	static const unsigned int Counter_sizeOverLeafSignatures = 7;

	static const unsigned int Counter_Prefetch = 8;
	static const unsigned int Counter_Prefetch_Sort = 9;
	static const unsigned int Counter_LeafArraySize = 10;

	static const unsigned int Counter_intersectMBRs = 10;

	static const unsigned int Timer_queryTime = 0;
	static const unsigned int Timer_searchRegions = 1;
	static const unsigned int Timer_intersection = 2;
	static const unsigned int Timer_Prefetch = 3;
	static const unsigned int Timer_Prefetch_Sort = 4;

	static const unsigned int CompressLeaf = 0;
	static const unsigned int CompressInner = 1;
	static const unsigned int DecompressLeaf = 2;
	static const unsigned int DecompressInner = 3;
	static const unsigned int Swap = 4;

	static const unsigned int CompressQlQh = 5;
	static const unsigned int InitRefItem = 6;
	static const unsigned int ComparisonLeaf = 7;
	static const unsigned int ComparisonInner = 8;

	static const unsigned int CompIrrelevant = 9;
	static const unsigned int CompRelevant = 10;

	// **** setting ****
	typedef cUInt tLeafData;
	typedef tLeafData::Type tLeafDataType;

	// **** setting ****
	// split algorithm to be used
	static const int Node_Split =   
		//Split::COMPUTE_VOLUME
		Split::CUT_LONGEST
		;
	static const int Find_MBR = 
		// Find::MINIMAL_INTERSECTION
		// Find::MINIMAL_VOLUME 
		Find::MINMAX_TAXIDIST;
		;

	// Compression Settings
	static const unsigned int COMPRESSION_TYPE = Compression::eELIAS_DELTA;
	static const unsigned int REDUCTION_TYPE = Reduction::REF_ITEM;
	static const unsigned int REF_ITEMS_COUNT = 3;         // count of reference items
	static const unsigned int MAX_DISTANCE = 20000;        // taxi distance between ref. item and item //3435975885/4000;//65535;//131072;//292144;//131072;//292144;//3435975885/4000;
	static const unsigned int REF_TUPLE_LINKS_SIZE = 480;//600;  // for 1 item is necessary 2 bites in BitString -> place for 240 items

	unsigned int static const NODE_CAPACITY = 512;         // maximal number of items in decompressed nodes
	unsigned int static const BLOCK_SIZE = 8192; /* 4096*/; // size of decompressed node

	// Statistical Settings
	unsigned int static const HISTOGRAM_RANGE = 1024;

	// Common Constants
	static const unsigned int BITS_IN_BYTE = 8;

};
#endif