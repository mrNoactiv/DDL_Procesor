#include "cCommandLineUtil.h"

cCommandLineUtil::cCommandLineUtil(int argc, char*** argv)
{
	mArgc = argc;
	mArgv = argv;
}

bool cCommandLineUtil::GetParameter(char* pref, char *parameter)
{
	bool ret = false;
	for (int i = 1; i < mArgc ; i++)
	{
		int len = strlen(pref);
		if (strncmp(pref, (*mArgv)[i], len) == 0)
		{
			strcpy(parameter, (*mArgv)[i] + len);
			ret = true;
			break;
		}
	}
	return ret;
}
