/*----==== RENDEROBJECT_D3D9.CPP ====----
	Author: Jeff Kiah
	Date:	4/13/2007
---------------------------------------*/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN	// defined in project settings
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include "RenderObject_D3D9.h"
#include "Utility Code/TypeDefs.h"


///// FUNCTIONS /////

RenderObject_D3D9::RenderObject_D3D9() : RenderObject(),
	vertexBuffer(0),	
	lpVertices(0),
	numIndexBuffers(0)
{
	indexBuffer.reserve(1);
}


RenderObject_D3D9::~RenderObject_D3D9()
{
	SAFE_RELEASE(vertexBuffer);
	for (unsigned int i = 0; i < indexBuffer.size(); ++i) delete indexBuffer[i];	
	indexBuffer.clear();
}