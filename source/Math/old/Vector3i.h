/*----==== VECTOR3I.H ====----
	Author:	Jeffrey Kiah
	Date:	7/1/2007
----------------------------*/


#pragma once


#include <string>
#include <cmath>
#include "../Utility Code/Typedefs.h"

///// STRUCTURES /////

//class Vector4i;

class Vector3i {
	public:
		///// VARIABLES /////
		
		union {
			struct {
				int x, y, z;		// This union allows access to the same memory location through either
			};						// x,y,z or the array v[3]

			int v[3];
		};

		///// OPERATORS /////

		// Conditional operators
		bool	operator==(const Vector3i &p) const { return (x == p.x && y == p.y && z == p.z); }
		bool	operator!=(const Vector3i &p) const { return (x != p.x || y != p.y || z != p.z); }

		// Assignment operators
		void	operator= (const Vector3i &p) { x = p.x; y = p.y; z = p.z; }
		void	operator+=(const Vector3i &p) { x += p.x; y += p.y; z += p.z; }
		void	operator-=(const Vector3i &p) { x -= p.x; y -= p.y; z -= p.z; }
		void	operator*=(int s) { x *= s; y *= s; z *= s; }
		void	operator/=(int s) { _ASSERTE(s != 0); x /= s; y /= s; z /= s; }

		// Not as fast as functions below, but these allow streamed operations ie. V = VA + VB - VC
		Vector3i	operator- (void) const { return Vector3i(-x,-y,-z); }
		Vector3i	operator+ (const Vector3i &p) const { return Vector3i(x+p.x, y+p.y, z+p.z); }
		Vector3i	operator- (const Vector3i &p) const { return Vector3i(x-p.x, y-p.y, z-p.z); }
		Vector3i	operator* (int s) const { return Vector3i(x*s, y*s, z*s); }
		Vector3i	operator/ (int s) const { _ASSERTE(s != 0); return Vector3i(x/s, y/s, z/s); }

		// Dot product
		int			operator^ (const Vector3i &p) const { return (x * p.x) + (y * p.y) + (z * p.z); }

		///// FUNCTIONS /////

		// Convert from 4d vector to 3d vector, ignore w component of 4d vector
//		void			assign(const Vector4i &p);
		void			assign(int _x, int _y, int _z) { x = _x; y = _y; z = _z; }

		// Check equality with floating point error, SLOW
		inline bool		equalTo(const Vector3i &p) const;
		inline bool		notEqualTo(const Vector3i &p) const;

		// Faster than the overloaded operators above
		inline void		add(const Vector3i &p1, const Vector3i &p2);
		inline void		subtract(const Vector3i &p1, const Vector3i &p2);
		inline void		multiply(const Vector3i &p, int s);
		inline void		divide(const Vector3i &p, int s);
		inline void		crossProduct(const Vector3i &p1, const Vector3i &p2);

		// Distance
		inline float	dist(const Vector3i &p) const;
		inline int		distSquared(const Vector3i &p) const;

		// Finds the magnitude of the vector, or it's distance from the origin
		float			mag(void) const { return sqrtf(static_cast<float>(x*x + y*y + z*z)); }
		int				magSquared(void) const { return x*x + y*y + z*z; }

		// Debugging
		std::string		toString(void) const;

		// Constructors / Destructor
		Vector3i() : x(0), y(0), z(0) {}
		Vector3i(const Vector3i &p) : x(p.x), y(p.y), z(p.z) {}
		explicit Vector3i(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
//		explicit Vector3i(const Vector4i &p);
		~Vector3i() {}
};


/*------------------------
---- INLINE FUNCTIONS ----
------------------------*/


inline bool Vector3i::equalTo(const Vector3i &p) const
{
	if (p.x == x)
		if (p.y == y)
			if (p.z == z)
				return true;
	
	return false;
}


inline bool Vector3i::notEqualTo(const Vector3i &p) const
{
	if (p.x == x || p.y == y || p.z == z) return true;
	else return false;
}


inline void Vector3i::add(const Vector3i &p1,const Vector3i &p2)
{
	x = p1.x + p2.x;
	y = p1.y + p2.y;
	z = p1.z + p2.z;
}


inline void Vector3i::subtract(const Vector3i &p1,const Vector3i &p2)
{
	x = p1.x - p2.x;
	y = p1.y - p2.y;
	z = p1.z - p2.z;
}


inline void Vector3i::multiply(const Vector3i &p, int s)
{
	x = p.x * s;
	y = p.y * s;
	z = p.z * s;
}


inline void Vector3i::divide(const Vector3i &p, int s)
{
	_ASSERTE(s != 0);
	
	x = p.x / s;
	y = p.y / s;
	z = p.z / s;
}


inline float Vector3i::dist(const Vector3i &p) const
{
	const float dx = static_cast<float>(p.x - x);
	const float dy = static_cast<float>(p.y - y);
	const float dz = static_cast<float>(p.z - z);
	return sqrtf(dx*dx + dy*dy + dz*dz);
}


inline int Vector3i::distSquared(const Vector3i &p) const
{
	const int dx = p.x - x;
	const int dy = p.y - y;
	const int dz = p.z - z;
	return dx*dx + dy*dy + dz*dz;
}
