/*----==== TVECTOR4.H ====----
	Author:		Jeff Kiah
	Orig.Date:	07/17/2009
	Rev.Date:	07/17/2009
----------------------------*/

#pragma once

#include <sstream>
#include <cmath>
#include "FastMath.h"
#include "../Utility/Typedefs.h"

using std::string;
using std::ostringstream;

///// STRUCTURES /////

//class Matrix4x4f;

template <typename T>
class TVector4 {
	public:
		///// VARIABLES /////
		
		union {
			struct {
				T x, y, z, w;
			};
			T v[4];
		};

		///// OPERATORS /////

		// Conditional operators
		bool		operator==(const TVector4<T> &p) const { return (x == p.x && y == p.y && z == p.z && w == p.w); }
		bool		operator!=(const TVector4<T> &p) const { return (x != p.x || y != p.y || z != p.z || w != p.w); }

		// Assignment operators
		void		operator= (const TVector4<T> &p) { x = p.x; y = p.y; z = p.z; w = p.w }
		void		operator+=(const TVector4<T> &p) { x += p.x; y += p.y; z += p.z; w += p.w; }
		void		operator-=(const TVector4<T> &p) { x -= p.x; y -= p.y; z -= p.z; w -= p.w; }
		void		operator*=(const TVector4<T> &p) { x *= p.x; y *= p.y; z *= p.z; w *= p.w; }
		void		operator/=(const TVector4<T> &p) { _ASSERTE(p.x!=0&&p.y!=0&&p.z!=0&&p.w!=0); x /= p.x; y /= p.y; z /= p.z; w /= p.w; }
		void		operator+=(T s) { x += s; y += s; z += s; w += s; }
		void		operator-=(T s) { x -= s; y -= s; z -= s; w -= s; }
		void		operator*=(T s) { x *= s; y *= s; z *= s; w *= s; }
		inline void	operator/=(T s);
		//void		operator*=(const Matrix4x4f &m);

		// Not as fast as functions below, but these allow streamed operations ie. V = VA + VB - VC
		TVector4<T>	operator- () const { return TVector4<T>(-x,-y,-z,-w); } // unary
		// Scalar operations
		TVector4<T>			operator+ (T s) const { return TVector4<T>(x+s, y+s, z+s, w+s); }
		TVector4<T>			operator- (T s) const { return TVector4<T>(x-s, y-s, z-s, w-s); }
		TVector4<T>			operator* (T s) const {	return TVector4<T>(x*s, y*s, z*s, w*s); }
		inline TVector4<T>	operator/ (T s) const;
		//TVector4<T>	operator* (const Matrix4x4f &m) const;	// Use 3x3 portion of matrix
		// Vector operations
		TVector4<T>	operator+ (const TVector4<T> &p) const { return TVector4<T>(x+p.x, y+p.y, z+p.z, w+p.w); }
		TVector4<T>	operator- (const TVector4<T> &p) const { return TVector4<T>(x-p.x, y-p.y, z-p.z, w-p.w); }
		TVector4<T>	operator* (const TVector4<T> &p) const { return TVector4<T>(x*p.x, y*p.y, z*p.z, w*p.w); }
		TVector4<T>	operator/ (const TVector4<T> &p) const { _ASSERTE(p.x!=0&&p.y!=0&&p.z!=0&&p.w!=0); return TVector4<T>(x/p.x, y/p.y, z/p.z, w/p.w); }
		// Dot product
		T		operator^ (const TVector4<T> &p) const { return (x * p.x) + (y * p.y) + (z * p.z) + (w * p.w); }

		///// FUNCTIONS /////

		// Convert from 4d vector to 3d vector, ignore w component of 4d vector
		void			assign(T _x, T _y, T _z, T _w) { x = _x; y = _y; z = _z; w = _w; }

		// Check equality with floating point error, SLOW
		inline bool		equalTo(const TVector4<T> &p, const T epsilon) const;
		inline bool		notEqualTo(const TVector4<T> &p, const T epsilon) const;

		// Faster than the overloaded operators above
		inline void		add(const TVector4<T> &p1, const TVector4<T> &p2);
		inline void		subtract(const TVector4<T> &p1, const TVector4<T> &p2);
		inline void		multiply(const TVector4<T> &p1, const TVector4<T> &p2);
		inline void		divide(const TVector4<T> &p1, const TVector4<T> &p2);
		inline void		add(const TVector4<T> &p, T s);
		inline void		subtract(const TVector4<T> &p, T s);
		inline void		multiply(const TVector4<T> &p, T s);
		inline void		divide(const TVector4<T> &p, T s);
		inline void		crossProduct(const TVector4<T> &p1, const TVector4<T> &p2, const TVector4<T> &p3);
		//void			multiplyMatrix(const TVector4<T> &p, const Matrix4x4f &m);

