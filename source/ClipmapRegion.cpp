/* ----==== CLIPMAPREGION.CPP ====---- */

#include "ClipmapRegion.h"
#include "Utility/Typedefs.h"


///// CONSTRUCTOR / DESTRUCTOR /////

ClipmapRegion::ClipmapRegion() :
	level(0),
	elementLength(0.0f),
	invElementLength(0.0f),
	regionLength(0.0f),
	halfRegionLength(0.0f),
	maxVisibleHeight(0.0f),
	regionActive(false)
{
}


ClipmapRegion::ClipmapRegion(int _level, int _size, float _elementLength) :
	level(_level),
	elementLength(_elementLength),	
	regionActive(false)
{
	_ASSERTE(_elementLength > 0.0f);

	invElementLength = 1.0f / _elementLength;
	regionLength = (_size-1) * elementLength;
	halfRegionLength = regionLength * 0.5f;
	maxVisibleHeight = regionLength / 2.5f;
}


ClipmapRegion::~ClipmapRegion()
{

}
