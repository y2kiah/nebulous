/*----==== RENDEROPTIONS.H ====----
	Author: Jeff Kiah
	Date:	4/19/2007
---------------------------------*/

#pragma once


///// TYPEDEFS /////

// Mirrors values in D3DPRIMITIVETYPE
enum RenderPrimitiveType : int {
    RNDR_PT_POINTLIST		= 1,
    RNDR_PT_LINELIST		= 2,
    RNDR_PT_LINESTRIP		= 3,
    RNDR_PT_TRIANGLELIST	= 4,
    RNDR_PT_TRIANGLESTRIP	= 5,
    RNDR_PT_TRIANGLEFAN		= 6
};

enum RenderVertexFormat : int {
	RNDR_RVF_UNSPECIFIED = -1,	// uninitialized value -1 so first valid format will be index 0 for array lookups
	RNDR_RVF_V3,				// float x y z		
	RNDR_RVF_V3_N3_UV1,			// float x y z, float nx ny nz, float u v
	RNDR_RVF_V3_N3_UV1_C2		// float x y z, float nx ny nz, float u1 v1, DWORD diffCol, DWORD specCol	
};
#define RNDR_RVF_COUNT		3


///// STRUCTURES /////

struct RVF_V3 {
	float			x, y, z;
};

struct RVF_V3_N3_UV1 {
	float			x, y, z;
	float			nx, ny, nz;
	float			u, v;	
};

struct RVF_V3_N3_UV1_C2 {
	float			x, y, z;
	float			nx, ny, nz;
	float			u1, v1;
	unsigned long	diffCol; // DWORD
	unsigned long	specCol; // DWORD
};


///// FUNCTIONS /////

inline int sizeRVF(RenderVertexFormat rvf)
{
	switch (rvf) {
		case RNDR_RVF_V3:
			return sizeof(RVF_V3);
		case RNDR_RVF_V3_N3_UV1:
			return sizeof(RVF_V3_N3_UV1);
		case RNDR_RVF_V3_N3_UV1_C2:
			return sizeof(RVF_V3_N3_UV1_C2);
	}
	return 0;
}
