/* ----==== RENDERMANAGER.H ====---- */

#pragma once

#include <string>
#include <vector>
#include <map>
#include "RenderObject.h"
#include "RenderObjectParams.h"


///// TYPEDEFS /////

enum RenderDevice : int {
	RNDR_DEVICE_D3D9	= 0,
	RNDR_DEVICE_OPENGL	= 1
};

enum RenderOptionsFlag : int {
	RNDR_OPT_CLEARSCREEN	= 1,
	RNDR_OPT_CLEARZBUFFER	= 2,
	RNDR_OPT_ENABLEFOG		= 4
};


///// STRUCTURES /////

class Camera;
class RenderObject;


/*=============================================================================
	class RenderManager

=============================================================================*/
class RenderManager abstract {
	protected:
		///// VARIABLES /////

		int								renderOptions;			// set bits with RENDER_OPTIONS enum
		std::string						renderName;

		std::vector<RenderObject*>		renderObjects;			// list of RenderObjects created, indexed by roID
		std::vector<unsigned int>		freeRenderObjectIDs;	// roIDs of removed RenderObjects

		std::vector<RenderObjectParams*>	submittedObjects;	// renderObjects submitted for rendering
		unsigned int						numSubmitted;

		///// FUNCTIONS /////

		virtual bool			renderObject(const RenderObjectParams &rop, const Camera *cam) const = 0;

		// Constructor
		explicit RenderManager(const std::string &_renderName);		

	public:
		///// FUNCTIONS /////

		virtual const std::string &getRenderName() const { return renderName; }

		// RenderObject management
		virtual unsigned int	createRenderObject() = 0;				
		virtual void			removeRenderObject(unsigned int roID) = 0;
		virtual void			clearRenderObjects() = 0;
		virtual void			submitForRendering(const RenderObjectParams &rop) = 0; // - may want to replace int priority with enum type value
		virtual void			resetSubmitList() = 0;
		virtual void			prepareSubmitList() = 0;
		virtual bool			fillVertexData(unsigned int roID, const void *_data, int _numVerts, RenderVertexFormat _vFormat) const = 0;
		virtual bool			addIndexBuffer(unsigned int roID, const void *_data, int _numIndices, int _indexSizeInBytes,
												unsigned short _numVertices, unsigned short _numPrimitives, RenderPrimitiveType _primitiveType) const = 0;

		// RenderManager methods
		virtual bool			initRenderDevice(int bpp, int resX, int resY, int refreshRate, bool fullscreen) = 0;
		virtual void			freeRenderDevice() = 0;
		virtual bool			render(const Camera *cam) const = 0;

		// Destructor
		virtual ~RenderManager() {}
};