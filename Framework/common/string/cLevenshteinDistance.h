/**
*	\file cLevenshteinDistance.h
*	\author Michael Gilleland
*	\brief Compute Levenshtein distance between two strings
*/


#ifndef __cClass_h__
#define __cClass_h__

#include "cStream.h"
#include "cArray.h"
#include <string.h>
#include <malloc.h>

/**
*	Compute Levenshtein distance between two strings 
*
*	\author M.Gilleland, R.Baca
**/
class cLevenshteinDistance
{
	cArray<int> *mMatrix;
	
	inline int Minimum (int a, int b, int c);
	inline int *GetCellPointer (int *pOrigin, int col, int row, int nCols);
	inline int GetAt (int *pOrigin, int col, int row, int nCols);
	inline void PutAt (int *pOrigin, int col, int row, int nCols, int x);

public:

	cLevenshteinDistance::cLevenshteinDistance();
	cLevenshteinDistance::~cLevenshteinDistance();

	int LD (char const *s, char const *t, int weightInsert = 1, int weightDelete = 1, int weightReplace = 1);
	int LD (int const *s, int slength, int const *t, int tlength, int weightInsert = 1, int weightDelete = 1, int weightReplace = 1);
}; 

/// Get minimum of three values
int cLevenshteinDistance::Minimum (int a, int b, int c)
{
	int mi;

	mi = a;
	if (b < mi) {
		mi = b;
	}
	if (c < mi) {
		mi = c;
	}
	return mi;

}

/// Get a pointer to the specified cell of the matrix
int *cLevenshteinDistance::GetCellPointer (int *pOrigin, int col, int row, int nCols)
{
	return (pOrigin + col + (row * (nCols + 1)));
}

/// Get the contents of the specified cell in the matrix 
int cLevenshteinDistance::GetAt (int *pOrigin, int col, int row, int nCols)
{
	return *GetCellPointer (pOrigin, col, row, nCols);
}

/// Fill the specified cell in the matrix with the value x
void cLevenshteinDistance::PutAt (int *pOrigin, int col, int row, int nCols, int x)
{
	(*GetCellPointer (pOrigin, col, row, nCols) = x);
}

#endif