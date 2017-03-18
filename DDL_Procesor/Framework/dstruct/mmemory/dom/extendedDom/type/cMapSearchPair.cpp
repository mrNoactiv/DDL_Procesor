#include "cMapSearchPair.h"

/// Constructor
cMapSearchPair::cMapSearchPair(): mNext(NULL)
{
}

/// Destructor
cMapSearchPair::~cMapSearchPair()
{
}

/// Print
void cMapSearchPair::Print(char *string) const
{
	printf("%d -> %d ", mLeft, mRight);
	if (GetIsRightOptional())
	{
		printf("@");
	}
	printf("%s", string);
}

