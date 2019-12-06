/* ----==== PLANET.H ====---- */

#pragma once

#include "Math/TVector3.h"

///// STRUCTURES /////

class Planet;

class PlanetTerrainQuadtreeNodeData {
	private:
	public:
		explicit PlanetTerrainQuadtreeNodeData() {}
		~PlanetTerrainQuadtreeNodeData() {}
};

class PlanetTerrainQuadtreeNode {
	private:
		// union containing 4 child nodes with difference access methods
		union {
			struct { PlanetTerrainQuadtreeNode *n11, *n12, *n21, *n22; };
			PlanetTerrainQuadtreeNode * n[4];
			PlanetTerrainQuadtreeNode * n2[2][2];
		};
		PlanetTerrainQuadtreeNode *		mParentNode;
		PlanetTerrainQuadtreeNodeData * mNodeData;

	public:
		explicit PlanetTerrainQuadtreeNode() : n11(0), n12(0), n21(0), n22(0), mParentNode(0), mNodeData(0) {}
		~PlanetTerrainQuadtreeNode() { for (int i = 0; i < 4; ++i) delete n[i]; delete mNodeData; }
};

class PlanetTerrainQuadtree {
	private:
		PlanetTerrainQuadtreeNode rootNode;

	public:
		explicit PlanetTerrainQuadtree() {}
		~PlanetTerrainQuadtree() {}
};

class PlanetTerrain {
	private:
		Vector3f	terrainUnitNormal;	// identifies the terrain as one of the 6 cube faces (+x,-x,+y,-y,+z,-z)
		int			maxLevel;			// maximum quadtree level determined from planet radius, # chunk elements, and desired precision
		const Planet *parentPlanet;

	public:
		void		initTerrain(const Planet &planet, const Vector3f &terrUnitNormal);
		void		render();

		explicit PlanetTerrain() {}
		~PlanetTerrain() {}
};

class Planet {
	friend class PlanetTerrain;
	private:
		Vector3f	position;			// in meters, kept in camera space not local world-space
		float		radius;				// in meters
		float		diameter;			// = 2*r
		float		circumference;		// = 2*PI*r
		float		surfaceArea;		// = 4*PI*r^2
		float		volume;				// = 4/3*PI*r^3
		float		mass;				// in kg
		float		desiredPrecision;	// in meters
		uint		numChunkElements;	// = # chunk axis vertices - 1
		
		//RenderObjectOwner	sphereSection;	// this should be static except the render parameters would not be (different textures)

		void	buildRenderObjects();
		
	public:
		void	initPlanet();
		void	update();
		void	render();

		explicit Planet();
		~Planet() {}
};