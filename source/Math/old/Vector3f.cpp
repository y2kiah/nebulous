/* ----==== VECTOR3F.CPP ====---- */

#include <sstream>
#include "TVector3.h"
#include "Vector4f.h"
//#include "matrix4x4.h"
#include "FastMath.h"

///// FUNCTIONS /////

// The following 3 functions use only the 3x3 transformation portion of the matrix
// so the point will not be translated
/*Vector3	Vector3::operator* (const Matrix4x4 &m) const
{
	return Vector3(	x*m.i[0] + y*m.i[4] + z*m.i[8],
					x*m.i[1] + y*m.i[5] + z*m.i[9],
					x*m.i[2] + y*m.i[6] + z*m.i[10]);
}


/*void Vector3::operator*=(const Matrix4x4 &m)
{
	x = x*m.i[0] + y*m.i[4] + z*m.i[8];
	y = x*m.i[1] + y*m.i[5] + z*m.i[9];
	z = x*m.i[2] + y*m.i[6] + z*m.i[10];
}


void Vector3::multiplyMatrix(const Vector3 &p, const Matrix4x4 &m)
{
	x = p.x*m.i[0] + p.y*m.i[4] + p.z*m.i[8];
	y = p.x*m.i[1] + p.y*m.i[5] + p.z*m.i[9];
	z = p.x*m.i[2] + p.y*m.i[6] + p.z*m.i[10];
}
*/

// Convert from 4d vector to 3d vector, ignore w component of 4d vector
void Vector3f::assign(const Vector4f &p)
{
	x = p.x; y = p.y; z = p.z;
}

void Vector3f::unitNormalOf(const Vector3f &p1, const Vector3f &p2)
{
	// Calculates the normal vector with cross product
	x = (p1.y * p2.z) - (p1.z * p2.y);
	y = (p1.z * p2.x) - (p1.x * p2.z);
	z = (p1.x * p2.y) - (p1.y * p2.x);

	// Normalizes the vector
	normalize();
}

void Vector3f::normalize(void)
{
	// Normalize the vector to unit length
	float magSq = this->magSquared();

	if (magSq > 0) {
		float invMag = 1.0f / sqrtf(magSq);
		x *= invMag;
		y *= invMag;
		z *= invMag;
	}
}

void Vector3f::rotate(Vector3f &p, uint xa, uint ya, uint za)
{
	_ASSERTE(xa < math.ANGLE360);
	_ASSERTE(ya < math.ANGLE360);
	_ASSERTE(za < math.ANGLE360);

	*this = p;
	Vector3f tp;
	
	tp.x = x * math.getCos(za) + y * math.getSin(za);
	tp.y = y * math.getCos(za) - x * math.getSin(za);
	x = tp.x;
	y = tp.y;
	
	tp.y = y * math.getCos(xa) + z * math.getSin(xa);
	tp.z = z * math.getCos(xa) - y * math.getSin(xa);
	y = tp.y;
	z = tp.z;

	tp.z = z * math.getCos(ya) + x * math.getSin(ya);
	tp.x = x * math.getCos(ya) - z * math.getSin(ya);
	x = tp.x;
	z = tp.z;
}

void Vector3f::rotate(uint xa, uint ya, uint za)
{
	_ASSERTE(xa < math.ANGLE360);
	_ASSERTE(ya < math.ANGLE360);
	_ASSERTE(za < math.ANGLE360);

	Vector3f tp;
	
	tp.x = x * math.getCos(za) + y * math.getSin(za);
	tp.y = y * math.getCos(za) - x * math.getSin(za);
	x = tp.x;
	y = tp.y;
		
	tp.y = y * math.getCos(xa) + z * math.getSin(xa);
	tp.z = z * math.getCos(xa) - y * math.getSin(xa);
	y = tp.y;
	z = tp.z;

	tp.z = z * math.getCos(ya) + x * math.getSin(ya);
	tp.x = x * math.getCos(ya) - z * math.getSin(ya);
	x = tp.x;
	z = tp.z;
}

std::string Vector3f::toString(void) const
{
	std::ostringstream returnStr;
	returnStr << "(" << x << "," << y << "," << z << ")";
	return returnStr.str();
}

Vector3f::Vector3f(const Vector4f &p) : x(p.x), y(p.y), z(p.z) {}
