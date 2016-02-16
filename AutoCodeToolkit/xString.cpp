#include "stdafx.h"
#include "xString.h"


xString::xString()
{
}


xString::~xString()
{
}

xString::xString(char* initString)
{
	strcpy(lpszString, initString);
}
