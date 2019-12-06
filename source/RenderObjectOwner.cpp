/* ----==== RENDEROBJECTOWNER.CPP ====---- */

#include "RenderObjectOwner.h"
#include "RenderManager.h"


///// FUNCTIONS /////

bool RenderObjectOwner::setRoID(unsigned int _roID)
{
	if (!initialized) {
		roID = _roID;
		roParams.roID = roID;
		// add to reference count
		initialized = true;
		return true;

	} else {
		return false;
	}
}


RenderObjectOwner::RenderObjectOwner() :
	roID(0),
	initialized(false)
{
}


RenderObjectOwner::RenderObjectOwner(unsigned int _roID) :
	roID(_roID),
	initialized(true)
{
	roParams.roID = _roID;
	// add to reference count	
}