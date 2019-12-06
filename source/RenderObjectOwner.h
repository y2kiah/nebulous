/* ----==== RENDEROBJECTOWNER.H ====---- */

#pragma once

#include "RenderObjectParams.h"


///// STRUCTURES /////

class RenderObjectOwner {
	friend class RenderManager;
	friend class RenderManager_D3D9;

	private:
		///// VARIABLES /////

		unsigned int		roID;		// ID of the RenderObject
		bool				initialized;

	public:
		RenderObjectParams	roParams;	// stored copy of roParams for submitting to RenderManager
		
		///// FUNCTIONS /////

		unsigned int		getRoID() const			{ _ASSERTE(initialized); return roID; }		
		bool				isInitialized() const	{ return initialized; }

		bool				setRoID(unsigned int _roID);

		// Constructor / Destructor
		explicit RenderObjectOwner();
		explicit RenderObjectOwner(unsigned int _roID);
		~RenderObjectOwner() {} // subtract from reference count
};