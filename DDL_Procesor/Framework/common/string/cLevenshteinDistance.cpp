#include "cLevenshteinDistance.h"

/// Constructor
cLevenshteinDistance::cLevenshteinDistance()
{
	mMatrix = new cArray<int>();
	mMatrix->Resize(200);
}

/// Destructor
cLevenshteinDistance::~cLevenshteinDistance()
{
}

/// Compute Levenshtein distance
int cLevenshteinDistance::LD (char const *s, char const *t, int weightInsert, int weightDelete, int weightReplace)
{
	int n; // length of s
	int m; // length of t
	int i; // iterates through s
	int j; // iterates through t
	char s_i; // ith character of s
	char t_j; // jth character of t
	int cost; // cost
	int cell; // contents of target cell
	int above; // contents of cell immediately above
	int left; // contents of cell immediately to left
	int diag; // contents of cell immediately above and to left
	unsigned int sz; // number of cells in matrix

	// Step 1	

	n = (int)strlen (s);
	m = (int)strlen (t);
	if (n == 0) {
		return m;
	}
	if (m == 0) {
		return n;
	}
	sz = (n+1) * (m+1) * sizeof (int);
	//d = (int *) malloc (sz);
	while (sz >= mMatrix->Size())
	{
		mMatrix->Resize(mMatrix->Size() * 2);
	}

	// Step 2

	for (i = 0; i <= n; i++) {
		PutAt (mMatrix->GetArray(), i, 0, n, i);
	}

	for (j = 0; j <= m; j++) {
		PutAt (mMatrix->GetArray(), 0, j, n, j);
	}

	// Step 3

	for (i = 1; i <= n; i++) {

		s_i = s[i-1];

		// Step 4

		for (j = 1; j <= m; j++) {

			t_j = t[j-1];

			// Step 5

			if (s_i == t_j) {
				cost = 0;
			}
			else {
				cost = weightReplace;
			}

			// Step 6 

			above = GetAt (mMatrix->GetArray(),i-1,j, n);		// deletion
			left = GetAt (mMatrix->GetArray(),i, j-1, n);		// insertion
			diag = GetAt (mMatrix->GetArray(), i-1,j-1, n);
			cell = Minimum (above + weightDelete, left + weightInsert, diag + cost);
			PutAt (mMatrix->GetArray(), i, j, n, cell);
		}
	}

	return GetAt (mMatrix->GetArray(), n, m, n);;
}

/// Compute Levenshtein distance
int cLevenshteinDistance::LD (int const *s, int slength, int const *t, int tlength, int weightInsert, int weightDelete, int weightReplace)
{
	int n; // length of s
	int m; // length of t
	int i; // iterates through s
	int j; // iterates through t
	int s_i; // ith character of s
	int t_j; // jth character of t
	int cost; // cost
	int cell; // contents of target cell
	int above; // contents of cell immediately above
	int left; // contents of cell immediately to left
	int diag; // contents of cell immediately above and to left
	unsigned int sz; // number of cells in matrix

	// Step 1	

	n = slength;
	m = tlength;
	if (n == 0) {
		return m;
	}
	if (m == 0) {
		return n;
	}
	sz = (n+1) * (m+1) * sizeof (int);
	//d = (int *) malloc (sz);
	while (sz >= mMatrix->Size())
	{
		mMatrix->Resize(mMatrix->Size() * 2);
	}

	// Step 2

	for (i = 0; i <= n; i++) {
		PutAt (mMatrix->GetArray(), i, 0, n, i);
	}

	for (j = 0; j <= m; j++) {
		PutAt (mMatrix->GetArray(), 0, j, n, j);
	}

	// Step 3

	for (i = 1; i <= n; i++) {

		s_i = s[i-1];

		// Step 4

		for (j = 1; j <= m; j++) {

			t_j = t[j-1];

			// Step 5

			if (s_i == t_j) {
				cost = 0;
			}
			else {
				cost = weightReplace;
			}

			// Step 6 

			above = GetAt (mMatrix->GetArray(),i-1,j, n);
			left = GetAt (mMatrix->GetArray(),i, j-1, n);
			diag = GetAt (mMatrix->GetArray(), i-1,j-1, n);
			cell = Minimum (above + weightDelete, left + weightInsert, diag + cost);
			PutAt (mMatrix->GetArray(), i, j, n, cell);
		}
	}

	return GetAt (mMatrix->GetArray(), n, m, n);;
}