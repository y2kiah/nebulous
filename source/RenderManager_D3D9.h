/*----==== RENDERMANAGER_D3D9.H ====----
	Author: Jeff Kiah
	Date:	4/13/2007
--------------------------------------*/

#pragma once

#include <d3d9.h>
#include "RenderManager.h"


/*=============================================================================
	class  RenderManager_D3D9

=============================================================================*/

class RenderManager_D3D9 : public RenderManager {
	private:
		///// VARIABLES /////

		IDirect3D9					*d3d;
		IDirect3DDevice9			*d3dDevice;
		IDirect3DVertexDeclaration9	*d3dVertexDeclaration[RNDR_RVF_COUNT];
		D3DPRESENT_PARAMETERS		d3dParams;

		///// FUNCTIONS /////

		// Common
		bool			renderObject(const RenderObjectParams &rop, const Camera *cam) const;

		// Specific
		bool			createVertexDeclarations();

	public:
		// Common

		// RenderObject management
		unsigned int	createRenderObject();		
		void			removeRenderObject(unsigned int roID);
		void			clearRenderObjects();
		void			submitForRendering(const RenderObjectParams &rop);
		void			resetSubmitList() { numSubmitted = 0; }
		void			prepareSubmitList();
		bool			fillVertexData(unsigned int roID, const void *_data, int _numVerts, RenderVertexFormat _vFormat) const;
		bool			addIndexBuffer(unsigned int roID, const void *_data, int _numIndices, int _indexSizeInBytes,
									unsigned short _numVertices, unsigned short _numPrimitives, RenderPrimitiveType _primitiveType) const;

		// RenderManager methods
		bool			initRenderDevice(int bpp, int resX, int resY, int refreshRate, bool fullscreen);
		void			freeRenderDevice();
		bool			render(const Camera *cam) const;
		
		// Specific

		HRESULT			resetRenderDevice();
		void			initVolatileResources() {}
		void			freeVolatileResources() {}
		const IDirect3DDevice9 *getDevice() const { return d3dDevice; }

		// Constructor / Destructor

		explicit RenderManager_D3D9();
		~RenderManager_D3D9();
};