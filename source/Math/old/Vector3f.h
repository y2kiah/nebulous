/*----==== VECTOR3F.H ====----
	Author:	Jeffrey Kiah
	Date:	5/2004
----------------------------*/

#pragma once

#include <string>
#include <cmath>
#include "FastMath.h"
#include "../Utility Code/Typedefs.h"

///// STRUCTURES /////

class Vector4f;
class Matrix4x4f;

class Vector3f {
	public:
		///// VARIABLES /////
		
		union {
			struct {
				float x, y, z;		// This union allows access to the same memory location through either
			};						// x,y,z or the array v[3]. Use v for fast OpenGL functions
			float v[3];
		};

		///// OPERATORS /////

		// Conditional operators
		bool		operator==(const Vector3f &p) const { return (x == p.x && y == p.y && z == p.z); }
		bool		operator!=(const Vector3f &p) const { return (x != p.x || y != p.y || z != p.z); }

		// Assignment operators
		void		operator= (const Vector3f &p) { x = p.x; y = p.y; z = p.z; }
		void		operator+=(const Vector3f &p) { x += p.x; y += p.y; z += p.z; }
		void		operator-=(const Vector3f &p) { x -= p.x; y -= p.y; z -= p.z; }
		void		operator*=(const Vector3f &p) { x *= p.x; y *= p.y; z *= p.z; }
		void		operator/=(const Vector3f &p) { _ASSERTE(p.x!=0.0f&&p.y!=0.0f&&p.z!=0.0f); x /= p.x; y /= p.y; z /= p.z; }
		void		operator+=(float s) { x += s; y += s; z += s; }
		void		operator-=(float s) { x -= s; y -= s; z -= s; }
		void		operator*=(float s) { x *= s; y *= s; z *= s; }
		void		operator/=(float s) { _ASSERTE(s!=0.0f); s = 1.0f / s; x *= s; y *= s; z *= s; }
		void		operator*=(const Matrix4x4f &m);

		// Not as fast as functions below, but these allow streamed operations ie. V = VA + VB - VC
		Vector3f	operator- (void) const { return Vector3f(-x,-y,-z); } // unary
		// Scalar operations
		Vector3f	operator+ (float s) const { return Vector3f(x+s, y+s, z+s); }
		Vector3f	operator- (float s) const { return Vector3f(x-s, y-s, z-s); }
		Vector3f	operator* (float s) const {	return Vector3f(x*s, y*s, z*s); }
		Vector3f	operator/ (float s) const { _ASSERTE(s!=0.0f); s = 1.0f / s; return Vector3f(x*s, y*s, z*s); }
		Vector3f	operator* (const Matrix4x4f &m) const;	// Use 3x3 portion of matrix
		// Vector operations
		Vector3f	operator+ (const Vector3f &p) const { return Vector3f(x+p.x, y+p.y, z+p.z); }
		Vector3f	operator- (const Vector3f &p) const { return Vector3f(x-p.x, y-p.y, z-p.z); }
		Vector3f	operator* (const Vector3f &p) const { return Vector3f(x*p.x, y*p.y, z*p.z); }
		Vector3f	operator/ (const Vector3f &p) const { _ASSERTE(p.x!=0.0f&&p.y!=0.0f&&p.z!=0.0f); return Vector3f(x/p.x, y/p.y, z/p.z); }
		// Dot product
		float		operator^ (const Vector3f &p) const { return (x * p.x) + (y * p.y) + (z * p.z); }
		// Cross Product
		Vector3f	operator% (const Vector3f &p) const { return Vector3f((y*p.z)-(z*p.y),(z*p.x)-(x*p.z),(x*p.y)-(y*p.x)); }

		///// FUNCTIONS /////

		// Convert from 4d vector to 3d vector, ignore w component of 4d vector
		void			assign(const Vector4f &p);
		void			assign(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }

		// Check equality with floating point error, SLOW
		inline bool		equalTo(const Vector3f &p) const;
		inline bool		notEqualTo(const Vector3f &p) const;

		// Faster than the overloaded operators above
		inline void		add(const Vector3f &p1, const Vector3f &p2);
		inline void		subtract(const Vector3f &p1, const Vector3f &p2);
		inline void		multiply(const Vector3f &p1, const Vector3f &p2);
		inline void		divide(const Vector3f &p1, const Vector3f &p2);
		inline void		add(const Vector3f &p, float s);
		inline void		subtract(const Vector3f &p, float s);
		inline void		multiply(const Vector3f &p, float s);
		inline void		divide(const Vector3f &p, float s);
		inline void		crossProduct(const Vector3f &p1, const Vector3f &p2);
		
