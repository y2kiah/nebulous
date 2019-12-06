/* ----==== SPHERICALTERRAIN.H ====---- */

#pragma once

#include "Utility/Quadtree.h"

///// STRUCTURES /////

//=============================================================================
//=============================================================================
class SphericalTerrainChunk : public IQuadtreeNode {
	private:
		

	public:
		explicit SphericalTerrainChunk();
		virtual ~SphericalTerrainChunk();
};

//=============================================================================
//=============================================================================
class SphericalTerrain {
	private:
		///// VARIABLES /////
		Quadtree32 *	mQuadTree;
		
		float			mRadius;		// inner radius of the terrain
		float			mDesiredPrecM;	// desired max surface precision (meters)
		float			mActualPrecM;	// actual max surface precision (meters)

		// Function objects
		struct onIsLeafNode {
			bool operator()(int level, const uint *index);
		};
		struct onBranchFirstVisit {
			void operator()(int level, const uint *index);
		};
		struct onLeafVisit {
			void operator()(int level, const uint *index);
		};

	public:
		///// FUNCTIONS /////

		// Accessors
		float	radius() const { return mRadius; }
		float	actualPrec() const { return mActualPrecM; }

		// Constructor / Destructor
		explicit SphericalTerrain(float _radius, float _desiredPrecM);
		~SphericalTerrain();
};