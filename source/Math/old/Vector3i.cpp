//	----==== VECTOR3I.CPP ====----
//
//	Author:			Jeffrey Kiah
//	Date:			7/1/2007
//	Description:	A 3D integer vector class
//
//	------------------------------

#include <sstream>
#include "Vector3i.h"
//#include "Vector4i.h"
#include "FastMath.h"


/*-----------------
---- FUNCTIONS ----
-----------------*/

////////// class Vector3i //////////

// Convert from 4d vector to 3d vector, ignore w component of 4d vector
/*void Vector3i::assign(const Vector4i &p)
{
	x = p.x;
	y = p.y;
	z = p.z;
}*/


std::string Vector3i::toString(void) const
{
	std::ostringstream returnStr;
	returnStr << "(" << x << "," << y << "," << z << ")";
	return returnStr.str();
}


//Vector3i::Vector3i(const Vector4i &p) : x(p.x), y(p.y), z(p.z) {}
