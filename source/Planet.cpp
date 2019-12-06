/* ----==== PLANET.CPP ====---- */

#include "Planet.h"
#include "Engine.h"
#include "Math/FastMath.h"
#include "Math/TVector3.h"

///// FUNCTIONS /////

void PlanetTerrain::initTerrain(const Planet &planet, const Vector3f &terrUnitNormal)
{
	parentPlanet = &planet;
	terrainUnitNormal = terrUnitNormal;
	terrainUnitNormal.normalize(); // ensure it is a unit vector

	// determine maximum level quadtree depth
	maxLevel = 0;
	float elementLength = (planet.circumference * 0.25f) / (float)planet.numChunkElements;
	while (elementLength > planet.desiredPrecision) {
		elementLength *= 0.5f;
		++maxLevel;
	}


}

void PlanetTerrain::render()
{
	Vector3f planetPos(parentPlanet->position);
	// determine maximum level based on altitude
	float altitudeSq = planetPos.magSquared(); // planet coordinates kept in camera-space
	planetPos.normalize();
	float dot = planetPos.dot(terrainUnitNormal);
	int useMaxLevel = maxLevel;

	// determine minimum level based on altitude
	int useMinLevel = 0;

	// start at root of quadtree and render chunks
}

void Planet::buildRenderObjects()
{
	// Sphere section
/*
	// Create RenderObject
	sphereSection.setRoID(engine.renderMgr().createRenderObject());
	sphereSection.roParams.setupRender(0, -1, false, false, false);
	sphereSection.roParams.setTextureID(0,0); // ask the rendermanager for a new dynamic texture? generate the cube map for the planet?

	// Construct vertex buffer
	const unsigned short numVertices = 32;
	const unsigned short vbSize = numVertices * numVertices;
	const float radianStart = 0.0f;
	const float radianSpan = PIf / 2.0f; //PI / 4.0f; // 45 degrees
	const float radiansPerVertex = radianSpan / (float)(numVertices - 1);
	//const float uniformVertDist = 1.0f / (float)(numVertices - 1);

	float *tempVBuffer = new float[vbSize*3];
	
	float currentYRad = radianStart;
	for (int y = 0; y < numVertices; ++y) {
		float currentXRad = 0.0f;
		for (int x = 0; x < numVertices; ++x) {
			float valueX = math.getTan(math.radToIndex(currentXRad));
			float valueY = math.getTan(math.radToIndex(currentYRad));

			Vector3f v(	(x == numVertices-1) ? 1.0f : ((x == 0) ? 0.0f : valueX), 
						(y == numVertices-1) ? 1.0f : ((y == 0) ? 0.0f : valueY),
						-1.0f);

			v.normalize();
			v *= 6378000.0f;

			tempVBuffer[(y*numVertices*3)+(x*3)]	= v.x;
			tempVBuffer[(y*numVertices*3)+(x*3)+1]	= v.y;
			tempVBuffer[(y*numVertices*3)+(x*3)+2]	= v.z;

			currentXRad += radiansPerVertex;
		}
		currentYRad += radiansPerVertex;
	}

	// fill vertex data
//	engine.renderMgr().fillVertexData(sphereSection.getRoID(), tempVBuffer, vbSize, RNDR_RVF_P3);

	// Construct Index Buffers
	const unsigned short ibSize = ((numVertices * 2) + 2) * (numVertices - 1) - 2;
	short *tempIBuffer = new short[ibSize];

	// CCW winding for regular patches
	int ibIndex = 0;
	for (int y = 0; y < numVertices - 1; ++y) {
		for (int x = 0; x < numVertices; ++x) {
			tempIBuffer[ibIndex] = y*numVertices + x;
			++ibIndex;
			tempIBuffer[ibIndex] = (y+1)*numVertices + x;
			++ibIndex;
			// degenerate triangle
			if ((x == numVertices - 1) && (y < numVertices - 2)) {
				tempIBuffer[ibIndex] = (y+1)*numVertices + x;
				++ibIndex;
				tempIBuffer[ibIndex] = (y+1)*numVertices;
				++ibIndex;
			}
		}
	}

	// fill index buffer 0
//	engine.renderMgr().addIndexBuffer(sphereSection.getRoID(), tempIBuffer, ibSize, sizeof(short), vbSize, ibSize-2, RNDR_PT_TRIANGLESTRIP);

	// CW winding for mirrored chunks
	ibIndex = 0;
	for (int y = 0; y < numVertices - 1; ++y) {
		for (int x = numVertices - 1; x >= 0; --x) {
			tempIBuffer[ibIndex] = y*numVertices + x;
			++ibIndex;
			tempIBuffer[ibIndex] = (y+1)*numVertices + x;
			++ibIndex;
			// degenerate triangle
			if ((x == 0) && (y < numVertices - 2)) {
				tempIBuffer[ibIndex] = (y+1)*numVertices;
				++ibIndex;
				tempIBuffer[ibIndex] = (y+1)*numVertices + numVertices - 1;
				++ibIndex;
			}
		}
	}

	// fill index buffer 1
//	engine.renderMgr().addIndexBuffer(sphereSection.getRoID(), tempIBuffer, ibSize, sizeof(short), vbSize, ibSize-2, RNDR_PT_TRIANGLESTRIP);
	
	try {
	delete [] tempVBuffer;
	delete [] tempIBuffer;
	} catch (...) {
		debugPrintf("exception caught");
	}
*/
}

