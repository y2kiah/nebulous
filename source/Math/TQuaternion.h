/*----==== QUATERNION.H ====----
	Author:	Jeffrey Kiah
	Date:	2/25/2010
Notes:
	A unit quaternion represents a rotation 2*acos(w) about the (x,y,z) axis
------------------------------*/

#pragma once

#include <sstream>
#include "TVector3.h"
#include "FastMath.h"

template <typename T>
class TQuaternion {
public:
	///// VARIABLES /////
	union {
		struct {
			T w, x, y, z;
		};
		struct {
			T w;
			TVector3<T> v;
		};
		T q[4];
	};

	///// OPERATORS /////
	// boolean operators
	inline bool	operator==(const TQuaternion<T> &q) const;
	bool		operator!=(const TQuaternion<T> &q) const { return !(*this==q); }
	// assignment operators
	void		operator+=(const TQuaternion<T> &q)	{ add(q); }
	void		operator-=(const TQuaternion<T> &q)	{ subtract(q); }
	void		operator*=(const TQuaternion<T> &q)	{ multiply(q); }
	void		operator*=(const T s)				{ multiply(s); }
	void		operator/=(T s)						{ divide(s); }
	// copy operators
	TQuaternion<T>	operator* (const T s) const	{ return TQuaternion<T>(w*s,x*s,y*s,z*s); }

	///// FUNCTIONS /////
	void		assign(const T _w, const T _x, const T _y, const T _z)	{ w = _w; x = _x; y = _y; z = _z; }
	void		assign(const TQuaternion<T> &q)				{ w = q.w; x = q.x; y = q.y; z = q.z; }
	void		assign(const T _w, const TVector3<T> &_v)	{ w = _w; v.assign(_v); }
	// set quaternion to identity
	void		identity()						{ assign(1,0,0,0); }
	// calculate conjugate of quaternion
	void		conjugate()						{ x = -x; y = -y; z = -z; }
	// normalize quaternion
	inline void normalize();
	// calculate inverse of quaternion and store result in parameter
	inline void	inverse(TQuaternion<T> &result) const;

	// dot product of two quaternions
	T dot(const TQuaternion<T> &q) const	{ return w*q.w + x*q.x + y*q.y + z*q.z; }
	// calculate length of quaternion
	T length() const						{ return sqrt(w*w + x*x + y*y + z*z); }
	// calculate norm of quaternion
	T norm() const							{ return w*w + x*x + y*y + z*z; }

	// add another quaternion to this quaternion
	TQuaternion<T> &		add(const TQuaternion<T> &q) { w += q.w; x += q.x; y += q.y; z += q.z; return *this; }
	// subtract another quaternion from this quaternion
	TQuaternion<T> &		subtract(const TQuaternion<T> &q) { w -= q.w; x -= q.x; y -= q.y; z -= q.z; return *this; }
	// multiply this quaternion with another quaternion
	inline TQuaternion<T> & multiply(const TQuaternion<T> &q);
	// multiply this quaternion by a scalar
	TQuaternion<T> &		multiply(const T s)	{ w *= s; v *= s; return *this; }
	// divide this quaternion by a scalar
	TQuaternion<T> &		divide(const T s)	{ _ASSERTE(s != 0); multiply(1.0f / s); return *this; }
	
	// multiply two quaternions, store results
	inline TQuaternion<T> & multiply(const TQuaternion<T> &q0, const TQuaternion<T> &q1);
	inline TQuaternion<T> & nlerp(const TQuaternion<T> &q0, const TQuaternion<T> &q1, const T t);
	inline TQuaternion<T> & slerp(const TQuaternion<T> &q0, const TQuaternion<T> &q1, const T t);

	// convert quaternion to angle-axis
	inline void angleAxis(T &angle, TVector3<T> &axis) const;
	// convert quaternion to matrix
	/*inline void matrix(Matrix &m) const; */

	// Debugging
	inline string	toString() const;

	// Constructors
	TQuaternion() : w(0), x(0), y(0), z(0) {}
	// copy constructor
	TQuaternion(const TQuaternion<T> &q) : w(q.w), x(q.x), y(q.y), z(q.z) {}
	// construct quaternion from real component w and imaginary x,y,z
	explicit TQuaternion(T _w, T _x, T _y, T _z) : w(_w), x(_x), y(_y), z(_z) {}

	// construct quaternion from angle-axis
	explicit TQuaternion(T angleRad, const TVector3<T> &axis) :
		v(axis)
	{
		angleRad *= 0.5f;
		w = (T)cos(angleRad);
		v *= (T)sin(angleRad);
	}

	// construct quaternion from rotation matrix
	/*Quaternion(const Matrix &matrix)
	{
		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "Quaternion Calculus and Fast Animation".

		const float trace = matrix.m11 + matrix.m22 + matrix.m33;

		if (trace>0)
		{
			// |w| > 1/2, may as well choose w > 1/2

			float root = sqrt(trace + 1.0f);  // 2w
			w = 0.5f * root;
			root = 0.5f / root;  // 1/(4w)
			x = (matrix.m32-matrix.m23) * root;
			y = (matrix.m13-matrix.m31) * root;
			z = (matrix.m21-matrix.m12) * root;
		}
		else
		{
			// |w| <= 1/2

			static int next[3] = { 2, 3, 1 };
			
			int i = 1;
			if (matrix.m22>matrix.m11)  i = 2;
			if (matrix.m33>matrix(i,i)) i = 3;
			int j = next[i];
			int k = next[j];
			
			float root = sqrt(matrix(i,i)-matrix(j,j)-matrix(k,k) + 1.0f);
			float *quaternion[3] = { &x, &y, &z };
			*quaternion[i] = 0.5f * root;
			root = 0.5f / root;
			w = (matrix(k,j)-matrix(j,k))*root;
			*quaternion[j] = (matrix(j,i)+matrix(i,j))*root;
			*quaternion[k] = (matrix(k,i)+matrix(i,k))*root;
		}
	}*/
};

