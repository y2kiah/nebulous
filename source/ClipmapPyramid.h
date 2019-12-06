/* ----==== CLIPMAPPYRAMID.H ====---- */

#pragma once

#include <vector>

///// STRUCTURES /////

class ClipmapRegion;


/*=============================================================================
	class  ClipmapPyramid

=============================================================================*/
class ClipmapPyramid {
	private:
		///// VARIABLES /////

		int		numLevels;
		int		regionSize;				// = 2^k - 1 (default 255, min 15, max 1023), vertices, a.k.a. n
		int		regionBlockM;			// = (size + 1)/4, this is numVertices in the mxm, mx3, and (2m+1)x2 drawing blocks

		std::vector<ClipmapRegion*>		regions;

	public:
		///// ACCESSORS /////

		int		getSize() const				{ return regionSize; }
		int		getBlockM() const			{ return regionBlockM; }

		///// FUNCTIONS /////

		void	placeRegion(int level, float cameraPosX, float cameraPosZ);
		void	updateRegions(float cameraPosX, float cameraPosZ, float cameraHeightAboveSurface);

		explicit ClipmapPyramid(int _numLevels, int _size, float _smallestElementLength);
		~ClipmapPyramid();
};
