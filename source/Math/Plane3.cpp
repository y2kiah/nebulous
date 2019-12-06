/*----==== PLANE3.CPP ====----
	Author:	Jeffrey Kiah
	Date:	2/3/2009
----------------------------*/

#include "Plane3.h"

void Plane3::set(const Vector3f &p1, const Vector3f &p2, const Vector3f &p3)
{
	Vector3f one(p2);
	one -= p1;						// one = p2 - p1;
		
	Vector3f two(p3);
	two -= (p1);					// two = p3 - p1;

	n.unitNormalOf(one, two);		// Get the unit normal
	d = n.dot(p1);						// Get the plane equation constant with dot product
}

void Plane3::set(const Vector3f &p, const Vector3f &_n)
{
	n = _n;
	n.normalize();

	d = n.dot(p);
}

Plane3::Plane3(float x1, float y1, float z1,
			   float x2, float y2, float z2,
			   float x3, float y3, float z3)
{
	Vector3f p1(x1, y1, z1);

	Vector3f one(x2-x1, y2-y1, z2-z1);	// one = p2 - p1;
	Vector3f two(x3-x1, y3-y1, z3-z1);	// two = p3 - p1;

	n.unitNormalOf(one, two);		// Get the unit normal
	d = n.dot(p1);					// Get the plane equation constant with dot product
}


Plane3::Plane3(const Vector3f &p1, const Vector3f &p2, const Vector3f &p3)
{
	Vector3f one(p2);
	one -= p1;						// one = p2 - p1;
		
	Vector3f two(p3);
	two -= (p1);					// two = p3 - p1;

	n.unitNormalOf(one, two);		// Get the unit normal
	d = n.dot(p1);					// Get the plane equation constant with dot product
}


Plane3::Plane3(const Vector3f &_n, const Vector3f &_p, bool unitNormal)
{
	n = _n;
	if (!unitNormal) n.normalize();

	d = n.dot(_p);
}
