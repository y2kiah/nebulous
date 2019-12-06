/*----==== TVECTOR3.H ====----
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
class TVector3 {
	public:
		///// VARIABLES /////
		union {
			struct {
				T x, y, z;		// This union allows access to the same memory location through either
			};					// x,y,z or the array v[3]. Use v for fast OpenGL functions
			T v[3];
		};

		///// OPERATORS /////

		// Conditional operators
		bool	operator==(const TVector3<T> &p) const { return (x == p.x && y == p.y && z == p.z); }
		bool	operator!=(const TVector3<T> &p) const { return (x != p.x || y != p.y || z != p.z); }

		// Assignment operators
		void	operator= (const TVector3<T> &p) { assign(p); }
		void	operator+=(const TVector3<T> &p) { add(p); }
		void	operator-=(const TVector3<T> &p) { subtract(p); }
		void	operator*=(const TVector3<T> &p) { multiply(p); }
		void	operator+=(const T s) { add(s); }
		void	operator-=(const T s) { subtract(s); }
		void	operator*=(const T s) { multiply(s); }
		void	operator/=(const T s) { divide(s); }
		//void	operator*=(const Matrix4x4f &m);

		// Not as fast as functions below, but these allow streamed operations e.g. V = VA + VB - VC
		TVector3<T>		operator- () const { return TVector3<T>(-x,-y,-z); } // unary
		// Scalar operations
		TVector3<T>		operator+ (const T s) const { return TVector3<T>(x+s, y+s, z+s); }
		TVector3<T>		operator- (const T s) const { return TVector3<T>(x-s, y-s, z-s); }
		TVector3<T>		operator* (const T s) const { return TVector3<T>(x*s, y*s, z*s); }
		inline TVector3<T>	operator/ (T s) const;
		//TVector3<T>	operator* (const Matrix4x4f &m) const;	// Use 3x3 portion of matrix
		// Vector operations
		TVector3<T>		operator+ (const TVector3<T> &p) const { return TVector3<T>(x+p.x, y+p.y, z+p.z); }
		TVector3<T>		operator- (const TVector3<T> &p) const { return TVector3<T>(x-p.x, y-p.y, z-p.z); }
		TVector3<T>		operator* (const TVector3<T> &p) const { return TVector3<T>(y*p.z-z*p.y, z*p.x-x*p.z, x*p.y-y*p.x); }

		///// FUNCTIONS /////

		// Convert from 4d vector to 3d vector, ignore w component of 4d vector
		void			assign(T _x, T _y, T _z) { x = _x; y = _y; z = _z; }
		void			assign(const TVector3<T> p) { x = p.x; y = p.y; z = p.z; }

		// Check equality with floating point error, SLOW
		inline bool		equalTo(const TVector3<T> &p, const T epsilon) const;
		inline bool		notEqualTo(const TVector3<T> &p, const T epsilon) const;

		// Faster than the overloaded operators above
		inline void		add(const TVector3<T> &p, const T s);
		inline void		subtract(const TVector3<T> &p, const T s);
		inline void		multiply(const TVector3<T> &p, const T s);
		inline void		divide(const TVector3<T> &p, T s);
		inline void		add(const TVector3<T> &p1, const TVector3<T> &p2);
		inline void		subtract(const TVector3<T> &p1, const TVector3<T> &p2);
		inline void		cross(const TVector3<T> &p1, const TVector3<T> &p2);
		inline void		lerp(const TVector3<T> &p1, const TVector3<T> &p2, const T t);
		//void			multiplyMatrix(const TVector3<T> &p, const Matrix4x4f &m);
		
		// Fast operators for chaining commands
		inline TVector3<T> & add(const T s);
		inline TVector3<T> & subtract(const T s);
		inline TVector3<T> & multiply(const T s);
		inline TVector3<T> & divide(T s);
		inline TVector3<T> & add(const TVector3<T> &p);
		inline TVector3<T> & subtract(const TVector3<T> &p);
		inline TVector3<T> & cross(const TVector3<T> &p);
		inline TVector3<T> & lerp(const TVector3<T> &p, const T t);

		// Dot product
		T				dot(const TVector3<T> &p) const { return x*x + y*y + z*z; }

		// Distance
		inline T		distance(const TVector3<T> &p) const;
		inline T		distSquared(const TVector3<T> &p) const;

		// Finds the magnitude of the vector, or its distance from the origin
		inline T		magnitude() const;
		T				magSquared() const { return x*x + y*y + z*z; }

		// Vector methods
		inline void		unitNormalOf(const TVector3<T> &p1, const TVector3<T> &p2);
		inline void		normalize();

		// Get angle to another vector
		inline float	angleRad(const TVector3<T> &p) const;
		inline float	angleDeg(const TVector3<T> &p) const;
		inline float	angleRadUnit(const TVector3<T> &p) const; // assumes both vectors are already unit (length = 1.0)
		inline float	angleDegUnit(const TVector3<T> &p) const; // and are faster by avoiding normalization step

		// 3D Rotation functions
		inline void		rotate(TVector3<T> &p, uint xa, uint ya, uint za);
		inline void		rotate(uint xa, uint ya, uint za);

		// Debugging
		inline string	toString() const;

		// Constructors / Destructor
		TVector3() : x(0), y(0), z(0) {}
		TVector3(const TVector3<T> &p) : x(p.x), y(p.y), z(p.z) {}
		explicit TVector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
		~TVector3() {}
};

///// DEFINES /////

typedef TVector3<float>		Vector3f;
typedef TVector3<double>	Vector3d;
typedef TVector3<int>		Vector3i;

///// INLINE OPERATORS /////

template <typename T>
inline TVector3<T> TVector3<T>::operator/ (T s) const
{
	_ASSERTE(s != 0);
	return TVector3<T>(x/s, y/s, z/s);
}

inline TVector3<float> TVector3<float>::operator/ (float s) const
{
	_ASSERTE(s != 0.0f);
	s = 1.0f / s;
	return TVector3<float>(x*s, y*s, z*s);
}

inline TVector3<double> TVector3<double>::operator/ (double s) const
{
	_ASSERTE(s != 0.0);
	s = 1.0 / s;
	return TVector3<double>(x*s, y*s, z*s);
}

///// INLINE FUNCTIONS /////

template <typename T>
inline bool TVector3<T>::equalTo(const TVector3<T> &p, const T epsilon) const
{
	if (math.abs<T>(p.x - x) <= epsilon)
		if (math.abs<T>(p.y - y) <= epsilon)
			if (math.abs<T>(p.z - z) <= epsilon)
				return true;
	return false;
}

template <typename T>
inline bool TVector3<T>::notEqualTo(const TVector3<T> &p, const T epsilon) const
{
	if ((math.abs<T>(p.x-x) > epsilon) || (math.abs<T>(p.y-y) > epsilon) || (math.abs<T>(p.z-z) > epsilon)) return true;
	else return false;
}

template <typename T>
inline void TVector3<T>::add(const TVector3<T> &p1,const TVector3<T> &p2)
{
	x = p1.x + p2.x; y = p1.y + p2.y; z = p1.z + p2.z;
}

template <typename T>
inline TVector3<T> & TVector3<T>::add(const TVector3<T> &p)
{
	x += p.x; y += p.y; z += p.z;
	return *this;
}

template <typename T>
inline void TVector3<T>::subtract(const TVector3<T> &p1,const TVector3<T> &p2)
{
	x = p1.x - p2.x; y = p1.y - p2.y; z = p1.z - p2.z;
}

template <typename T>
inline TVector3<T> & TVector3<T>::subtract(const TVector3<T> &p)
{
	x -= p.x; y -= p.y; z -= p.z;
	return *this;
}

template <typename T>
inline void TVector3<T>::cross(const TVector3<T> &p1, const TVector3<T> &p2)
{
	x = (p1.y * p2.z) - (p1.z * p2.y);
	y = (p1.z * p2.x) - (p1.x * p2.z);
	z = (p1.x * p2.y) - (p1.y * p2.x);
}

template <typename T>
inline TVector3<T> & TVector3<T>::cross(const TVector3<T> &p)
{
	assign(	y * p.z - z * p.y,
			z * p.x - x * p.z,
			x * p.y - y * p.x);
	return *this;
}

template <typename T>
inline void TVector3<T>::lerp(const TVector3<T> &p1, const TVector3<T> &p2, const T t)
{
	x = FastMath::lerp(p1.x, p2.x, t);
	y = FastMath::lerp(p1.y, p2.y, t);
	z = FastMath::lerp(p1.z, p2.z, t);
}

template <typename T>
inline TVector3<T> & TVector3<T>::lerp(const TVector3<T> &p, const T t)
{
	assign(	FastMath::lerp(x, p.x, t),
			FastMath::lerp(y, p.y, t),
			FastMath::lerp(z, p.z, t));
	return *this;
}

template <typename T>
inline void TVector3<T>::add(const TVector3<T> &p, const T s)
{
	x = p.x + s; y = p.y + s; z = p.z + s;
}

template <typename T>
inline TVector3<T> & TVector3<T>::add(const T s)
{
	x += s; y += s; z += s;
	return *this;
}

template <typename T>
inline void TVector3<T>::subtract(const TVector3<T> &p, const T s)
{
	x = p.x - s; y = p.y - s; z = p.z - s;
}

template <typename T>
inline TVector3<T> & TVector3<T>::subtract(const T s)
{
	x -= s; y -= s; z -= s;
	return *this;
}

template <typename T>
inline void TVector3<T>::multiply(const TVector3<T> &p, const T s)
{
	x = p.x * s; y = p.y * s; z = p.z * s;
}

template <typename T>
inline TVector3<T> & TVector3<T>::multiply(const T s)
{
	x *= s; y *= s; z *= s;
	return *this;
}

template <typename T>
inline void TVector3<T>::divide(const TVector3<T> &p, T s)
{
	_ASSERTE(s != 0);
	x = p.x / s; y = p.y / s; z = p.z / s;
}

template <typename T>
inline TVector3<T> & TVector3<T>::divide(T s)
{
	_ASSERTE(s != 0.0f);
	x /= s; y /= s; z /= s;
	return *this;
}

inline void TVector3<float>::divide(const TVector3<float> &p, float s)
{
	_ASSERTE(s != 0.0f);
	s = 1.0f / s;
	x = p.x * s; y = p.y * s; z = p.z * s;
}

inline TVector3<float> & TVector3<float>::divide(float s)
{
	_ASSERTE(s != 0.0f);
	s = 1.0f / s;
	x *= s; y *= s; z *= s;
	return *this;
}

inline void TVector3<double>::divide(const TVector3<double> &p, double s)
{
	_ASSERTE(s != 0.0);
	s = 1.0 / s;
	x = p.x * s; y = p.y * s; z = p.z * s;
}

inline TVector3<double> & TVector3<double>::divide(double s)
{
	_ASSERTE(s != 0.0);
	s = 1.0 / s;
	x *= s; y *= s; z *= s;
	return *this;
}

template <typename T>
inline T TVector3<T>::distance(const TVector3<T> &p) const
{
	const T dx = p.x - x;
	const T dy = p.y - y;
	const T dz = p.z - z;
	return static_cast<T>(sqrtf(dx*dx + dy*dy + dz*dz));
}

inline float TVector3<float>::distance(const TVector3<float> &p) const
{
	const float dx = p.x - x;
	const float dy = p.y - y;
	const float dz = p.z - z;
	return sqrtf(dx*dx + dy*dy + dz*dz);
}

inline double TVector3<double>::distance(const TVector3<double> &p) const
{
	const double dx = p.x - x;
	const double dy = p.y - y;
	const double dz = p.z - z;
	return sqrt(dx*dx + dy*dy + dz*dz);
}

inline int TVector3<int>::distance(const TVector3<int> &p) const
{
	const int dx = p.x - x;
	const int dy = p.y - y;
	const int dz = p.z - z;
	return FastMath::sqrti(dx*dx + dy*dy + dz*dz);
}

template <typename T>
inline T TVector3<T>::distSquared(const TVector3<T> &p) const
{
	const T dx = p.x - x;
	const T dy = p.y - y;
	const T dz = p.z - z;
	return dx*dx + dy*dy + dz*dz;
}

template <typename T>
inline T TVector3<T>::magnitude() const
{
	return static_cast<T>(sqrt(x*x + y*y + z*z));
}

inline float TVector3<float>::magnitude() const
{
	return sqrtf(x*x + y*y + z*z);
}

inline double TVector3<double>::magnitude() const
{
	return sqrt(x*x + y*y + z*z);
}

inline int TVector3<int>::magnitude() const
{
	return FastMath::sqrti(x*x + y*y + z*z);
}

template <typename T>
inline void TVector3<T>::unitNormalOf(const TVector3<T> &p1, const TVector3<T> &p2)
{
	// Calculates the normal vector with cross product
	cross(p1, p2);
	// Normalizes the vector
	normalize();
}

template <typename T>
inline void TVector3<T>::normalize()
{
	// Normalize the vector to unit length
	T mag = magnitude();
	_ASSERTE(mag != 0);
	x /= mag; y /= mag; z /= mag;
}

inline void TVector3<float>::normalize()
{
	float mag = magnitude();
	_ASSERTE(mag != 0.0f);
	float invMag = 1.0f / mag;
	x *= invMag; y *= invMag; z *= invMag;
}

inline void TVector3<double>::normalize()
{
	double mag = magnitude();
	_ASSERTE(mag != 0.0);
	double invMag = 1.0 / mag;
	x *= invMag; y *= invMag; z *= invMag;
}

template <typename T>
inline float TVector3<T>::angleRad(const TVector3<T> &p) const
{
	return acosf(static_cast<float>(dot(p) / (magnitude() * p.magnitude())));
}

template <typename T>
inline float TVector3<T>::angleDeg(const TVector3<T> &p) const
{
	return angleRad(p) * RADTODEGf;
}

template <typename T>
inline float TVector3<T>::angleRadUnit(const TVector3<T> &p) const
{
	return acosf(static_cast<float>(dot(p)));
}

template <typename T>
inline float TVector3<T>::angleDegUnit(const TVector3<T> &p) const
{
	return angleRadUnit(p) * RADTODEGf;
}

template <typename T>
inline void TVector3<T>::rotate(TVector3<T> &p, uint xa, uint ya, uint za)
{
	_ASSERTE(xa < math.ANGLE360);
	_ASSERTE(ya < math.ANGLE360);
	_ASSERTE(za < math.ANGLE360);

	*this = p;
	TVector3<T> tp;
	
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
inline void TVector3<T>::rotate(uint xa, uint ya, uint za)
{
	_ASSERTE(xa < math.ANGLE360);
	_ASSERTE(ya < math.ANGLE360);
	_ASSERTE(za < math.ANGLE360);

	TVector3<T> tp;
	
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
inline string TVector3<T>::toString() const
{
	ostringstream returnStr;
	returnStr << "(" << x << "," << y << "," << z << ")";
	return returnStr.str();
}