///// DEFINES /////

typedef TQuaternion<float>		Quaternionf;
typedef TQuaternion<double>		Quaterniond;

///// INLINE OPERATORS /////

template <typename T>
inline bool TQuaternion<T>::operator==(const TQuaternion<T> &q) const
{
	//if (equal(w,other.w) && equal(x,other.x) && equal(y,other.y) && equal(z,other.z)) {
		return true;
	//}
	//return false;
}

///// INLINE FUNCTIONS /////

template <typename T>
inline void TQuaternion<T>::normalize()
{
	T len = length();
	if (len == 0) { identity(); return; }
	T invLen = 1.0f / len;
	multiply(invLen);
}
// calculate inverse of quaternion and store result in parameter
template <typename T>
inline void TQuaternion<T>::inverse(TQuaternion<T> &result) const
{
	T n = norm(); _ASSERTE(n != 0);
	invNorm = (t)1.0 / n;
	assign(w*invNorm, -x*invNorm, -y*invNorm, -z*invNorm);
}

// multiply this quaternion with another quaternion
template <typename T>
inline TQuaternion<T> & TQuaternion<T>::multiply(const TQuaternion<T> &q)
{
	const T rw = w*q.w - x*q.x - y*q.y - z*q.z;
	const T rx = w*q.x + x*q.w + y*q.z - z*q.y;
	const T ry = w*q.y - x*q.z + y*q.w + z*q.x;
	const T rz = w*q.z + x*q.y - y*q.x + z*q.w;
	assign(rw, rx, ry, rz);
	return *this;
}

// multiply two quaternions, store results
template <typename T>
inline TQuaternion<T> & TQuaternion<T>::multiply(const TQuaternion<T> &q0, const TQuaternion<T> &q1)
{
	w = q0.w*q1.w - q0.x*q1.x - q0.y*q1.y - q0.z*q1.z;
	x = q0.w*q1.x + q0.x*q1.w + q0.y*q1.z - q0.z*q1.y;
	y = q0.w*q1.y - q0.x*q1.z + q0.y*q1.w + q0.z*q1.x;
	z = q0.w*q1.z + q0.x*q1.y - q0.y*q1.x + q0.z*q1.w;
	return *this;
}

template <typename T>
inline TQuaternion<T> & TQuaternion<T>::nlerp(const TQuaternion<T> &q0, const TQuaternion<T> &q1, const T t)
{
	// normalize(q0 + (q1 - q0)*t)
	w = q0.w + (q1.w - q0.w)*t;
	x = q0.x + (q1.x - q0.x)*t;
	y = q0.y + (q1.y - q0.y)*t;
	z = q0.z + (q1.z - q0.z)*t;
	normalize();
	return *this;
}

template <typename T>
inline TQuaternion<T> & TQuaternion<T>::slerp(const TQuaternion<T> &q0, const TQuaternion<T> &q1, const T t)
{
	_ASSERTE(t >= 0); _ASSERTE(t <= 1);

	T cosine = q0.dot(q1);
	FastMath::clamp((T)-1.0, (T)1.0, cosine);	// stay within domain of acos

	if ((T)1.0-FastMath::abs(cosine) < std::numeric_limits<T>::epsilon()) {
		// if q0 and q1 are close to aligned, just nlerp them
		nlerp(q0, q1, t);
	} else {
		// perform the slerp
		T theta = acos(cosine);
		T tTheta = t*theta;
		T sine = sin(theta);
		T invSin = (T)1.0/sine;
		T coeff0 = sin(theta - tTheta) * invSin;
		T coeff1 = sin(tTheta) * invSin;

		w = coeff0*q0.w + coeff1*q1.w;
		x = coeff0*q0.x + coeff1*q1.x;
		y = coeff0*q0.y + coeff1*q1.y;
		z = coeff0*q0.z + coeff1*q1.z;
	}
	return *this;
}

template <typename T>
inline void TQuaternion<T>::angleAxis(T &angle, TVector3<T> &axis) const
{
	const T squareLength = v.magSquared();
	if (squareLength > 0) {
		angle = 2 * (T)acos(w);
		const T inverseLength = (T)1.0 / (T)sqrt(squareLength);
		axis.assign(x*inverseLength, y*inverseLength, z*inverseLength);
	} else {
		angle = 0;
		axis.assign(1, 0, 0);
	}
}

/*
template <typename T>
inline void TQuaternion<T>::matrix(Matrix &m) const
{
	// from david eberly's sources used with permission
	float fTx  = 2.0f*x;
	float fTy  = 2.0f*y;
	float fTz  = 2.0f*z;
	float fTwx = fTx*w;
	float fTwy = fTy*w;
	float fTwz = fTz*w;
	float fTxx = fTx*x;
	float fTxy = fTy*x;
	float fTxz = fTz*x;
	float fTyy = fTy*y;
	float fTyz = fTz*y;
	float fTzz = fTz*z;

	return Matrix(1.0f-(fTyy+fTzz), fTxy-fTwz, fTxz+fTwy,
				  fTxy+fTwz, 1.0f-(fTxx+fTzz), fTyz-fTwx,
				  fTxz-fTwy, fTyz+fTwx, 1.0f-(fTxx+fTyy));
}*/

template <typename T>
inline string TQuaternion<T>::toString() const
{
	ostringstream returnStr;
	returnStr << "(" << w << "," << x << "," << y << "," << z << ")";
	return returnStr.str();
}