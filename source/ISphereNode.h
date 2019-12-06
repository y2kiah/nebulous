/*----==== ISPHERENODE.H ====----
	Author:	Jeffrey Kiah
	Date:	7/4/2007
-------------------------------*/

#pragma once

#include <vector>
#include "Math/TVector3.h"


///// STRUCTURES /////

/*=============================================================================
	A single node in an ISphereTree, a hierarchical bounding sphere tree with
	integer precision. This class is owned by the objects that use it with a
	"has-a" relationship. The construction and destruction of ISphereNode
	objects is the job of the owning object, not the ISphereTree.
=============================================================================*/
class ISphereNode {
	private:
		ISphereNode					*parentNode;
		std::vector<ISphereNode*>	childNodes;

		Vector3i			position;
		uint				radius;
		uint				radiusSquared;

		explicit ISphereNode(const ISphereNode &n) {}	// disable copy constructor

	public:
		const ISphereNode *	getParentNode() const		{ return parentNode; }
		const Vector3i &	getPosition() const			{ return position; }
		uint				getRadius() const			{ return radius; }
		uint				getRadiusSquared() const	{ return radiusSquared; }

		bool fullyContains(const ISphereNode *n) {
			_ASSERTE(n);
			if (n->getRadius() >= radius) return false;
			return (position.distSquared(n->getPosition()) < static_cast<int>(radiusSquared - n->getRadiusSquared()));
		}
		
		bool isfullyContainedBy(const ISphereNode *n) {
			_ASSERTE(n);
			if (radius >= n->getRadius()) return false;
			return (position.distSquared(n->getPosition()) < static_cast<int>(n->getRadiusSquared() - radiusSquared));
		}

		int		addChildNode(ISphereNode *n);
		int		removeChildNode(int index);

		// Constructor / Destructor
		explicit ISphereNode() : parentNode(0), radius(0), radiusSquared(0) {}
		explicit ISphereNode(int _x, int _y, int _z, uint _radius) : parentNode(0), position(_x,_y,_z), radius(_radius), radiusSquared(_radius*_radius) {}
		explicit ISphereNode(const Vector3i &_pos, uint _radius) : parentNode(0), position(_pos), radius(_radius), radiusSquared(_radius*_radius) {}

		~ISphereNode() {
			for (uint n = 0; n < childNodes.size(); ++n) delete childNodes[n];
			childNodes.clear();
		}
};