		// Distance
		inline T		distance(const TVector4<T> &p) const;
		inline T		distSquared(const TVector4<T> &p) const;

		// Finds the magnitude of the vector, or it's distance from the origin
		inline T		magnitude() const;
		T				magSquared() const { return x*x + y*y + z*z + w*w; }

		// Vector methods
		inline void		unitNormalOf(const TVector4<T> &p1, const TVector4<T> &p2, const TVector4<T> &p3);
		inline void		normalize();
		
		// Debugging
		inline string	toString() const;

		// Constructors / Destructor
		TVector4() : x(0), y(0), z(0), w(0) {}
		TVector4(const TVector4<T> &p) : x(p.x), y(p.y), z(p.z), w(p.w) {}
		explicit TVector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
		~TVector4() {}
};

///// DEFINES /////

typedef TVector4<float>		Vector4f;
typedef TVector4<double>	Vector4d;
typedef TVector4<int>		Vector4i;

///// INLINE OPERATORS /////

template <typename T>
inline void TVector4<T>::operator/=(T s)
{
	_ASSERTE(s != 0);
	x /= s; y /= s; z /= s; w /= s;
}

inline void TVector4<float>::operator/=(float s)
{
	_ASSERTE(s != 0.0f);
	s = 1.0f / s;
	x *= s; y *= s; z *= s; w *= s;
}

inline void TVector4<double>::operator/=(double s)
{
	_ASSERTE(s != 0.0);
	s = 1.0 / s;
	x *= s; y *= s; z *= s; w *= s;
}

template <typename T>
inline TVector4<T> TVector4<T>::operator/ (T s) const
{
	_ASSERTE(s != 0);
	return TVector4<T>(x/s, y/s, z/s, w/s);
}

inline TVector4<float> TVector4<float>::operator/ (float s) const
{
	_ASSERTE(s != 0.0f);
	s = 1.0f / s;
	return TVector4<float>(x*s, y*s, z*s, w*s);
}

inline TVector4<double> TVector4<double>::operator/ (double s) const
{
	_ASSERTE(s != 0.0);
	s = 1.0 / s;
	return TVector4<double>(x*s, y*s, z*s, w*s);
}

///// INLINE FUNCTIONS /////

template <typename T>
inline bool TVector4<T>::equalTo(const TVector4<T> &p, const T epsilon) const
{
	if (math.abs(p.x - x) <= epsilon)
		if (math.abs(p.y - y) <= epsilon)
			if (math.abs(p.z - z) <= epsilon)
				if (math.abs(p.w - w) <= epsilon)
					return true;
	return false;
}

template <typename T>
inline bool TVector4<T>::notEqualTo(const TVector4<T> &p, const T epsilon) const
{
	if ((math.abs(p.x-x) > epsilon) || (math.abs(p.y-y) > epsilon) || (math.abs(p.z-z) > epsilon) || (math.abs(p.w-w) > epsilon)) return true;
	else return false;
}

template <typename T>
inline void TVector4<T>::add(const TVector4<T> &p1,const TVector4<T> &p2)
{
	x = p1.x + p2.x; y = p1.y + p2.y; z = p1.z + p2.z; w = p1.w + p2.w;
}

template <typename T>
inline void TVector4<T>::subtract(const TVector4<T> &p1,const TVector4<T> &p2)
{
	x = p1.x - p2.x; y = p1.y - p2.y; z = p1.z - p2.z; w = p1.w - p2.w;
}

template <typename T>
inline void TVector4<T>::multiply(const TVector4<T> &p1, const TVector4<T> &p2)
{
	x = p1.x * p2.x; y = p1.y * p2.y; z = p1.z * p2.z; w = p1.w * p2.w;
}

template <typename T>
inline void	TVector4<T>::divide(const TVector4<T> &p1, const TVector4<T> &p2)
{
	_ASSERTE(p2.x != 0 && p2.y != 0 && p2.z != 0 && p2.w != 0);
	x = p1.x / p2.x; y = p1.y / p2.y; z = p1.z / p2.z; w = p1.w / p2.w;
}

template <typename T>
inline void TVector4<T>::add(const TVector4<T> &p, T s)
{
	x = p.x + s; y = p.y + s; z = p.z + s; w = p.w + s;
}

template <typename T>
inline void TVector4<T>::subtract(const TVector4<T> &p, T s)
{
	x = p.x - s; y = p.y - s; z = p.z - s; w = p.w - s;
}

template <typename T>
inline void TVector4<T>::multiply(const TVector4<T> &p, T s)
{
	x = p.x * s; y = p.y * s; z = p.z * s; w = p.w * s;
}

template <typename T>
inline void TVector4<T>::divide(const TVector4<T> &p, T s)
{
	_ASSERTE(s != 0);
	x = p.x / s; y = p.y / s; z = p.z / s; w = p.w / s;
}

