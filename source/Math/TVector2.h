/*----==== TVECTOR2.H ====----
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

template <typename T>
class TVector2 {
	public:
		///// VARIABLES /////
		
		union {
			struct {
				T x, y;		// This union allows access to the same memory location through either
			};				// x,y or the array v[2]. Use v for fast OpenGL functions
			T v[2];
		};

		///// OPERATORS /////

		// Conditional operators
		bool		operator==(const TVector2<T> &p) const { return (x == p.x && y == p.y); }
		bool		operator!=(const TVector2<T> &p) const { return (x != p.x || y != p.y); }

		// Assignment operators
		void		operator= (const TVector2<T> &p) { x = p.x; y = p.y; }
		void		operator+=(const TVector2<T> &p) { x += p.x; y += p.y; }
		void		operator-=(const TVector2<T> &p) { x -= p.x; y -= p.y; }
		void		operator*=(const TVector2<T> &p) { x *= p.x; y *= p.y; }
		void		operator/=(const TVector2<T> &p) { _ASSERTE(p.x!=0&&p.y!=0); x /= p.x; y /= p.y; }
		void		operator+=(T s) { x += s; y += s; }
		void		operator-=(T s) { x -= s; y -= s; }
		void		operator*=(T s) { x *= s; y *= s; }
		inline void	operator/=(T s);

		// Not as fast as functions below, but these allow streamed operations ie. V = VA + VB - VC
		TVector2<T>			operator- () const { return TVector2<T>(-x,-y); } // unary
		// Scalar operations
		TVector2<T>			operator+ (T s) const { return TVector2<T>(x+s, y+s); }
		TVector2<T>			operator- (T s) const { return TVector2<T>(x-s, y-s); }
		TVector2<T>			operator* (T s) const {	return TVector2<T>(x*s, y*s); }
		inline TVector2<T>	operator/ (T s) const;
		// Vector operations
		TVector2<T>	operator+ (const TVector2<T> &p) const { return TVector2<T>(x+p.x, y+p.y); }
		TVector2<T>	operator- (const TVector2<T> &p) const { return TVector2<T>(x-p.x, y-p.y); }
		TVector2<T>	operator* (const TVector2<T> &p) const { return TVector2<T>(x*p.x, y*p.y); }
		TVector2<T>	operator/ (const TVector2<T> &p) const { _ASSERTE(p.x!=0&&p.y!=0); return TVector2<T>(x/p.x, y/p.y); }
		// Dot product
		T			operator^ (const TVector2<T> &p) const { return (x * p.x) + (y * p.y); }
		// Cross Product
		//TVector2<T>	operator% (const TVector2<T> &p) const { return TVector2<T>((y*p.z)-(z*p.y),(z*p.x)-(x*p.z),(x*p.y)-(y*p.x)); }

		///// FUNCTIONS /////

		// Convert from 4d vector to 3d vector, ignore w component of 4d vector
		void			assign(T _x, T _y) { x = _x; y = _y; }

		// Check equality with floating point error, SLOW
		inline bool		equalTo(const TVector2<T> &p, const T epsilon) const;
		inline bool		notEqualTo(const TVector2<T> &p, const T epsilon) const;

		// Faster than the overloaded operators above
		inline void		add(const TVector2<T> &p1, const TVector2<T> &p2);
		inline void		subtract(const TVector2<T> &p1, const TVector2<T> &p2);
		inline void		multiply(const TVector2<T> &p1, const TVector2<T> &p2);
		inline void		divide(const TVector2<T> &p1, const TVector2<T> &p2);
		inline void		add(const TVector2<T> &p, T s);
		inline void		subtract(const TVector2<T> &p, T s);
		inline void		multiply(const TVector2<T> &p, T s);
		inline void		divide(const TVector2<T> &p, T s);
//		inline void		crossProduct(const TVector2<T> &p1, const TVector2<T> &p2);

		// Distance
		inline T		distance(const TVector2<T> &p) const;
		inline T		distSquared(const TVector2<T> &p) const;

		// Finds the magnitude of the vector, or its distance from the origin
		inline T		magnitude() const;
		T				magSquared() const { return x*x + y*y; }

		// Vector methods
//		inline void		unitNormalOf(const TVector2<T> &p1, const TVector2<T> &p2);
		inline void		normalize();

		// Get angle to another vector
		inline float	angleRad(const TVector2<T> &p) const;
		inline float	angleDeg(const TVector2<T> &p) const;
		inline float	angleRadUnit(const TVector2<T> &p) const; // assumes both vectors are already unit (length = 1.0)
		inline float	angleDegUnit(const TVector2<T> &p) const; // and are faster by avoiding normalization step

		// 2D Rotation functions
//		inline void		rotate(TVector2<T> &p, uint xa, uint ya, uint za);
//		inline void		rotate(uint xa, uint ya, uint za);

		// Debugging
		inline string	toString() const;

		// Constructors / Destructor
		TVector2() : x(0), y(0) {}
		TVector2(const TVector2<T> &p) : x(p.x), y(p.y) {}
		explicit TVector2(T _x, T _y) : x(_x), y(_y) {}
		~TVector2() {}
};

///// DEFINES /////

typedef TVector2<float>		Vector2f;
typedef TVector2<double>	Vector2d;
typedef TVector2<int>		Vector2i;

///// INLINE OPERATORS /////

template <typename T>
inline void TVector2<T>::operator/=(T s)
{
	_ASSERTE(s != 0);
	x /= s; y /= s;
}

inline void TVector2<float>::operator/=(float s)
{
	_ASSERTE(s != 0.0f);
	s = 1.0f / s;
	x *= s; y *= s;
}

inline void TVector2<double>::operator/=(double s)
{
	_ASSERTE(s != 0.0);
	s = 1.0 / s;
	x *= s; y *= s;
}

template <typename T>
inline TVector2<T> TVector2<T>::operator/ (T s) const
{
	_ASSERTE(s != 0);
	return TVector2<T>(x/s, y/s);
}

inline TVector2<float> TVector2<float>::operator/ (float s) const
{
	_ASSERTE(s != 0.0f);
	s = 1.0f / s;
	return TVector2<float>(x*s, y*s);
}

inline TVector2<double> TVector2<double>::operator/ (double s) const
{
	_ASSERTE(s != 0.0);
	s = 1.0 / s;
	return TVector2<double>(x*s, y*s);
}

///// INLINE FUNCTIONS /////

template <typename T>
inline bool TVector2<T>::equalTo(const TVector2<T> &p, const T epsilon) const
{
	if (math.abs<T>(p.x - x) <= epsilon)
		if (math.abs<T>(p.y - y) <= epsilon)
				return true;	
	return false;
}

template <typename T>
inline bool TVector2<T>::notEqualTo(const TVector2<T> &p, const T epsilon) const
{
	if ((math.abs<T>(p.x-x) > epsilon) || (math.abs<T>(p.y-y) > epsilon)) return true;
	else return false;
}

template <typename T>
inline void TVector2<T>::add(const TVector2<T> &p1,const TVector2<T> &p2)
{
	x = p1.x + p2.x; y = p1.y + p2.y;
}

template <typename T>
inline void TVector2<T>::subtract(const TVector2<T> &p1,const TVector2<T> &p2)
{
	x = p1.x - p2.x; y = p1.y - p2.y;
}

template <typename T>
inline void TVector2<T>::multiply(const TVector2<T> &p1, const TVector2<T> &p2)
{
	x = p1.x * p2.x; y = p1.y * p2.y;
}

template <typename T>
inline void	TVector2<T>::divide(const TVector2<T> &p1, const TVector2<T> &p2)
{
	_ASSERTE(p2.x != 0 && p2.y != 0);
	x = p1.x / p2.x; y = p1.y / p2.y;
}

template <typename T>
inline void TVector2<T>::add(const TVector2<T> &p, T s)
{
	x = p.x + s; y = p.y + s;
}

template <typename T>
inline void TVector2<T>::subtract(const TVector2<T> &p, T s)
{
	x = p.x - s; y = p.y - s;
}

template <typename T>
inline void TVector2<T>::multiply(const TVector2<T> &p, T s)
{
	x = p.x * s; y = p.y * s;
}

template <typename T>
inline void TVector2<T>::divide(const TVector2<T> &p, T s)
{
	_ASSERTE(s != 0);
	x = p.x / s; y = p.y / s;
}

inline void TVector2<float>::divide(const TVector2<float> &p, float s)
{
	_ASSERTE(s != 0.0f);
	s = 1.0f / s;
	x = p.x * s; y = p.y * s;
}

inline void TVector2<double>::divide(const TVector2<double> &p, double s)
{
	_ASSERTE(s != 0.0);
	s = 1.0 / s;
	x = p.x * s; y = p.y * s;
}

/*template <typename T>
inline void TVector2<T>::crossProduct(const TVector2<T> &p1, const TVector2<T> &p2)
{
	// Sets point to cross product of two vectors
	x = (p1.y * p2.z) - (p1.z * p2.y);
	y = (p1.z * p2.x) - (p1.x * p2.z);
	z = (p1.x * p2.y) - (p1.y * p2.x);
}*/

