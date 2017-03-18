/**************************************************************************}
{                                                                          }
{    constants.h                                   		      						   }
{                                                                          }
{    following functionality:                                              }
{      common used constants                                               }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __qdb_constants_h__
#define __qdb_constants_h__

/////// NODE_NOT_EXIST
#ifndef C_EMPTY_LINK
#define C_EMPTY_LINK (unsigned int)-1	/* No real node attached yet */
#endif

/////// return values of Insert()
#ifndef C_INSERT_YES 
#define C_INSERT_YES 0
#define C_INSERT_NO -1
#define C_INSERT_DUPLICATE -2
#endif

#ifndef C_INSERT_AT_THE_END 
#define C_INSERT_AT_THE_END 1
#endif

#ifndef C_INSERT_NOSPACE 
#define C_INSERT_NOSPACE INT_MIN + 1
#endif

#ifndef C_INSERT_EXIST 
#define C_INSERT_EXIST -2
#define C_INSERT_EXIST2 INT_MIN + 2
#endif

#ifndef C_SUBNODE_EXIST 
#define C_SUBNODE_EXIST INT_MIN + 3
#endif

#ifndef C_SUBNODE_NOTEXIST 
#define C_SUBNODE_NOTEXIST INT_MIN + 4
#endif

/////// parameter value for FindOrder() method
#ifndef C_FIND_E 
#define C_FIND_E 0				/* equal */
#endif

#ifndef C_FIND_SBE 
#define C_FIND_SBE 1			/* smalest bigger or equal */
#endif

#ifndef C_FIND_INSERT 
#define C_FIND_INSERT 2			/* very similar to SBE, but return FIND_EQUAL if the item already exists */
#endif

/////// return values (used by FindOrder)
#ifndef C_FIND_NOTEXIST 
#define C_FIND_NOTEXIST -1		/* no item found */
#endif

#ifndef C_FIND_EQUAL 
#define C_FIND_EQUAL -2			/* equal item exists */
#endif

/////// return values (used by SplitNode)
#ifndef C_SPLIT_FAILED
#define C_SPLIT_FAILED -1		/* Split not possible - do chaining */
#endif

#ifndef C_SPLIT_NO_CHANGE
#define C_SPLIT_NO_CHANGE 0	/* Split done, but all records remain in old node */
#endif

#ifndef C_SPLIT_OK
#define C_SPLIT_OK 1		/* Split done successfully */
#endif

#ifndef C_UNUSED_NODES_INITIAL_SIZE
#define C_UNUSED_NODES_INITIAL_SIZE 16
#endif

#ifndef C_COMPARE_EQUAL 
#define C_COMPARE_EQUAL 0
#endif

#ifndef C_COMPARE_BIGGER 
#define C_COMPARE_BIGGER 1
#endif

#ifndef C_COMPARE_SMALLER
#define C_COMPARE_SMALLER -1
#endif

#endif