		void			multiplyMatrix(const Vector3f &p, const Matrix4x4f &m);

		// Distance
		inline float	distance(const Vector3f &p) const;
		inline float	distSquared(const Vector3f &p) const;

		// Finds the magnitude of the vector, or it's distance from the origin
		float			magnitude(void) const { return sqrtf(x*x + y*y + z*z); }
		float			magSquared(void) const { return x*x + y*y + z*z; }

		// Vector methods
		inline void		normalOf(const Vector3f &p1, const Vector3f &p2);
		void			unitNormalOf(const Vector3f &p1, const Vector3f &p2);
		void			normalize(void);
		
		// 3D Rotation functions
		void			rotate(Vector3f &p, uint xa, uint ya, uint za);
		void			rotate(uint xa, uint ya, uint za);

		// Debugging
		std::string		toString(void) const;

		// Constructors / Destructor
		Vector3f() : x(0), y(0), z(0) {}
		Vector3f(const Vector3f &p) : x(p.x), y(p.y), z(p.z) {}
		explicit Vector3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
		explicit Vector3f(const Vector4f &p);
		~Vector3f() {}
};

///// INLINE FUNCTIONS /////

inline bool Vector3f::equalTo(const Vector3f &p) const
{
	if (math.abs(p.x - x) <= EPSILONf)
		if (math.abs(p.y - y) <= EPSILONf)
			if (math.abs(p.z - z) <= EPSILONf)
				return true;
	
	return false;
}

inline bool Vector3f::notEqualTo(const Vector3f &p) const
{
	if ((math.abs(p.x-x) > EPSILONf) || (math.abs(p.y-y) > EPSILONf) || (math.abs(p.z-z) > EPSILONf)) return true;
	else return false;
}

inline void Vector3f::add(const Vector3f &p1,const Vector3f &p2)
{
	x = p1.x + p2.x; y = p1.y + p2.y; z = p1.z + p2.z;
}

inline void Vector3f::subtract(const Vector3f &p1,const Vector3f &p2)
{
	x = p1.x - p2.x; y = p1.y - p2.y; z = p1.z - p2.z;
}

inline void Vector3f::multiply(const Vector3f &p1, const Vector3f &p2)
{
	x = p1.x * p2.x; y = p1.y * p2.y; z = p1.z * p2.z;
}

inline void	Vector3f::divide(const Vector3f &p1, const Vector3f &p2)
{
	_ASSERTE(p2.x != 0.0f && p2.y != 0.0f && p2.z != 0.0f);
	x = p1.x / p2.x; y = p1.y / p2.y; z = p1.z / p2.z;
}

inline void Vector3f::add(const Vector3f &p, float s)
{
	x = p.x + s; y = p.y + s; z = p.z + s;
}

inline void Vector3f::subtract(const Vector3f &p, float s)
{
	x = p.x - s; y = p.y - s; z = p.z - s;
}

inline void Vector3f::multiply(const Vector3f &p, float s)
{
	x = p.x * s; y = p.y * s; z = p.z * s;
}

inline void Vector3f::divide(const Vector3f &p, float s)
{
	_ASSERTE(s != 0.0f);
	s = 1.0f / s;
	x = p.x * s; y = p.y * s; z = p.z * s;
}

inline void Vector3f::crossProduct(const Vector3f &p1, const Vector3f &p2)
{
	// Sets point to cross product of two vectors
	x = (p1.y * p2.z) - (p1.z * p2.y),
	y = (p1.z * p2.x) - (p1.x * p2.z),
	z = (p1.x * p2.y) - (p1.y * p2.x);
}

inline float Vector3f::distance(const Vector3f &p) const
{
	const float dx = p.x - x;
	const float dy = p.y - y;
	const float dz = p.z - z;
	return sqrtf(dx*dx + dy*dy + dz*dz);
}

inline float Vector3f::distSquared(const Vector3f &p) const
{
	const float dx = p.x - x;
	const float dy = p.y - y;
	const float dz = p.z - z;
	return dx*dx + dy*dy + dz*dz;
}

inline void Vector3f::normalOf(const Vector3f &p1, const Vector3f &p2)
{
	// Calculates the normal vector with cross product
	x = (p1.y * p2.z) - (p1.z * p2.y);
	y = (p1.z * p2.x) - (p1.x * p2.z);
	z = (p1.x * p2.y) - (p1.y * p2.x);
}
