/*----==== PLANE3.H ====----
	Author:	Jeffrey Kiah
	Date:	2/3/2009
--------------------------*/

#pragma once

#include "TVector3.h"

/*=============================================================================
=============================================================================*/
class Plane3 {
	public:
		///// VARIABLES /////
		Vector3f	n;			// plane normal
		float		d;			// ax + by + cz + d = 0 ... or distance from origin of plane

		///// FUNCTIONS /////
		float	findDist(const Vector3f &p) const { return n.dot(p) - d; }
		void	set(const Vector3f &p1, const Vector3f &p2, const Vector3f &p3);
		void	set(const Vector3f &p, const Vector3f &n);

		// Constructors / Destructor
		explicit Plane3() : n(0,0,0), d(0) {}
		explicit Plane3(float x1, float y1, float z1,
						float x2, float y2, float z2,
						float x3, float y3, float z3);
		explicit Plane3(const Vector3f &p1, const Vector3f &p2, const Vector3f &p3);
		explicit Plane3(const Vector3f &_n, const Vector3f &_p, bool unitNormal);
		explicit Plane3(const Vector3f &_n, float _d) : n(_n), d(_d) { n.normalize(); }
		explicit Plane3(const Plane3 &p) : n(p.n), d(p.d) {}
		~Plane3() {}
};