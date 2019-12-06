//	----==== VECTOR2F.CPP ====----
//
//	Author:			Jeffrey Kiah
//	Date:			5/04
//	Description:	A 2D single floating point vector class
//
//	------------------------------


#include <sstream>
#include "Vector2f.h"

/*-----------------
---- FUNCTIONS ----
-----------------*/

////////// class Vector2f //////////


void Vector2f::normalize(void)
{
	// Normalize the vector to unit length
	float magSq = this->magSquared();

	if (magSq > 0) {
		float invMag = 1.0f / sqrtf(magSq);
		x *= invMag;
		z *= invMag;
	}
}


std::string Vector2f::toString(void) const
{
	std::ostringstream returnStr;
	returnStr << "(" << x << "," << z << ")";
	return returnStr.str();
}