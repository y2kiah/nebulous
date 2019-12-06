/* ----==== RENDEROBJECT.H ====---- */

#pragma once

#include "RenderOptions.h"


///// STRUCTURES /////

/*=============================================================================
	class  RenderObject
		Intended to be abstract base class for API specific derived
		implementations. Derived class should store vertex buffer, index
		buffer, shader, material/texture info, etc. to submit to RenderManager.
=============================================================================*/
class RenderObject abstract {
	protected:
		///// VARIABLES /////

		// Derived classes will include API specific objects here to store rendering data

		RenderVertexFormat		vFormat;
		int						numVerts;

		///// FUNCTIONS /////

		explicit RenderObject() : vFormat(RNDR_RVF_UNSPECIFIED), numVerts(0) {}

	public:		

		virtual ~RenderObject() {}
};