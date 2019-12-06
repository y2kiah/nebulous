/*----==== VECTOR3D.H ====----
	Author:	Jeffrey Kiah
	Date:	7/4/2007
----------------------------*/

#pragma once

#include <string>
#include <cmath>
#include "FastMath.h"
#include "../Utility Code/Typedefs.h"

///// STRUCTURES /////

class Vector4d;
class Matrix4x4d;

class Vector3d {
	public:
		///// VARIABLES /////
		
		union {
			struct {
				double x, y, z;		// This union allows access to the same memory location through either
			};						// x,y,z or the array v[3]. Use v for fast OpenGL functions

			double v[3];
		};

		///// OPERATORS /////

		// Conditional operators
		bool	operator==(const Vector3d &p) const { return (x == p.x && y == p.y && z == p.z); }
		bool	operator!=(const Vector3d &p) const { return (x != p.x || y != p.y || z != p.z); }

		// Assignment operators
		void	operator= (const Vector3d &p) { x = p.x; y = p.y; z = p.z; }
		void	operator+=(const Vector3d &p) { x += p.x; y += p.y; z += p.z; }
		void	operator-=(const Vector3d &p) { x -= p.x; y -= p.y; z -= p.z; }
		void	operator*=(double s) { x *= s; y *= s; z *= s; }
		void	operator*=(const Matrix4x4d &m);
		void	operator/=(double s) { _ASSERTE(s != 0.0); s = 1.0 / s; x *= s; y *= s; z *= s; }

		// Not as fast as functions below, but these allow streamed operations ie. V = VA + VB - VC
		Vector3d	operator- (void) const { return Vector3d(-x,-y,-z); }
		Vector3d	operator+ (const Vector3d &p) const { return Vector3d(x+p.x, y+p.y, z+p.z); }
		Vector3d	operator- (const Vector3d &p) const { return Vector3d(x-p.x, y-p.y, z-p.z); }
		Vector3d	operator* (double s) const { return Vector3d(x*s, y*s, z*s); }
		Vector3d	operator/ (double s) const { _ASSERTE(s != 0.0f); s = 1.0f / s; return Vector3d(x*s, y*s, z*s); }

		Vector3d	operator* (const Matrix4x4d &m) const;	// Use 3x3 portion of matrix

		// Dot product
		double	operator^ (const Vector3d &p) const { return (x * p.x) + (y * p.y) + (z * p.z); }

		// Cross Product
		Vector3d	operator% (const Vector3d &p) const { return Vector3d((y*p.z)-(z*p.y),(z*p.x)-(x*p.z),(x*p.y)-(y*p.x)); }

		///// FUNCTIONS /////

		// Convert from 4d vector to 3d vector, ignore w component of 4d vector
		void			assign(const Vector4d &p);
		void			assign(double _x, double _y, double _z) { x = _x; y = _y; z = _z; }

		// Check equality with floating point error, SLOW
		inline bool		equalTo(const Vector3d &p) const;
		inline bool		notEqualTo(const Vector3d &p) const;

		// Faster than the overloaded operators above
		inline void		add(const Vector3d &p1, const Vector3d &p2);
		inline void		subtract(const Vector3d &p1, const Vector3d &p2);
		inline void		multiply(const Vector3d &p, double s);
		inline void		divide(const Vector3d &p, double s);
		inline void		crossProduct(const Vector3d &p1, const Vector3d &p2);
		
		void			multiplyMatrix(const Vector3d &p, const Matrix4x4d &m);

		// Distance
		inline double	dist(const Vector3d &p) const;
		inline double	distSquared(const Vector3d &p) const;

		// Finds the magnitude of the vector, or it's distance from the origin
		double			mag(void) const { return sqrt(x*x + y*y + z*z); }
		double			magSquared(void) const { return x*x + y*y + z*z; }

		// Vector methods
		inline void		normalOf(const Vector3d &p1, const Vector3d &p2);
		void			unitNormalOf(const Vector3d &p1, const Vector3d &p2);
		void			normalize(void);
		
		// 3D Rotation functions
		void			rot3D(Vector3d &p, uint xa, uint ya, uint za);
		void			rot3D(uint xa, uint ya, uint za);

		// Debugging
		std::string		toString(void) const;

		// Constructors / Destructor
		Vector3d() : x(0), y(0), z(0) {}
		Vector3d(const Vector3d &p) : x(p.x), y(p.y), z(p.z) {}
		explicit Vector3d(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
		explicit Vector3d(const Vector4d &p);
		~Vector3d() {}
};

///// INLINE FUNCTIONS /////

inline bool Vector3d::equalTo(const Vector3d &p) const
{
	if (math.abs(p.x - x) <= EPSILONd)
		if (math.abs(p.y - y) <= EPSILONd)
			if (math.abs(p.z - z) <= EPSILONd)
				return true;
	
	return false;
}

inline bool Vector3d::notEqualTo(const Vector3d &p) const
{
	if ((math.abs(p.x-x) > EPSILONd) || (math.abs(p.y-y) > EPSILONd) || (math.abs(p.z-z) > EPSILONd)) return true;
	else return false;
}

inline void Vector3d::add(const Vector3d &p1,const Vector3d &p2)
{
	x = p1.x + p2.x;
	y = p1.y + p2.y;
	z = p1.z + p2.z;
}

inline void Vector3d::subtract(const Vector3d &p1,const Vector3d &p2)
{
	x = p1.x - p2.x;
	y = p1.y - p2.y;
	z = p1.z - p2.z;
}

inline void Vector3d::multiply(const Vector3d &p, double s)
{
	x = p.x * s;
	y = p.y * s;
	z = p.z * s;
}

inline void Vector3d::divide(const Vector3d &p, double s)
{
	_ASSERTE(s != 0.0);

	s = 1.0 / s;
	x = p.x * s;
	y = p.y * s;
	z = p.z * s;
}

inline void Vector3d::crossProduct(const Vector3d &p1, const Vector3d &p2)
{
	// Sets point to cross product of two vectors
	x = (p1.y * p2.z) - (p1.z * p2.y),
	y = (p1.z * p2.x) - (p1.x * p2.z),
	z = (p1.x * p2.y) - (p1.y * p2.x);
}

inline double Vector3d::dist(const Vector3d &p) const
{
	const double dx = p.x - x;
	const double dy = p.y - y;
	const double dz = p.z - z;
	return sqrt(dx*dx + dy*dy + dz*dz);
}

inline double Vector3d::distSquared(const Vector3d &p) const
{
	const double dx = p.x - x;
	const double dy = p.y - y;
	const double dz = p.z - z;
	return dx*dx + dy*dy + dz*dz;
}

inline void Vector3d::normalOf(const Vector3d &p1, const Vector3d &p2)
{
	// Calculates the normal vector with cross product
	x = (p1.y * p2.z) - (p1.z * p2.y);
	y = (p1.z * p2.x) - (p1.x * p2.z);
	z = (p1.x * p2.y) - (p1.y * p2.x);
}