template <typename T>
inline T TVector2<T>::distance(const TVector2<T> &p) const
{
	const T dx = p.x - x;
	const T dy = p.y - y;
	return static_cast<T>(sqrtf(dx*dx + dy*dy));
}

inline float TVector2<float>::distance(const TVector2<float> &p) const
{
	const float dx = p.x - x;
	const float dy = p.y - y;
	return sqrtf(dx*dx + dy*dy);
}

inline double TVector2<double>::distance(const TVector2<double> &p) const
{
	const double dx = p.x - x;
	const double dy = p.y - y;
	return sqrt(dx*dx + dy*dy);
}

inline int TVector2<int>::distance(const TVector2<int> &p) const
{
	const int dx = p.x - x;
	const int dy = p.y - y;
	return FastMath::sqrti(dx*dx + dy*dy);
}

template <typename T>
inline T TVector2<T>::distSquared(const TVector2<T> &p) const
{
	const T dx = p.x - x;
	const T dy = p.y - y;
	return dx*dx + dy*dy;
}

template <typename T>
inline T TVector2<T>::magnitude() const
{
	return static_cast<T>(sqrtf(x*x + y*y));
}

inline float TVector2<float>::magnitude() const
{
	return sqrtf(x*x + y*y);
}

inline double TVector2<double>::magnitude() const
{
	return sqrt(x*x + y*y);
}