inline void TVector4<float>::divide(const TVector4<float> &p, float s)
{
	_ASSERTE(s != 0.0f);
	s = 1.0f / s;
	x = p.x * s; y = p.y * s; z = p.z * s; w = p.w * s;
}

inline void TVector4<double>::divide(const TVector4<double> &p, double s)
{
	_ASSERTE(s != 0.0);
	s = 1.0 / s;
	x = p.x * s; y = p.y * s; z = p.z * s; w = p.w * s;
}

template <typename T>
inline void TVector4<T>::crossProduct(const TVector4<T> &p1, const TVector4<T> &p2, const TVector4<T> &p3)
{
    // calc intermediate values
    const T a = (p2.x * p3.y) - (p2.y * p3.x);
    const T b = (p2.x * p3.z) - (p2.z * p3.x);
    const T c = (p2.x * p3.w) - (p2.w * p3.x);
    const T d = (p2.y * p3.z) - (p2.z * p3.y);
    const T e = (p2.y * p3.w) - (p2.w * p3.y);
    const T f = (p2.z * p3.w) - (p2.w * p3.z);
    // calc the result-vector components
    x =  (p1.y * f) - (p1.z * e) + (p1.w * d);
    y = -(p1.x * f) + (p1.z * c) - (p1.w * b);
    z =  (p1.x * e) - (p1.y * c) + (p1.w * a);
    w = -(p1.x * d) + (p1.y * b) - (p1.z * a);
}

template <typename T>
inline T TVector4<T>::distance(const TVector4<T> &p) const
{
	const T dx = p.x - x;
	const T dy = p.y - y;
	const T dz = p.z - z;
	const T dw = p.w - w;
	return static_cast<T>(sqrtf(dx*dx + dy*dy + dz*dz + dw*dw));
}

inline float TVector4<float>::distance(const TVector4<float> &p) const
{
	const float dx = p.x - x;
	const float dy = p.y - y;
	const float dz = p.z - z;
	const float dw = p.w - w;
	return sqrtf(dx*dx + dy*dy + dz*dz + dw*dw);
}

inline double TVector4<double>::distance(const TVector4<double> &p) const
{
	const double dx = p.x - x;
	const double dy = p.y - y;
	const double dz = p.z - z;
	const double dw = p.w - w;
	return sqrt(dx*dx + dy*dy + dz*dz + dw*dw);
}

inline int TVector4<int>::distance(const TVector4<int> &p) const
{
	const int dx = p.x - x;
	const int dy = p.y - y;
	const int dz = p.z - z;
	const int dw = p.w - w;
	return FastMath::sqrti(dx*dx + dy*dy + dz*dz + dw*dw);
}

template <typename T>
inline T TVector4<T>::distSquared(const TVector4<T> &p) const
{
	const T dx = p.x - x;
	const T dy = p.y - y;
	const T dz = p.z - z;
	const T dw = p.w - w;
	return dx*dx + dy*dy + dz*dz + dw*dw;
}

template <typename T>
inline T TVector4<T>::magnitude() const
{
	return static_cast<T>(sqrtf(x*x + y*y + z*z + w*w));
}

inline float TVector4<float>::magnitude() const
{
	return sqrtf(x*x + y*y + z*z + w*w);
}

inline double TVector4<double>::magnitude() const
{
	return sqrt(x*x + y*y + z*z + w*w);
}

inline int TVector4<int>::magnitude() const
{
	return FastMath::sqrti(x*x + y*y + z*z + w*w);
}

template <typename T>
inline void TVector4<T>::unitNormalOf(const TVector4<T> &p1, const TVector4<T> &p2, const TVector4<T> &p3)
{
	// Calculates the normal vector with cross product
	crossProduct(p1, p2, p3);
	// Normalizes the vector
	normalize();
}

template <typename T>
inline void TVector4<T>::normalize()
{
	// Normalize the vector to unit length
	T mag = magnitude();
	_ASSERTE(mag != 0);
	x /= mag; y /= mag; z /= mag; w /= mag;
}

inline void TVector4<float>::normalize()
{
	float magSq = magSquared();
	_ASSERTE(magSq != 0.0f);
	float invMag = 1.0f / sqrtf(magSq);
	x *= invMag; y *= invMag; z *= invMag; w *= invMag;
}

inline void TVector4<double>::normalize()
{
	double magSq = magSquared();
	_ASSERTE(magSq != 0.0);
	double invMag = 1.0 / sqrt(magSq);
	x *= invMag; y *= invMag; z *= invMag; w *= invMag;
}

template <typename T>
string TVector4<T>::toString() const
{
	ostringstream returnStr;
	returnStr << "(" << x << "," << y << "," << z << "," << w << ")";
	return returnStr.str();
}
