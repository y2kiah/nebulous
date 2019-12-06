/*----==== VECTOR2F.H ====----
	Author:	Jeffrey Kiah
	Date:	5/2004
----------------------------*/

#pragma once

#include <crtdbg.h>
#include <string>
#include <cmath>
#include "FastMath.h"
#include "../Utility Code/Typedefs.h"

///// STRUCTURES /////

class Vector2f {
	
	public:
		///// VARIABLES /////

		union {
			struct {
				float x, z;
			};

			float v[2];
		};

		///// OPERATORS /////

		// Conditional operators
		bool	operator==(const Vector2f &p) const { return (x == p.x && z == p.z); }
		bool	operator!=(const Vector2f &p) const { return (x != p.x || z != p.z); }

		// Assignment operators
		void	operator+=(const Vector2f &p) { x += p.x; z += p.z; }
		void	operator-=(const Vector2f &p) { x -= p.x; z -= p.z; }
		void	operator*=(float s) { x *= s; z *= s; }
		void	operator/=(float s) { _ASSERTE(s != 0.0f); x /= s; z /= s; }

		// Not as fast as functions below, but these allow streamed operations ie. V = VA + VB - VC
		Vector2f	operator- (void) const { return Vector2f(-x,-z); }
		Vector2f	operator+ (const Vector2f &p) const { return Vector2f(x+p.x, z+p.z); }
		Vector2f	operator- (const Vector2f &p) const { return Vector2f(x-p.x, z-p.z); }
		Vector2f	operator* (float s) const { return Vector2f(x*s, z*s); }
		Vector2f	operator/ (float s) const { _ASSERTE(s != 0.0f); return Vector2f(x/s, z/s); }

		// Dot product
		float	operator^ (const Vector2f &p) const { return (x * p.x) + (z * p.z); }

		///// FUNCTIONS /////

		void			assign(float _x, float _z) { x = _x; z = _z; }

		// Check equality with floating point error, SLOW
		inline bool		equalTo(const Vector2f &p) const;
		inline bool		notEqualTo(const Vector2f &p) const;

		// Faster than the overloaded operators above
		inline void		add(const Vector2f &p1, const Vector2f &p2);
		inline void		subtract(const Vector2f &p1, const Vector2f &p2);
		inline void		multiply(const Vector2f &p, float s);
		inline void		divide(const Vector2f &p, float s);
		
		// Finds the distance between two points
		inline float	dist(const Vector2f &p) const;
		inline float	distSquared(const Vector2f &p) const;

		// Finds the magnitude of the vector, or it's distance from the origin
		float			mag(void) const { return sqrtf(x*x + z*z); }
		float			magSquared(void) const { return x*x + z*z; }

		// Vector methods
		void			normalize(void);

		// Debugging
		std::string		toString(void) const;

		// Constructors / Destructors
		Vector2f() : x(0), z(0) {}
		Vector2f(const Vector2f &p) : x(p.x), z(p.z) {}
		explicit Vector2f(float _x, float _z) : x(_x), z(_z) {}
		~Vector2f() {}
};


/*------------------------
---- INLINE FUNCTIONS ----
------------------------*/


inline bool Vector2f::equalTo(const Vector2f &p) const
{
	if (math.abs(p.x - x) <= EPSILONf)
		if (math.abs(p.z - z) <= EPSILONf)
			return true;
	
	return false;
}


inline bool Vector2f::notEqualTo(const Vector2f &p) const
{
	if ((math.abs(p.x-x) > EPSILONf) || (math.abs(p.z-z) > EPSILONf)) return true;
	else return false;
}


inline void Vector2f::add(const Vector2f &p1, const Vector2f &p2)
{
	x = p1.x + p2.x;
	z = p1.z + p2.z;
}


inline void Vector2f::subtract(const Vector2f &p1, const Vector2f &p2)
{
	x = p1.x - p2.x;
	z = p1.z - p2.z;
}


inline void Vector2f::multiply(const Vector2f &p, float s)
{
	x = p.x * s;
	z = p.z * s;
}


inline void Vector2f::divide(const Vector2f &p, float s)
{
	_ASSERTE(s != 0.0f);

	x = p.x / s;
	z = p.z / s;
}


inline float Vector2f::dist(const Vector2f &p) const
{
	const float dx = p.x - x;
	const float dz = p.z - z;
	return sqrtf(dx*dx + dz*dz);
}


inline float Vector2f::distSquared(const Vector2f &p) const
{
	const float dx = p.x - x;
	const float dz = p.z - z;
	return dx*dx + dz*dz;
}
