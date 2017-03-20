/**
 *	\file cCommandLineUtil.h
 *	\author Michal Kratky, Radim Baca
 *	\version 0.1
 *	\date jun 2006
 *	\brief The B-tree based index of ngrams
 */

#pragma once

#include <string>

/**
* The B-tree based index of ngrams.
*
* \author Michal Kratky, Radim Baca
* \version 0.1
* \date jun 2006
**/
class cCommandLineUtil
{
	int mArgc;
	char*** mArgv;

public:
	cCommandLineUtil(int argc, char*** argv);

	bool GetParameter(char* pref, char *parameter);
};