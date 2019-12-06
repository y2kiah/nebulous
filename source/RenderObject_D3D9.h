/* ----==== RENDEROBJECT_D3D9.H ====---- */

#pragma once

#include <vector>
#include "RenderObject.h"
#include "Utility Code/TypeDefs.h"


///// STRUCTURES /////

class RO_D3D9_IndexBuffer {
	friend class RenderManager_D3D9;
	friend class RenderObject_D3D9;

	private:
		///// FUNCTIONS /////
		RO_D3D9_IndexBuffer() : indexBuffer(0), lpIndices(0), numVertices(0), numPrimitives(0), primitiveType(D3DPT_TRIANGLESTRIP) {}

		RO_D3D9_IndexBuffer(unsigned short _numVertices, unsigned short _numPrimitives, D3DPRIMITIVETYPE _primitiveType) :
			indexBuffer(0), lpIndices(0), numVertices(_numVertices), numPrimitives(_numPrimitives), primitiveType(_primitiveType) {}

		~RO_D3D9_IndexBuffer() { SAFE_RELEASE(indexBuffer); }

	public:
		///// VARIABLES /////
		IDirect3DIndexBuffer9	*indexBuffer;
		void					*lpIndices;
		unsigned short			numVertices;	// number of unique vertices referenced by this buffer
		unsigned short			numPrimitives;	// number of primitives drawn with this buffer
		D3DPRIMITIVETYPE		primitiveType;
};


/*=============================================================================
	class  RenderObject_D3D9
		Direct3D9 implementation of a renderable object.
=============================================================================*/
class RenderObject_D3D9 : public RenderObject {
	friend class RenderManager_D3D9;

	private:
		///// VARIABLES /////

		// Vertex Buffer
		IDirect3DVertexBuffer9				*vertexBuffer;		
		void								*lpVertices;
		// Index Buffers
		std::vector<RO_D3D9_IndexBuffer*>	indexBuffer;
//		std::vector<IDirect3DIndexBuffer9*>	indexBuffer;
//		std::vector<void*>					lpIndices;
		unsigned int						numIndexBuffers;

	public:
		///// FUNCTIONS /////

		// Contructor / Destructor
		explicit RenderObject_D3D9();
		~RenderObject_D3D9();

};