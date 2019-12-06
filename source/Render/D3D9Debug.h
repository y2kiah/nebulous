/*----==== D3D9DEBUG.H ====----
	Author: Jeff Kiah
	Date:	4/13/2007
-----------------------------*/

#pragma once

#include "../Utility/Typedefs.h"

///// MACROS /////

#ifdef _DEBUG
	#define d3dDebugSwitch(hr)		d3d_HRESULT_DebugSwitch(hr, __FUNCTION__, __LINE__)
#else
	#define d3dDebugSwitch(hr)
#endif


///// FUNCTIONS /////

inline void d3d_HRESULT_DebugSwitch(HRESULT hr, const char *funcName, int line)
{
	switch (hr) {
		case E_OUTOFMEMORY:
			debugPrintf("%s: line %d: E_OUTOFMEMORY\n", funcName, line); break;

		// D3D Errors
		case D3DERR_WRONGTEXTUREFORMAT:
			debugPrintf("%s: line %d: D3DERR_WRONGTEXTUREFORMAT\n", funcName, line); break;
		case D3DERR_UNSUPPORTEDCOLOROPERATION:
			debugPrintf("%s: line %d: D3DERR_UNSUPPORTEDCOLOROPERATION\n", funcName, line); break;
		case D3DERR_UNSUPPORTEDCOLORARG:
			debugPrintf("%s: line %d: D3DERR_UNSUPPORTEDCOLORARG\n", funcName, line); break;
		case D3DERR_UNSUPPORTEDALPHAOPERATION:
			debugPrintf("%s: line %d: D3DERR_UNSUPPORTEDALPHAOPERATION\n", funcName, line); break;
		case D3DERR_UNSUPPORTEDALPHAARG:
			debugPrintf("%s: line %d: D3DERR_UNSUPPORTEDALPHAARG\n", funcName, line); break;
		case D3DERR_TOOMANYOPERATIONS:
			debugPrintf("%s: line %d: D3DERR_TOOMANYOPERATIONS\n", funcName, line); break;
		case D3DERR_CONFLICTINGTEXTUREFILTER:
			debugPrintf("%s: line %d: D3DERR_CONFLICTINGTEXTUREFILTER\n", funcName, line); break;
		case D3DERR_UNSUPPORTEDFACTORVALUE:
			debugPrintf("%s: line %d: D3DERR_UNSUPPORTEDFACTORVALUE\n", funcName, line); break;
		case D3DERR_CONFLICTINGRENDERSTATE:
			debugPrintf("%s: line %d: D3DERR_CONFLICTINGRENDERSTATE\n", funcName, line); break;
		case D3DERR_UNSUPPORTEDTEXTUREFILTER:
			debugPrintf("%s: line %d: D3DERR_UNSUPPORTEDTEXTUREFILTER\n", funcName, line); break;
		case D3DERR_CONFLICTINGTEXTUREPALETTE:
			debugPrintf("%s: line %d: D3DERR_CONFLICTINGTEXTUREPALETTE\n", funcName, line); break;
		case D3DERR_DRIVERINTERNALERROR:
			debugPrintf("%s: line %d: D3DERR_DRIVERINTERNALERROR\n", funcName, line); break;
		case D3DERR_NOTFOUND:
			debugPrintf("%s: line %d: D3DERR_NOTFOUND\n", funcName, line); break;
		case D3DERR_MOREDATA:
			debugPrintf("%s: line %d: D3DERR_MOREDATA\n", funcName, line); break;
		case D3DERR_DEVICELOST:
			debugPrintf("%s: line %d: D3DERR_DEVICELOST\n", funcName, line); break;
		case D3DERR_DEVICENOTRESET:
			debugPrintf("%s: line %d: D3DERR_DEVICENOTRESET\n", funcName, line); break;
		case D3DERR_NOTAVAILABLE:
			debugPrintf("%s: line %d: D3DERR_NOTAVAILABLE\n", funcName, line); break;
		case D3DERR_OUTOFVIDEOMEMORY:
			debugPrintf("%s: line %d: D3DERR_OUTOFVIDEOMEMORY\n", funcName, line); break;
		case D3DERR_INVALIDDEVICE:
			debugPrintf("%s: line %d: D3DERR_INVALIDDEVICE\n", funcName, line); break;
		case D3DERR_INVALIDCALL:
			debugPrintf("%s: line %d: D3DERR_INVALIDCALL\n", funcName, line); break;
		case D3DERR_DRIVERINVALIDCALL:
			debugPrintf("%s: line %d: D3DERR_DRIVERINVALIDCALL\n", funcName, line); break;
		case D3DERR_WASSTILLDRAWING:
			debugPrintf("%s: line %d: D3DERR_WASSTILLDRAWING\n", funcName, line); break;
		case D3DOK_NOAUTOGEN:
			debugPrintf("%s: line %d: D3DOK_NOAUTOGEN\n", funcName, line); break;

		// D3DX Errors
		case D3DXERR_CANNOTMODIFYINDEXBUFFER:
			debugPrintf("%s: line %d: D3DXERR_CANNOTMODIFYINDEXBUFFER\n", funcName, line); break;
		case D3DXERR_INVALIDMESH:
			debugPrintf("%s: line %d: D3DXERR_INVALIDMESH\n", funcName, line); break;
		case D3DXERR_CANNOTATTRSORT:
			debugPrintf("%s: line %d: D3DXERR_CANNOTATTRSORT\n", funcName, line); break;
		case D3DXERR_SKINNINGNOTSUPPORTED:
			debugPrintf("%s: line %d: D3DXERR_SKINNINGNOTSUPPORTED\n", funcName, line); break;
		case D3DXERR_TOOMANYINFLUENCES:
			debugPrintf("%s: line %d: D3DXERR_TOOMANYINFLUENCES\n", funcName, line); break;
		case D3DXERR_INVALIDDATA:
			debugPrintf("%s: line %d: D3DXERR_INVALIDDATA\n", funcName, line); break;
		case D3DXERR_LOADEDMESHASNODATA:
			debugPrintf("%s: line %d: D3DXERR_LOADEDMESHASNODATA\n", funcName, line); break;
		case D3DXERR_DUPLICATENAMEDFRAGMENT:
			debugPrintf("%s: line %d: D3DXERR_DUPLICATENAMEDFRAGMENT\n", funcName, line); break;
		case D3DXERR_CANNOTREMOVELASTITEM:
			debugPrintf("%s: line %d: D3DXERR_CANNOTREMOVELASTITEM\n", funcName, line); break;
	}
}