void Planet::update()
{
}

void Planet::render()
{
/*
	// FRONT
	// upper right
	sphereSection.roParams.worldPos.assign(0.0f, 0.0f, 0.0f);
	sphereSection.roParams.angle.assign(0.0f, 0.0f, 0.0f);
	sphereSection.roParams.scale.assign(1.0f, 1.0f, 1.0f);	
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// upper left
	sphereSection.roParams.indexBufferID = 1;
	sphereSection.roParams.scale.assign(-1.0f, 1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// lower right
	sphereSection.roParams.scale.assign(1.0f, -1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// lower left
	sphereSection.roParams.indexBufferID = 0;
	sphereSection.roParams.scale.assign(-1.0f, -1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);

	// BACK	
	// upper right
	sphereSection.roParams.indexBufferID = 1;
	sphereSection.roParams.scale.assign(1.0f, 1.0f, -1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// upper left
	sphereSection.roParams.indexBufferID = 0;
	sphereSection.roParams.scale.assign(-1.0f, 1.0f, -1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// lower right
	sphereSection.roParams.scale.assign(1.0f, -1.0f, -1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// lower left
	sphereSection.roParams.indexBufferID = 1;
	sphereSection.roParams.scale.assign(-1.0f, -1.0f, -1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);

	// TOP
	// upper right
	sphereSection.roParams.indexBufferID = 0;
	sphereSection.roParams.angle.assign(PI * 0.5f, 0.0f, 0.0f);
	sphereSection.roParams.scale.assign(1.0f, 1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// upper left
	sphereSection.roParams.indexBufferID = 1;
	sphereSection.roParams.scale.assign(-1.0f, 1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// lower right
	sphereSection.roParams.scale.assign(1.0f, -1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// lower left
	sphereSection.roParams.indexBufferID = 0;
	sphereSection.roParams.scale.assign(-1.0f, -1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);

	// BOTTOM
	// upper right
	sphereSection.roParams.indexBufferID = 0;
	sphereSection.roParams.angle.assign(PI * 1.5f, 0.0f, 0.0f);
	sphereSection.roParams.scale.assign(1.0f, 1.0f, 1.0f);	
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// upper left
	sphereSection.roParams.indexBufferID = 1;
	sphereSection.roParams.scale.assign(-1.0f, 1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// lower right
	sphereSection.roParams.scale.assign(1.0f, -1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// lower left
	sphereSection.roParams.indexBufferID = 0;
	sphereSection.roParams.scale.assign(-1.0f, -1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);

	// LEFT
	// upper right
	sphereSection.roParams.indexBufferID = 0;
	sphereSection.roParams.angle.assign(0.0f, PI * 0.5f, 0.0f);
	sphereSection.roParams.scale.assign(1.0f, 1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// upper left
	sphereSection.roParams.indexBufferID = 1;
	sphereSection.roParams.scale.assign(-1.0f, 1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// lower right
	sphereSection.roParams.scale.assign(1.0f, -1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// lower left
	sphereSection.roParams.indexBufferID = 0;
	sphereSection.roParams.scale.assign(-1.0f, -1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);

	// RIGHT
	// upper right
	sphereSection.roParams.indexBufferID = 0;
	sphereSection.roParams.angle.assign(0.0f, PI * 1.5f, 0.0f);
	sphereSection.roParams.scale.assign(1.0f, 1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// upper left
	sphereSection.roParams.indexBufferID = 1;
	sphereSection.roParams.scale.assign(-1.0f, 1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// lower right
	sphereSection.roParams.scale.assign(1.0f, -1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);
	// lower left
	sphereSection.roParams.indexBufferID = 0;
	sphereSection.roParams.scale.assign(-1.0f, -1.0f, 1.0f);
	engine.renderMgr().submitForRendering(sphereSection.roParams);*/
}

Planet::Planet()
{
	buildRenderObjects();
}