inline int TVector2<int>::magnitude() const
{
	return FastMath::sqrti(x*x + y*y);
}

/*template <typename T>
inline void TVector2<T>::unitNormalOf(const TVector2<T> &p1, const TVector2<T> &p2)
{
	// Calculates the normal vector with cross product
	crossProduct(p1, p2);
	// Normalizes the vector
	normalize();
}*/

template <typename T>
inline void TVector2<T>::normalize()
{
	// Normalize the vector to unit length
	T mag = magnitude();
	_ASSERTE(mag != 0);
	x /= mag; y /= mag;
}

inline void TVector2<float>::normalize()
{
	float magSq = magSquared();
	_ASSERTE(magSq != 0.0f);
	float invMag = 1.0f / sqrtf(magSq);
	x *= invMag; y *= invMag;
}

inline void TVector2<double>::normalize()
{
	double magSq = magSquared();
	_ASSERTE(magSq != 0.0);
	double invMag = 1.0 / sqrt(magSq);
	x *= invMag; y *= invMag;
}

template <typename T>
inline float TVector2<T>::angleRad(const TVector2<T> &p) const
{
	return acosf(static_cast<float>((*this ^ p) / (magnitude() * p.magnitude())));
}

template <typename T>
inline float TVector2<T>::angleDeg(const TVector2<T> &p) const
{
	return angleRad(p) * RADTODEGf;
}

template <typename T>
inline float TVector2<T>::angleRadUnit(const TVector2<T> &p) const
{
	return acosf(static_cast<float>(*this ^ p));
}

template <typename T>
inline float TVector2<T>::angleDegUnit(const TVector2<T> &p) const
{
	return angleRadUnit(p) * RADTODEGf;
}

/*template <typename T>
inline void TVector2<T>::rotate(TVector2<T> &p, uint xa, uint ya, uint za)
{
	_ASSERTE(xa < math.ANGLE360);
	_ASSERTE(ya < math.ANGLE360);
	_ASSERTE(za < math.ANGLE360);

	*this = p;
	TVector2<T> tp;
	
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

template <typename T>
inline void TVector2<T>::rotate(uint xa, uint ya, uint za)
{
	_ASSERTE(xa < math.ANGLE360);
	_ASSERTE(ya < math.ANGLE360);
	_ASSERTE(za < math.ANGLE360);

	TVector2<T> tp;
	
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
}*/

template <typename T>
inline string TVector2<T>::toString() const
{
	ostringstream returnStr;
	returnStr << "(" << x << "," << y << ")";
	return returnStr.str();
}
