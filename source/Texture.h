/*----==== TEXTURE.H ====----
	Author: Jeff Kiah
	Date:	4/19/2007
---------------------------*/

#pragma once

#include <string>
#include "Utility Code/Typedefs.h"


///// TYPEDEFS /////

enum TextureFilterMode : uchar {
	TEX_FILTER_DEFAULT = 0,		// Should be equivalent to TEX_FILTER_LINEAR in the switch statement
	TEX_FILTER_NEAREST,
	TEX_FILTER_LINEAR,
	TEX_FILTER_ANISOTROPIC,
	TEX_FILTER_PYRAMIDAL_QUAD,
	TEX_FILTER_GAUSSIANQUAD,
	TEX_FILTER_NONE
};

enum TextureRepeatMode : uchar {
	TEX_RPT_WRAP = 0,
	TEX_RPT_MIRROR_U,
	TEX_RPT_MIRROR_V,
	TEX_RPT_MIRROR,
	TEX_RPT_CLAMP_U,	// implemented in D3D9?
	TEX_RPT_CLAMP_V,	// implemented in D3D9?
	TEX_RPT_CLAMP		// implemented in D3D9?
};

enum TextureDataFormat : uchar {
	TEX_FMT_UCHAR = 0,
	TEX_FMT_FLOAT
};

enum TextureUsageMode : uchar {
	TEX_USAGE_DEFAULT = 0,
	TEX_USAGE_DYNAMIC,
	TEX_USAGE_DISPLACEMENT_MAP
};

enum TextureEnvMode : uchar {	// Use with OpenGL
	TEX_ENV_MODULATE = 0,
	TEX_ENV_DECAL,
	TEX_ENV_BLEND
};

enum TextureFileFormat : uchar {
	TEX_FILE_TGA = 0,
	TEX_FILE_JPG,
	TEX_FILE_BMP,
	TEX_FILE_DDS,
	TEX_FILE_PNG
};

///// STRUCTURES /////

class TextureBase {
	protected:

		///// VARIABLES /////
		
		std::wstring	name;
		ushort			width, height;
		uchar			initialized;
		
	public:
		///// FUNCTIONS /////

		// Accessors

		bool					isInitialized(void) const	{ return (initialized != 0); }
		ushort					getWidth(void) const		{ return width; }
		ushort					getHeight(void) const		{ return height; }
		const std::wstring &	getName(void) const			{ return name; }
		std::wstring			toString(void) const;
		
		// Constructor / Destructor
		explicit TextureBase();
		virtual ~TextureBase() {}
};
