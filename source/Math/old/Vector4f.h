//	----==== VECTOR4F.H ====----
//
//	Author:			Jeffrey Kiah
//	Date:			4/04
//	Description:	A 4D double floating point vector class
//	----------------------------

#pragma once

#include <string>
#include <cmath>
#include "FastMath.h"
#include "../Utility Code/Typedefs.h"

///// STRUCTURES /////

class Vector3f;
class Matrix4x4f;


class Vector4f {
	public:
		///// VARIABLES /////
		
		union {
			struct {
				float x, y, z, w;	// This union allows access to the same memory location through either
			};						// x,y,z,w or the array v[4]. Use v for fast OpenGL functions

			float v[4];
		};

		///// OPERATORS /////

		// Conditional operators
		bool	operator==(const Vector4f &p) const { return (x == p.x && y == p.y && z == p.z && w == p.w); }
		bool	operator!=(const Vector4f &p) const { return (x != p.x || y != p.y || z != p.z || w != p.w); }

		// Assignment operators
		void	operator= (const Vector4f &p) { x = p.x; y = p.y; z = p.z; w = p.w; }
		void	operator+=(const Vector4f &p) { x += p.x; y += p.y; z += p.z; w += p.w; }
		void	operator-=(const Vector4f &p) { x -= p.x; y -= p.y; z -= p.z; w -= p.w; }
		void	operator*=(float s) { x *= s; y *= s; z *= s; w *= s; }
		void	operator*=(const Matrix4x4f &m);
		void	operator/=(float s) { _ASSERTE(s != 0.0f); s = 1.0f / s; x *= s; y *= s; z *= s; w *= s; }

		// Not as fast as functions below, but these allow streamed operations ie. V = VA + VB - VC
		Vector4f	operator- (void) const { return Vector4f(-x,-y,-z,-w); }
		Vector4f	operator+ (const Vector4f &p) const { return Vector4f(x+p.x, y+p.y, z+p.z, w+p.w); }
		Vector4f	operator- (const Vector4f &p) const { return Vector4f(x-p.x, y-p.y, z-p.z, w-p.w); }
		Vector4f	operator* (float s) const { return Vector4f(x*s, y*s, z*s, w*s); }
		Vector4f	operator/ (float s) const { _ASSERTE(s != 0.0f); s = 1.0f / s; return Vector4f(x*s, y*s, z*s, w*s); }

		Vector4f	operator* (const Matrix4x4f &m) const;

		// Dot product
		float		operator^ (const Vector4f &p) const { return (x * p.x) + (y * p.y) + (z * p.z) + (w * p.w); }

		///// FUNCTIONS /////

		// Convert from 3d vector to 4d vector, often w = 0 used for vectors and w = 1 used for points
		// when multiplying by matrix, w = 1 means it can be translated from origin
		void			assign(const Vector4f &p) { x = p.x; y = p.y; z = p.z; w = p.w; }
		void			assign(const Vector3f &p, float _w);
		void			assign(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }

		// Check equality with floating point error, SLOW
		inline bool		equalTo(const Vector4f &p) const;
		inline bool		notEqualTo(const Vector4f &p) const;

		// Faster than the overloaded operators above		
		inline void		add(const Vector4f &p1, const Vector4f &p2);
		inline void		subtract(const Vector4f &p1, const Vector4f &p2);
		inline void		multiply(const Vector4f &p, float s);
		inline void		divide(const Vector4f &p, float s);
		
		void			multiplyMatrix(const Vector4f &p, const Matrix4x4f &m);

		// Distance
		inline float	dist(const Vector4f &p) const;
		inline float	distSquared(const Vector4f &p) const;

		// Finds the magnitude of the vector, or it's distance from the origin
		float			mag(void) const { return sqrtf(x*x + y*y + z*z + w*w); }
		float			magSquared(void) const { return x*x + y*y + z*z + w*w; }

		// Vector methods
		void			normalize(void);
		
		// Debugging
		std::string		toString(void) const;

		// Constructors / Destructor
		Vector4f() : x(0), y(0), z(0), w(0) {}
		Vector4f(const Vector4f &p) : x(p.x), y(p.y), z(p.z), w(p.w) {}
		explicit Vector4f(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
		explicit Vector4f(const Vector3f &p, float _w);
		~Vector4f() {}
};


///// INLINE FUNCTIONS /////

inline bool Vector4f::equalTo(const Vector4f &p) const
{
	if (math.abs(p.x - x) <= EPSILONf)
		if (math.abs(p.y - y) <= EPSILONf)
			if (math.abs(p.z - z) <= EPSILONf)
				if (math.abs(p.w - w) <= EPSILONf)
					return true;
	
	return false;
}


inline bool Vector4f::notEqualTo(const Vector4f &p) const
{
	if ((math.abs(p.x - x) > EPSILONf) || (math.abs(p.y - y) > EPSILONf) || (math.abs(p.z - z) > EPSILONf) || (math.abs(p.w - w) > EPSILONf))
		return true;
	else
		return false;
}


inline void Vector4f::add(const Vector4f &p1,const Vector4f &p2)
{
	x = p1.x + p2.x;
	y = p1.y + p2.y;
	z = p1.z + p2.z;
	w = p1.w + p2.w;
}


inline void Vector4f::subtract(const Vector4f &p1,const Vector4f &p2)
{
	x = p1.x - p2.x;
	y = p1.y - p2.y;
	z = p1.z - p2.z;
	w = p1.w - p2.w;
}


inline void Vector4f::multiply(const Vector4f &p, float s)
{
	x = p.x * s;
	y = p.y * s;
	z = p.z * s;
	w = p.w * s;
}


inline void Vector4f::divide(const Vector4f &p, float s)
{
	_ASSERTE(s != 0.0f);

	s = 1.0f / s;
	x = p.x * s;
	y = p.y * s;
	z = p.z * s;
	w = p.w * s;
}


inline float Vector4f::dist(const Vector4f &p) const
{
	const float dx = p.x - x;
	const float dy = p.y - y;
	const float dz = p.z - z;
	const float dw = p.w - w;
	return sqrtf(dx*dx + dy*dy + dz*dz + dw*dw);
}


inline float Vector4f::distSquared(const Vector4f &p) const
{
	const float dx = p.x - x;
	const float dy = p.y - y;
	const float dz = p.z - z;
	const float dw = p.w - w;
	return dx*dx + dy*dy + dz*dz + dw*dw;
}
