/*----==== TMATRIX4X4.H ====----
	Author:		Jeff Kiah
	Orig.Date:	07/08/2009
	Rev.Date:	03/09/2010
------------------------------*/

#pragma once

#include "FastMath.h"
#include "TVector3.h"
#include "TVector4.h"

///// STRUCTURES /////

struct D3DXMATRIX;

/*=============================================================================
class TMatrix4x4
=============================================================================*/
template <typename T>
class TMatrix4x4 {
	public:
		///// VARIABLES /////
		union {
			struct {
				T	p11,p12,p13,p14,p21,p22,p23,p24,
					p31,p32,p33,p34,p41,p42,p43,p44;
			};
			T p[16];
			T p2[4][4];
		};

		///// OPERATORS /////
		T &				operator[](const int n) { return p[n]; }
		const T			operator[](const int n) const { return p[n]; }
		T &				operator()(const int i, const int j) { return p2[i][j]; }
		const T			operator()(const int i, const int j) const { return p2[i][j]; }
		inline void		operator= (const TMatrix4x4<T> &m);
		inline void		operator*=(const TMatrix4x4<T> &m);
		inline void		operator*=(const TVector4<T> &v);

		///// FUNCTIONS /////
		// Manipulate the matrix
		inline void		multiply(const TMatrix4x4<T> &m1, const TMatrix4x4<T> &m2);
		inline void		translate(T x, T y, T z);
		inline void		scale(T x, T y, T z);
		inline void		rotateX(T a);
		inline void		rotateY(T a);
		inline void		rotateZ(T a);
		inline void		rotateX(uint a);
		inline void		rotateY(uint a);
		inline void		rotateZ(uint a);
		inline void		transpose();

		// Matrix operations
		inline void		set(const TMatrix4x4<T> &m);
		inline void		set(const TVector4<T> &r1, const TVector4<T> &r2,
							const TVector4<T> &r3, const TVector4<T> &r4);
		inline void		setIdentity();
		inline void		setTranslation(T x, T y, T z);
		inline void		setScaling(T x, T y, T z);
		void			setRotation(T x, T y, T z); // floating point angles, uses trig calculations, SLOW
		void			setRotation(uint x, uint y, uint z); // integer angles used with LookupManager, uses trig tables, FAST
		void			setTranspose(const TMatrix4x4<T> &m);

		// Set to a projection matrix
		inline void		setPerspectiveProjectionFovLH(T zNear, T zFar, T fovYDeg, T aspectRatio);
		inline void		setPerspectiveProjectionFovRH(T zNear, T zFar, T fovYDeg, T aspectRatio);
		inline void		setPerspectiveProjectionLH(T zNear, T zFar, T nearWidth, T nearHeight);
		inline void		setPerspectiveProjectionRH(T zNear, T zFar, T nearWidth, T nearHeight);

		// Set to a view matrix
		inline void		setViewMatrix(const TVector3<T> &from, const TVector3<T> &view,
									  const TVector3<T> &up, const TVector3<T> &right);
		inline void		setViewMatrix(const TVector3<T> &from, const TVector3<T> &lookAt, const TVector3<T> &up);
		
		// Casting
		inline D3DXMATRIX *			toD3DXMATRIX();
		inline const D3DXMATRIX *	toD3DXMATRIX() const;

		// Constructors / Destructor
		TMatrix4x4() {}
		TMatrix4x4(const TMatrix4x4<T> &m);
		explicit TMatrix4x4(const TVector4<T> &r1, const TVector4<T> &r2,
							const TVector4<T> &r3, const TVector4<T> &r4);
		explicit TMatrix4x4(const T *v, int length);
		~TMatrix4x4() {}
};

///// DEFINES /////

typedef TMatrix4x4<float>	Matrix4x4f;
typedef TMatrix4x4<double>	Matrix4x4d;

///// INLINE FUNCTIONS /////

template <typename T>
inline void TMatrix4x4<T>::operator= (const TMatrix4x4<T> &m)
{
	memcpy_s(p, sizeof(p), m.p, sizeof(T)*16);
}

