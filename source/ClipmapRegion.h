/* ----==== CLIPMAPREGION.H ====---- */

#pragma once

#include "Math/TVector2.h"


///// STRUCTURES /////

enum RegionCreateMethod : int {
	REGION_CREATE_HEIGHTMAP	= 1,
	REGION_CREATE_SMOOTH,
	REGION_CREATE_SYNTHESIZE
};


/*=============================================================================
=============================================================================*/
class ClipmapRegion {
	private:
		///// VARIABLES /////

		int			level;				// 0 to L-1
		
		float		elementLength;		// "clipmap space" length of one grid element
		float		invElementLength;	// inverse of elementLength
		float		regionLength;		// = numElements * elementLength
		float		halfRegionLength;	// = regionLength / 2

		float		maxVisibleHeight;	// = regionLength / 2.5
		bool		regionActive;		// if level height data not fully updated, or above maxVisibleHeight, it is inactive

		Vector2f	position;			// upper left corner of the region
		Vector2f	center;				// position in clipmap space of the center

		RegionCreateMethod	createMethod;

	public:
		///// ACCESSORS /////

		int		getLevel() const			{ return level; }
		float	getElementLength() const	{ return elementLength; }
		float	getInvElementLength() const { return invElementLength; }
		float	getRegionLength() const		{ return regionLength; }
		float	getMaxVisibleHeight() const	{ return maxVisibleHeight; }
		bool	getDrawingActive() const	{ return regionActive; }

		const Vector2f		&getPosition() const	{ return position; }
		const Vector2f		&getCenter() const		{ return center; }
		RegionCreateMethod	getCreateMethod() const	{ return createMethod; }

		///// FUNCTIONS /////
		
		void	setCenter(float x, float z)	{	center.assign(x, z);												
												position.assign(x - halfRegionLength, z + halfRegionLength);
											}



		explicit ClipmapRegion();
		explicit ClipmapRegion(int _level, int _size, float _elementLength);
		~ClipmapRegion();
};
