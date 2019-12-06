/*----==== RENDEROPTIONS.H ====----
	Author:		Jeff Kiah
	Orig.Date:	04/19/2007
	Rev.Date:	06/30/2009
---------------------------------*/

#pragma once

#include "../Utility/Typedefs.h"

///// DEFINITIONS /////

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
	RNDR_RVF_UNSPECIFIED = -1,	// uninitialized value -1, first valid format is index 0 for array lookups
	RNDR_RVF_P3,				// position
	RNDR_RVF_P3_N3_UV2,			// position, normal, texcoord
	RNDR_RVF_P3_N3_UV2_C2,		// position, normal, texcoord, color, color
	RNDR_RVF_P3_N3_T3_BN3_UV2,	// position, normal, tangent, binormal, texcoord
	RNDR_RVF_MAX
};

///// STRUCTURES /////

struct RVF_P3 {
	float	x, y, z;
};

struct RVF_P3_N3_UV2 {
	float	x, y, z;
	float	nx, ny, nz;
	float	u, v;	
};

struct RVF_P3_N3_UV2_C2 {
	float	x, y, z;
	float	nx, ny, nz;
	float	u1, v1;
	ulong	diffCol; // DWORD
	ulong	specCol; // DWORD
};

struct RVF_P3_N3_T3_BN3_UV2 {
	float	x, y, z;
	float	nx, ny, nz;
	float	tx, ty, tz;
	float	bn1, bn2, bn3;
	float	u1, v1;
};

///// FUNCTIONS /////

inline int sizeRVF(RenderVertexFormat rvf)
{
	switch (rvf) {
		case RNDR_RVF_P3:
			return sizeof(RVF_P3); break;
		case RNDR_RVF_P3_N3_UV2:
			return sizeof(RVF_P3_N3_UV2); break;
		case RNDR_RVF_P3_N3_UV2_C2:
			return sizeof(RVF_P3_N3_UV2_C2); break;
		case RNDR_RVF_P3_N3_T3_BN3_UV2:
			return sizeof(RVF_P3_N3_T3_BN3_UV2); break;
	}
	return 0;
}