template <typename T>
inline void TMatrix4x4<T>::operator*=(const TMatrix4x4<T> &m)
{
	TMatrix4x4<T> orig(*this);

	// row 1
	p[0]  = orig.p[0]*m.p[0] + orig.p[1]*m.p[4] + orig.p[2]*m.p[8]  + orig.p[3]*m.p[12];
	p[1]  = orig.p[0]*m.p[1] + orig.p[1]*m.p[5] + orig.p[2]*m.p[9]  + orig.p[3]*m.p[13];
	p[2]  = orig.p[0]*m.p[2] + orig.p[1]*m.p[6] + orig.p[2]*m.p[10] + orig.p[3]*m.p[14];
	p[3]  = orig.p[0]*m.p[3] + orig.p[1]*m.p[7] + orig.p[2]*m.p[11] + orig.p[3]*m.p[15];

	// row 2
	p[4]  = orig.p[4]*m.p[0] + orig.p[5]*m.p[4] + orig.p[6]*m.p[8]  + orig.p[7]*m.p[12];
	p[5]  = orig.p[4]*m.p[1] + orig.p[5]*m.p[5] + orig.p[6]*m.p[9]  + orig.p[7]*m.p[13];
	p[6]  = orig.p[4]*m.p[2] + orig.p[5]*m.p[6] + orig.p[6]*m.p[10] + orig.p[7]*m.p[14];
	p[7]  = orig.p[4]*m.p[3] + orig.p[5]*m.p[7] + orig.p[6]*m.p[11] + orig.p[7]*m.p[15];

	// row 3
	p[8]  = orig.p[8]*m.p[0] + orig.p[9]*m.p[4] + orig.p[10]*m.p[8]  + orig.p[11]*m.p[12];
	p[9]  = orig.p[8]*m.p[1] + orig.p[9]*m.p[5] + orig.p[10]*m.p[9]  + orig.p[11]*m.p[13];
	p[10] = orig.p[8]*m.p[2] + orig.p[9]*m.p[6] + orig.p[10]*m.p[10] + orig.p[11]*m.p[14];
	p[11] = orig.p[8]*m.p[3] + orig.p[9]*m.p[7] + orig.p[10]*m.p[11] + orig.p[11]*m.p[15];

	// row 4
	p[12] = orig.p[12]*m.p[0] + orig.p[13]*m.p[4] + orig.p[14]*m.p[8]  + orig.p[15]*m.p[12];
	p[13] = orig.p[12]*m.p[1] + orig.p[13]*m.p[5] + orig.p[14]*m.p[9]  + orig.p[15]*m.p[13];
	p[14] = orig.p[12]*m.p[2] + orig.p[13]*m.p[6] + orig.p[14]*m.p[10] + orig.p[15]*m.p[14];
	p[15] = orig.p[12]*m.p[3] + orig.p[13]*m.p[7] + orig.p[14]*m.p[11] + orig.p[15]*m.p[15];
}

// post multiply a column vector
template <typename T>
inline void TMatrix4x4<T>::operator*=(const TVector4<T> &v)
{
	p[0]  *= v.x; p[1]  *= v.y; p[2]  *= v.z; p[3]  *= v.w;
	p[4]  *= v.x; p[5]  *= v.y; p[6]  *= v.z; p[7]  *= v.w;
	p[8]  *= v.x; p[9]  *= v.y; p[10] *= v.z; p[11] *= v.w;
	p[12] *= v.x; p[13] *= v.y; p[14] *= v.z; p[15] *= v.w;
}

