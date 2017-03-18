/***************************************************************************}
{                                                                          }
{    cParser.h                                                          }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001 - 2003               Marek Andrt                   }
{                                                                          }
{    VERSION: 0.01                           DATE 01/11/2004               }
{                                                                          }
{    following functionality:                                              }
{       prase tokens from string                                          }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/
#ifndef __cParser_h__
#define __cParser_h__

#include <string>

class cParser
{
	const static char** DEFAULT_DELIMETERS;

	char* data;			// parsing buffer
	char** mDelimeters;	// delimeters for parsing
	int parser_ptr;		// pointer to buffer
	char delim_buff;	// buffer for delimeter used by getToken() & nextDelimeter()
	
	inline bool isDelimeter(const char character);
	inline bool isMoreCharacters();

protected:
	bool backToken();

public:
	const static char END_CHAR = '\0';
	const static size_t def_len = 1024;

	cParser(void);
	cParser(char* data);
	cParser(char* data, const char* delimeters);
	~cParser(void);

	void initParser(char* data);
	void initParser(char* data, char* delimeters);
	void setParser(char* data);

	char* getToken();
	char getDelimeter();

};
#endif