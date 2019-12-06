/* ----==== SPHERICALTERRAIN.CPP ====---- */

#include "SphericalTerrain.h"
#include "Math/FastMath.h"

bool SphericalTerrain::onIsLeafNode::operator()(int level, const uint *index)
{
	return true;
}

void SphericalTerrain::onBranchFirstVisit::operator()(int level, const uint *index)
{

}

void SphericalTerrain::onLeafVisit::operator()(int level, const uint *index)
{

}


SphericalTerrain::SphericalTerrain(float _radius, float _desiredPrecM) :
	mRadius(_radius), mDesiredPrecM(_desiredPrecM)
{
	float circ = TWO_PIf * mRadius;
	circ *= 0.25f;

	mQuadTree = new Quadtree32(18); // hard-coded maxLevel replace with calc
	mQuadTree->setOnIsLeafNode(onIsLeafNode());
	mQuadTree->setOnBranchFirstVisit(onBranchFirstVisit());
	mQuadTree->setOnLeafVisit(onLeafVisit());
}

SphericalTerrain::~SphericalTerrain()
{
	delete mQuadTree;
}