template <typename T>
inline void TMatrix4x4<T>::multiply(const TMatrix4x4<T> &m1, const TMatrix4x4<T> &m2)
{
	// row 1
	p[0]  = m1.p[0]*m2.p[0] + m1.p[1]*m2.p[4] + m1.p[2]*m2.p[8]  + m1.p[3]*m2.p[12];
	p[1]  = m1.p[0]*m2.p[1] + m1.p[1]*m2.p[5] + m1.p[2]*m2.p[9]  + m1.p[3]*m2.p[13];
	p[2]  = m1.p[0]*m2.p[2] + m1.p[1]*m2.p[6] + m1.p[2]*m2.p[10] + m1.p[3]*m2.p[14];
	p[3]  = m1.p[0]*m2.p[3] + m1.p[1]*m2.p[7] + m1.p[2]*m2.p[11] + m1.p[3]*m2.p[15];

	// row 2
	p[4]  = m1.p[4]*m2.p[0] + m1.p[5]*m2.p[4] + m1.p[6]*m2.p[8]  + m1.p[7]*m2.p[12];
	p[5]  = m1.p[4]*m2.p[1] + m1.p[5]*m2.p[5] + m1.p[6]*m2.p[9]  + m1.p[7]*m2.p[13];
	p[6]  = m1.p[4]*m2.p[2] + m1.p[5]*m2.p[6] + m1.p[6]*m2.p[10] + m1.p[7]*m2.p[14];
	p[7]  = m1.p[4]*m2.p[3] + m1.p[5]*m2.p[7] + m1.p[6]*m2.p[11] + m1.p[7]*m2.p[15];

	// row 3
	p[8]  = m1.p[8]*m2.p[0] + m1.p[9]*m2.p[4] + m1.p[10]*m2.p[8]  + m1.p[11]*m2.p[12];
	p[9]  = m1.p[8]*m2.p[1] + m1.p[9]*m2.p[5] + m1.p[10]*m2.p[9]  + m1.p[11]*m2.p[13];
	p[10] = m1.p[8]*m2.p[2] + m1.p[9]*m2.p[6] + m1.p[10]*m2.p[10] + m1.p[11]*m2.p[14];
	p[11] = m1.p[8]*m2.p[3] + m1.p[9]*m2.p[7] + m1.p[10]*m2.p[11] + m1.p[11]*m2.p[15];

	// row 4
	p[12] = m1.p[12]*m2.p[0] + m1.p[13]*m2.p[4] + m1.p[14]*m2.p[8]  + m1.p[15]*m2.p[12];
	p[13] = m1.p[12]*m2.p[1] + m1.p[13]*m2.p[5] + m1.p[14]*m2.p[9]  + m1.p[15]*m2.p[13];
	p[14] = m1.p[12]*m2.p[2] + m1.p[13]*m2.p[6] + m1.p[14]*m2.p[10] + m1.p[15]*m2.p[14];
	p[15] = m1.p[12]*m2.p[3] + m1.p[13]*m2.p[7] + m1.p[14]*m2.p[11] + m1.p[15]*m2.p[15];
}

template <typename T>
void TMatrix4x4<T>::setRotation(T h, T p, T b)
{
	T sh = sin(h); T ch = cos(h);
	T sp = sin(p); T cp = cos(p);
	T sb = sin(b); T cb = cos(b);
	
	p[0] = ch * cb + sh * sp * sb;
	p[1] = -ch * sb + sh * sp * cb;
	p[2] = sh * cp;

	p[4] = sb * cp;
	p[5] = cb * cp;
	p[6] = -sp;

	p[8] = -sh * cb + ch * sp * sb;
	p[9] = sb * sh + ch * sp * cb;
	p[10] = ch * cp;

	p[12] = p[13] = p[14] = 0;
}

template <typename T>
inline void TMatrix4x4<T>::rotateX(T a)
{
	p[5] = cos(a);
	p[6] = sin(a);
	p[9] = -p[6];	
	p[10] = p[5];
}

template <typename T>
inline void TMatrix4x4<T>::rotateY(T a)
{
	p[0] = cos(a);
	p[8] = sin(a);
	p[2] = -p[8];	
	p[10] = p[0];
}

template <typename T>
inline void TMatrix4x4<T>::rotateZ(T a)
{
	p[0] = cos(a);
	p[1] = sin(a);
	p[4] = -p[1];	
	p[5] = p[0];
}

template <typename T>
inline void TMatrix4x4<T>::rotateX(uint a)
{
	_ASSERTE(a < math.ANGLE360);

	p[5] = (T)math.getCos(a);
	p[6] = (T)math.getSin(a);
	p[9] = -p[6];	
	p[10] = p[5];
}

