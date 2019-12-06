/* ----==== CLIPMAPPYRAMID.CPP ====---- */

#include "ClipmapPyramid.h"
#include "ClipmapRegion.h"
#include "Math/FastMath.h"


///// FUNCTIONS /////

/*=============================================================================
	placeRegion
		assumes that (level-1) is already active and has already been placed
		accurately in the hierarchy
=============================================================================*/
void ClipmapPyramid::placeRegion(int level, float cameraPosX, float cameraPosZ)
{
	if (level > 0 && level < numLevels) {
		float cx = regions[level-1]->getCenter().x;
		float cz = regions[level-1]->getCenter().y;

		if (cameraPosX < cx) {
			cx -= regions[level]->getElementLength();
		} else {
			cx += regions[level]->getElementLength();
		}

		if (cameraPosZ < cz) {
			cz -= regions[level]->getElementLength();
		} else {
			cz += regions[level]->getElementLength();
		}

		regions[level]->setCenter(cx, cz);

	} else if (level == 0) {
		regions[0]->setCenter(0.0f, 0.0f);
	}
}

/*=============================================================================
	updateRegions
		cameraPosX, cameraPosZ are in clipmap space, and maintained outside of
		this function so can be assumed are relative to clipmap origin
=============================================================================*/
void ClipmapPyramid::updateRegions(float cameraPosX, float cameraPosZ, float cameraHeightAboveSurface)
{
	for (int c = numLevels - 1; c >= 0; --c) {
		if (regions[c]->getDrawingActive()) {
			int moveX = 0, moveZ = 0;

			// check if camera is forcing clipmap to move
			if (cameraPosX < regions[c]->getCenter().x - regions[c]->getElementLength()) {				
				moveX = FastMath::f2iTrunc((cameraPosX - regions[c]->getCenter().x) * regions[c]->getInvElementLength());
			} else if (cameraPosX >= regions[c]->getCenter().x + regions[c]->getElementLength()) {
				moveX = FastMath::f2iTrunc((cameraPosX - regions[c]->getCenter().x) * regions[c]->getInvElementLength());
			}

			if (cameraPosZ < regions[c]->getCenter().y - regions[c]->getElementLength()) {
				moveZ = FastMath::f2iTrunc((cameraPosZ - regions[c]->getCenter().y) * regions[c]->getInvElementLength());
			} else if (cameraPosZ >= regions[c]->getCenter().y + regions[c]->getElementLength()) {
				moveZ = FastMath::f2iTrunc((cameraPosZ - regions[c]->getCenter().y) * regions[c]->getInvElementLength());
			}

			// get new height values
			// move center of region
			regions[c]->setCenter((float)moveX*regions[c]->getElementLength(), (float)moveZ*regions[c]->getElementLength());
		}
	}
}



///// CONSTRUCTOR / DESTRUCTOR /////

ClipmapPyramid::ClipmapPyramid(int _numLevels, int _size, float _smallestElementLength) :
	numLevels(_numLevels),
	regionSize(_size),
	regionBlockM((_size + 1) / 4)
{
	regions.reserve(_numLevels);

	float elementLength = _smallestElementLength;
	for (int c = _numLevels - 1; c >= 0; --c) {
		regions[c] = new ClipmapRegion(c, _size, elementLength);
		elementLength *= 2.0f;
	}
}


ClipmapPyramid::~ClipmapPyramid()
{
	for (int c = 0; c < numLevels; ++c) {
		delete regions[c];
	}
	regions.clear();
}
