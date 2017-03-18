/***************************************************************************}
{                                                                          }
{    cParser.cpp                                                           }
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
#include "cParser.h"

const char* cParser::DEFAULT_DELIMETERS = " ";

cParser::cParser(void)
{
    size_t s = strlen(DEFAULT_DELIMETERS)+1;
	mDelimeters = new char[s];
	memcpy(mDelimeters, DEFAULT_DELIMETERS, s);

	this->data = new char[def_len];
}

cParser::cParser(char* data)
{
	size_t s = strlen(DEFAULT_DELIMETERS)+1;
	mDelimeters = new char[s];
	memcpy(mDelimeters, DEFAULT_DELIMETERS, s);
	
	s = strlen(data)+1;
	this->data = new char[s];
	memcpy(this->data, data, s);
	parser_ptr = 0;
	delim_buff = NULL;
}

cParser::cParser(char* data, const char** mDelimeters)
{
	/*size_t s = strlen(mDelimeters)+1;
	this->mDelimeters = new char[s];
	memcpy(this->mDelimeters, mDelimeters, s);*/
	mmDelimeters = mDelimeters;
	
	s = strlen(data)+1;
	this->data = new char[s];
	memcpy(this->data, data, s);
	parser_ptr = 0;
	delim_buff = NULL;
}

cParser::~cParser(void)
{
	delete mDelimeters; //error
	if (data != NULL) delete data;
}

void cParser::initParser(char* data)
{
	size_t s = strlen(DEFAULT_DELIMETERS)+1;
	if(this->mDelimeters ==NULL){
		mDelimeters = new char[s];
	}
	memcpy(this->mDelimeters, DEFAULT_DELIMETERS, s);

	s = strlen(data)+1;
	size_t old = strlen(this->data)+1;

	if((s > old) || (this->data == NULL)){
		if (this->data != NULL) 
		{
			delete [] this->data;
		}
		this->data = new char[s];
	}
	memcpy(this->data, data, s);
	parser_ptr = 0;
	delim_buff = NULL;
}

void cParser::setParser(char* data)
{
	size_t old = strlen(this->data)+1;
	size_t s = strlen(data)+1;
	if ((s > old) ||(this->data == NULL)){
		if (this->data != NULL)
		{
			delete [] this->data;
		}
		this->data = new char[s];
	}
	memcpy(this->data, data, s);
	this->data[s];
	parser_ptr = 0;
	delim_buff = NULL;
}

void cParser::initParser(char* data, char* mDelimeters)
{
	size_t old = strlen(this->mDelimeters)+1;
	size_t s = strlen(mDelimeters)+1;

	if ((s > old) || (this->mDelimeters == NULL))
	{
		if (this->mDelimeters != NULL) 
		{
			delete this->mDelimeters;
		}
		this->mDelimeters = new char[s];	
	}
	
	memcpy(this->mDelimeters, mDelimeters, s);

	s = strlen(data)+1;
	old = strlen(this->data)+1;
	if((s > old) || (this->data == NULL))
	{
		if (this->data != NULL)
		{
			delete [] this->data;
		}
		this->data = new char[s];
	}
	memcpy(this->data, data, s);
	parser_ptr = 0;
	delim_buff = NULL;
}

bool cParser::backToken()
{
	data[parser_ptr] = getDelimeter();
	while ((data[parser_ptr] != END_CHAR) || (parser_ptr > 0))
	{
		parser_ptr--;
	}
	if (parser_ptr == 0) 
	{
		return false;
	}
	return true;
}

/*
* -
* character - compared character
* returns - true if character is delimeter, otherwise false
*/
bool cParser::isDelimeter(const char character)
{
		bool result = false;
		int i = 0;
		while ( (mDelimeters[i] != END_CHAR) && (!result) ){
			result = result | (character == mDelimeters[i]);
			i++;
		}
		return result;
}

/*
*returns - true if is any character in parsering buffer, otherwise false
*/
bool cParser::isMoreCharacters()
{
	return (data[parser_ptr] != END_CHAR);
}

/**
*returns pointer to token string, if it's token otherwise (if it's delimeter or end of string) returns NULL;
*/

char* cParser::getToken(){
	
	if (!isMoreCharacters()) //no more tokens, and in case of delimeter (algorithm is) after token
	{
		return NULL; 
	}
	if (delim_buff != NULL){ //delimeter is in buffer
		return NULL;
	}
	char* begin_of_token = data + (int)parser_ptr; //returned address
	while( !isDelimeter(data[parser_ptr]) && isMoreCharacters() )
	{
		parser_ptr++;
	}
	if (isDelimeter(data[parser_ptr])){
		delim_buff = data[parser_ptr]; //buffreing next delimeter 
		data[parser_ptr] = END_CHAR; //end of token
		if (begin_of_token != (data + (int)parser_ptr)) //not only delimeter
		{ 
			return begin_of_token;
		}
		else //mDelimeters without tokens
		{
			return NULL;
		}
	}
	else //END of string (last token)
	{
		return begin_of_token;
	}
}

/**
 *returns delimeter character, if is in buffer otherwise return (cParser::END_CHAR)
*/
char cParser::getDelimeter(){
	char db = delim_buff;
	if (db != NULL) {
		parser_ptr++; //shift to next token
		delim_buff = NULL; //delimeter clear
	}
	return db;
}

/*void main(void)
{
	char* token, delim = NULL;
    cParser p1("ahoj svete jak se mas");

	while ( ((token = p1.getToken()) != NULL) || ((delim = p1.getDelimeter())!= NULL) )
	{
		printf("TOKEN: %s\n", token);
		if (token == NULL) 
		{
			printf("DELIMETER: '%c'\n", delim);
		}
	}
}*/