template <typename T>
inline void TMatrix4x4<T>::rotateY(uint a)
{
	_ASSERTE(a < math.ANGLE360);

	p[0] = (T)math.getCos(a);
	p[8] = (T)math.getSin(a);
	p[2] = -p[8];	
	p[10] = p[0];
}

template <typename T>
inline void TMatrix4x4<T>::rotateZ(uint a)
{
	_ASSERTE(a < math.ANGLE360);

	p[0] = (T)math.getCos(a);
	p[1] = (T)math.getSin(a);
	p[4] = -p[1];	
	p[5] = p[0];
}

template <typename T>
inline void TMatrix4x4<T>::set(const TMatrix4x4<T> &m)
{
	memcpy_s(p, sizeof(p), m.p, sizeof(T)*16);
}

template <typename T>
inline void TMatrix4x4<T>::set(	const TVector4<T> &r1, const TVector4<T> &r2,
								const TVector4<T> &r3, const TVector4<T> &r4)
{
	p[0]  = r1.x; p[1]  = r1.y; p[2]  = r1.z; p[3]  = r1.w;
	p[4]  = r2.x; p[5]  = r2.y; p[6]  = r2.z; p[7]  = r2.w;
	p[8]  = r3.x; p[9]  = r3.y; p[10] = r3.z; p[11] = r3.w;
	p[12] = r4.x; p[13] = r4.y; p[14] = r4.z; p[15] = r4.w;
}

// Set matrix to the identity matrix
template <typename T>
inline void TMatrix4x4<T>::setIdentity()
{
	p[0]  = 1; p[1]  = 0; p[2]  = 0; p[3]  = 0;
	p[4]  = 0; p[5]  = 1; p[6]  = 0; p[7]  = 0;
	p[8]  = 0; p[9]  = 0; p[10] = 1; p[11] = 0;
	p[12] = 0; p[13] = 0; p[14] = 0; p[15] = 1;
}

// Set matrix to a translation matrix
template <typename T>
inline void TMatrix4x4<T>::setTranslation(T x, T y, T z)
{
	p[0]  = 1; p[1]  = 0; p[2]  = 0; p[3]  = 0;
	p[4]  = 0; p[5]  = 1; p[6]  = 0; p[7]  = 0;
	p[8]  = 0; p[9]  = 0; p[10] = 1; p[11] = 0;
	p[12] = x; p[13] = y; p[14] = z; p[15] = 1;
}

// Set matrix to a scaling matrix
template <typename T>
inline void TMatrix4x4<T>::setScaling(T x, T y, T z)
{
	p[0]  = x; p[1]  = 0; p[2]  = 0; p[3]  = 0;
	p[4]  = 0; p[5]  = y; p[6]  = 0; p[7]  = 0;
	p[8]  = 0; p[9]  = 0; p[10] = z; p[11] = 0;
	p[12] = 0; p[13] = 0; p[14] = 0; p[15] = 1;
}

template <typename T>
void TMatrix4x4<T>::setRotation(uint h, uint p, uint b)
{
	_ASSERTE(h < math.ANGLE360);
	_ASSERTE(p < math.ANGLE360);
	_ASSERTE(b < math.ANGLE360);

	T sh = (T)math.getSin(h); T ch = (T)math.getCos(h);
	T sp = (T)math.getSin(p); T cp = (T)math.getCos(p);
	T sb = (T)math.getSin(b); T cb = (T)math.getCos(b);
	
	p[0] = ch * cb + sh * sp * sb;
	p[1] = -ch * sb + sh * sp * cb;
	p[2] = sh * cp;

	p[4] = sb * cp;
	p[5] = cb * cp;
	p[6] = -sp;

	p[8] = -sh * cb + ch * sp * sb;
	p[9] = sb * sh + ch * sp * cb;
	p[10] = ch * cp;

	p[12] = p[13] = p[14] = 0;
}

