//	----==== VECTOR4F.CPP ====----
//
//	Author:			Jeffrey Kiah
//	Date:			7/1/2007
//	Description:	A 4D floating point vector class
//
//	------------------------------

#include <sstream>
#include "Vector4f.h"
#include "TVector3.h"
//#include "Matrix4x4f.h"

///// FUNCTIONS /////

/*Vector4	Vector4::operator* (const Matrix4x4 &m) const
{
	return Vector4(	x*m.i[0] + y*m.i[4] + z*m.i[8]  + w*m.i[12],
					x*m.i[1] + y*m.i[5] + z*m.i[9]  + w*m.i[13],
					x*m.i[2] + y*m.i[6] + z*m.i[10] + w*m.i[14],
					x*m.i[3] + y*m.i[7] + z*m.i[11] + w*m.i[15]);
}


void Vector4::operator*=(const Matrix4x4 &m)
{
	Vector4 orig(*this);
	x = orig.x*m.i[0] + orig.y*m.i[4] + orig.z*m.i[8]  + orig.w*m.i[12];
	y = orig.x*m.i[1] + orig.y*m.i[5] + orig.z*m.i[9]  + orig.w*m.i[13];
	z = orig.x*m.i[2] + orig.y*m.i[6] + orig.z*m.i[10] + orig.w*m.i[14];
	w = orig.x*m.i[3] + orig.y*m.i[7] + orig.z*m.i[11] + orig.w*m.i[15];
}


void Vector4::multiplyMatrix(const Vector4 &p, const Matrix4x4 &m)
{
	x = p.x*m.i[0] + p.y*m.i[4] + p.z*m.i[8]  + p.w*m.i[12];
	y = p.x*m.i[1] + p.y*m.i[5] + p.z*m.i[9]  + p.w*m.i[13];
	z = p.x*m.i[2] + p.y*m.i[6] + p.z*m.i[10] + p.w*m.i[14];
	w = p.x*m.i[3] + p.y*m.i[7] + p.z*m.i[11] + p.w*m.i[15];
}
*/

// Convert from 3d vector to 4d vector, often w = 0 used for vectors and w = 1 used for points
// when multiplying by matrix, w = 1 means it can be translated from origin
void Vector4f::assign(const Vector3f &p, float _w)
{
	x = p.x;
	y = p.y;
	z = p.z;
	w = _w;
}


void Vector4f::normalize(void)
{
	// Normalize the vector to unit length
	float magSq = this->magSquared();

	if (magSq > 0) {
		float invMag = 1.0f / sqrtf(magSq);
		x *= invMag;
		y *= invMag;
		z *= invMag;
		w *= invMag;
	}
}


std::string Vector4f::toString(void) const
{
	std::ostringstream returnStr;
	returnStr << "(" << x << "," << y << "," << z << "," << w << ")";
	return returnStr.str();
}


Vector4f::Vector4f(const Vector3f &p, float _w) : x(p.x), y(p.y), z(p.z), w(_w) {}