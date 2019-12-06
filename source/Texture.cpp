/*----==== TEXTURE.CPP ====----
	Author: Jeff Kiah
	Date:	4/19/2007
-----------------------------*/

#include <sstream>
#include "Texture.h"


/*-----------------
---- FUNCTIONS ----
-----------------*/


////////// class TextureBase //////////

std::wstring TextureBase::toString(void) const
{
	std::wostringstream returnStr;
	returnStr << L"\"" << name << L"\": " << width << L"x" << height;

	return returnStr.str();
}

TextureBase::TextureBase() : name(L""), initialized(0), width(0), height(0)
{
}