// Set to transpose of matrix, usually used for giving to OpenGL
template <typename T>
void TMatrix4x4<T>::setTranspose(const TMatrix4x4<T> &m)
{
	p[0]  = m.p[0]; p[1]  = m.p[4]; p[2]  = m.p[8];  p[3]  = m.p[12];
	p[4]  = m.p[1]; p[5]  = m.p[5]; p[6]  = m.p[9];  p[7]  = m.p[13];
	p[8]  = m.p[2]; p[9]  = m.p[6]; p[10] = m.p[10]; p[11] = m.p[14];
	p[12] = m.p[3]; p[13] = m.p[7]; p[14] = m.p[11]; p[15] = m.p[15];
}

// Set translation portion of matrix, do not touch rest. This can be called after any other
// set function, but the translations are not additive
template <typename T>
inline void TMatrix4x4<T>::translate(T x, T y, T z)
{
	p[12] = x;
	p[13] = y;
	p[14] = z;
}

// Set scaling portion of matrix, do not touch rest
template <typename T>
inline void TMatrix4x4<T>::scale(T x, T y, T z)
{
	p[0] = x;
	p[5] = y;
	p[10] = z;
}


// Set current matrix to its transpose
template <typename T>
void TMatrix4x4<T>::transpose()
{
	T temp = p[1];
	p[1] = p[4];
	p[4] = temp;

	temp = p[2];
	p[2] = p[8];
	p[8] = temp;

	temp = p[6];
	p[6] = p[9];
	p[9] = temp;

	temp = p[3];
	p[3] = p[12];
	p[12] = temp;

	temp = p[7];
	p[7] = p[13];
	p[13] = temp;

	temp = p[11];
	p[11] = p[14];
	p[14] = temp;
}

/*---------------------------------------------------------------------
xScale	    0           0           0
0         yScale        0           0
0           0        zf/(zf-zn)     1
0           0       -zn*zf/(zf-zn)  0
where:
yScale = cot(fovY/2)
xScale = yScale / aspect ratio
---------------------------------------------------------------------*/
template <typename T>
inline void TMatrix4x4<T>::setPerspectiveProjectionFovLH(T zNear, T zFar, T fovYDeg, T aspectRatio)
{
	T yScale = (T)1.0 / tan(fovYDeg * (T)0.5 * (T)DEGTORAD);
	T q = zFar / (zFar - zNear);
	p12 = p13 = p14 = p21 = p23 = p24 = p31 = p32 = p41 = p42 = p44 = 0;
	p11 = yScale / aspectRatio;
	p22 = yScale;
	p33 = q;
	p34 = (T)1.0;
	p43 = -zNear * q;
}

/*---------------------------------------------------------------------
xScale      0           0           0
0         yScale        0           0
0           0        zf/(zn-zf)    -1
0           0        zn*zf/(zn-zf)  0
where:
yScale = cot(fovY/2)
xScale = yScale / aspect ratio
---------------------------------------------------------------------*/
template <typename T>
inline void TMatrix4x4<T>::setPerspectiveProjectionFovRH(T zNear, T zFar, T fovYDeg, T aspectRatio)
{
	T yScale = (T)1.0 / tan(fovYDeg * (T)0.5 * (T)DEGTORAD);
	T q = zFar / (zNear - zFar);
	p12 = p13 = p14 = p21 = p23 = p24 = p31 = p32 = p41 = p42 = p44 = 0;
	p11 = yScale / aspectRatio;
	p22 = yScale;
	p33 = q;
	p34 = (T)-1.0;
	p43 = zNear * q;
}

/*---------------------------------------------------------------------
2*zn/w      0           0           0
0        2*zn/h         0           0
0           0        zf/(zf-zn)     1
0           0       -zn*zf/(zf-zn)  0
---------------------------------------------------------------------*/
template <typename T>
inline void TMatrix4x4<T>::setPerspectiveProjectionLH(T zNear, T zFar, T nearWidth, T nearHeight)
{
	T zNearX2 = zNear * (T)2.0;
	T q = zFar / (zFar - zNear);
	p12 = p13 = p14 = p21 = p23 = p24 = p31 = p32 = p41 = p42 = p44 = 0;
	p11 = zNearX2 / nearWidth;
	p22 = zNearX2 / nearHeight;
	p33 = q;
	p34 = (T)1.0;
	p43 = -zNear * q;
}

/*---------------------------------------------------------------------
2*zn/w      0           0           0
0        2*zn/h         0           0
0           0        zf/(zn-zf)    -1
0           0        zn*zf/(zn-zf)  0
---------------------------------------------------------------------*/
template <typename T>
inline void TMatrix4x4<T>::setPerspectiveProjectionRH(T zNear, T zFar, T nearWidth, T nearHeight)
{
	T zNearX2 = zNear * (T)2.0;
	T q = zFar / (zNear - zFar);
	p12 = p13 = p14 = p21 = p23 = p24 = p31 = p32 = p41 = p42 = p44 = 0;
	p11 = zNearX2 / nearWidth;
	p22 = zNearX2 / nearHeight;
	p33 = q;
	p34 = (T)-1.0;
	p43 = zNear * q;
}

template <typename T>
inline void TMatrix4x4<T>::setViewMatrix(const TVector3<T> &from, const TVector3<T> &view,
										 const TVector3<T> &up, const TVector3<T> &right)
{
	p11 = right.x;	p21 = right.y;	p31 = right.z;	p41 = -(from.dot(right));
	p12 = up.x;		p22 = up.y;		p32 = up.z;		p42 = -(from.dot(up));
	p13 = -view.x;	p23 = -view.y;	p33 = -view.z;	p43 = -(from.dot(-view));
	p14 = 0;		p24 = 0;		p34 = 0;		p44 = 1;
}

template <typename T>
inline void TMatrix4x4<T>::setViewMatrix(const TVector3<T> &from, const TVector3<T> &lookAt, const TVector3<T> &up)
{
	TVector3<T> view(lookAt); view -= from;
	view.normalize();
	TVector3<T> right(view); right *= up;
	right.normalize();
	TVector3<T> trueUp(right); trueUp *= view;
	trueUp.normalize();
	setViewMatrix(from, view, trueUp, right);
}

// Cast to D3DXMATRIX
template <typename T>
inline D3DXMATRIX * TMatrix4x4<T>::toD3DXMATRIX()
{
	_ASSERTE(false && "toD3DXMATRIX() only supported for float type");
	return 0;
}

template <typename T>
inline const D3DXMATRIX * TMatrix4x4<T>::toD3DXMATRIX() const
{
	_ASSERTE(false && "toD3DXMATRIX() only supported for float type");
	return 0;
}

inline D3DXMATRIX * TMatrix4x4<float>::toD3DXMATRIX()
{
	return reinterpret_cast<D3DXMATRIX *>(this);
}

inline const D3DXMATRIX * TMatrix4x4<float>::toD3DXMATRIX() const
{
	return reinterpret_cast<const D3DXMATRIX *>(this);
}

// Constructors
template <typename T>
TMatrix4x4<T>::TMatrix4x4(const TMatrix4x4<T> &m)
{
	memcpy_s(p, sizeof(p), m.p, sizeof(T)*16);
}

template <typename T>
TMatrix4x4<T>::TMatrix4x4(const TVector4<T> &r1, const TVector4<T> &r2,
						  const TVector4<T> &r3, const TVector4<T> &r4)
{
	p[0]  = r1.x; p[1]  = r1.y; p[2]  = r1.z; p[3]  = r1.w;
	p[4]  = r2.x; p[5]  = r2.y; p[6]  = r2.z; p[7]  = r2.w;
	p[8]  = r3.x; p[9]  = r3.y; p[10] = r3.z; p[11] = r3.w;
	p[12] = r4.x; p[13] = r4.y; p[14] = r4.z; p[15] = r4.w;
}

template <typename T>
TMatrix4x4<T>::TMatrix4x4(const T *v, int length)
{
	_ASSERTE(p && (length > 0) && (length < 16));
	memcpy_s(p, sizeof(p), v, sizeof(T)*length);
}