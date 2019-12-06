/*----==== TEXTURE_D3D9.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	04/13/2007
	Rev.Date:	07/17/2009
----------------------------------*/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN	// defined in project settings
#endif

#include "Texture_D3D9.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "D3D9Debug.h"
#include "../Engine.h"

///// FUNCTIONS /////

/*---------------------------------------------------------------------
	Loads a texture directly from a file (not through the Resource-
	Source interface. Use this with the cache injection method.
---------------------------------------------------------------------*/
bool Texture_D3D9::loadFromFile(const string &filename, uint mipLevels, bool compress)
{
	_ASSERTE(mipLevels >= 0 && "mipLevels must be >= 0!");

	if (initialized()) { return false; }
	string loadFilename(filename);

	// get image info from the file in memory
	D3DXIMAGE_INFO imgInfo;
	HRESULT hr = D3DXGetImageInfoFromFileA(loadFilename.c_str(), &imgInfo);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}

	// make sure we're not getting a volume or cubemap texture
	if (imgInfo.ResourceType != D3DRTYPE_TEXTURE) {
		debugPrintf("Texture_D3D9: Error: Incompatible Resource Type!\n");
		return false;
	}
	
	DWORD mipFilter = D3DX_DEFAULT;	// same as D3DX_FILTER_BOX
	// if a DDS file, specify the number of mip levels to skip loading unless 0 or D3DX_DEFAULT
	// is provided, indicating that the full chain should be loaded
	if (imgInfo.ImageFileFormat == D3DXIFF_DDS && mipLevels != 0 && mipLevels != D3DX_DEFAULT) {
		// if we want a different number of mipmap levels, skip some if needed
		int skip = imgInfo.MipLevels - mipLevels;
		skip = skip < 0 ? 0 : skip;
		mipFilter = D3DX_SKIP_DDS_MIP_LEVELS(skip, D3DX_DEFAULT);
	}
	
	// in debug mode, check if the texture is being resized or format changed and dump the
	// before/after values to the console
	#ifdef _DEBUG
	debugCheckReqs(imgInfo, mipLevels);
	#endif

	// Load the right type of texture based on the file
	switch (imgInfo.ResourceType) {
		case D3DRTYPE_TEXTURE: {
			// attempt create the 2d texture
			hr = D3DXCreateTextureFromFileExA(
					spD3DDevice,
					loadFilename.c_str(),	// file to load
					D3DX_DEFAULT,			// dimensions taken from file and if not pow2, will be
					D3DX_DEFAULT,			//		resized to next higher pow2 and filtered
					mipLevels,				// number of mipmap levels from a DDS file, or full chain
					0,						// usage is not rendertarget or dynamic
					D3DFMT_UNKNOWN,			// create texture based on file, but don't fail if changes are needed
					D3DPOOL_MANAGED,		//		also, compression formats would be based on the DDS file
					D3DX_DEFAULT,			// this is D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER
					mipFilter,				// this is D3DX_FILTER_BOX, bits 27-31 specify mip levels to skip
					0,						// 0 disables color key
					&imgInfo,				// pass in the info that we already obtained, could speed things up??
					NULL,
					&mD3DTexture);
			break;
		}
		case D3DRTYPE_CUBETEXTURE: {
			// attempt create the Cube texture
			hr = D3DXCreateCubeTextureFromFileExA(
					spD3DDevice,
					loadFilename.c_str(),	// file to load
					D3DX_DEFAULT,			// dimensions taken from file and if not pow2, will be resized to next higher pow2 and filtered
					mipLevels,				// number of mipmap levels from a DDS file, or full chain
					0,						// usage is not rendertarget or dynamic
					D3DFMT_UNKNOWN,			// create texture based on file, but don't fail if changes are needed
					D3DPOOL_MANAGED,		//		also, compression formats would be based on the DDS file
					D3DX_DEFAULT,			// this is D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER
					mipFilter,				// this is D3DX_FILTER_BOX, bits 27-31 specify mip levels to skip
					0,						// 0 disables color key
					&imgInfo,				// pass in the info that we already obtained, could speed things up??
					NULL,
					&mD3DCubeTexture);
			break;
		}
		case D3DRTYPE_VOLUMETEXTURE: {
			// look in MaxVolumeExtent in D3DCAPS9 for compatibility, skip some mip levels if possible to fit
			// attempt create the 3d texture
			hr = D3DXCreateVolumeTextureFromFileExA(
					spD3DDevice,
					loadFilename.c_str(),	// file to load
					D3DX_DEFAULT,			// dimensions taken from file and if not pow2, will be
					D3DX_DEFAULT,			//		resized to next higher pow2 and filtered
					D3DX_DEFAULT,			// depth
					mipLevels,				// number of mipmap levels from a DDS file, or full chain
					0,						// usage is not rendertarget or dynamic
					D3DFMT_UNKNOWN,			// create texture based on file, but don't fail if changes are needed
					D3DPOOL_MANAGED,		//		also, compression formats would be based on the DDS file
					D3DX_DEFAULT,			// this is D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER
					mipFilter,				// this is D3DX_FILTER_BOX, bits 27-31 specify mip levels to skip
					0,						// 0 disables color key
					&imgInfo,				// pass in the info that we already obtained, could speed things up??
					NULL,
					&mD3DVolumeTexture);
			break;
		}
		default:
			debugPrintf("Texture_D3D9: Error: Incompatible Resource Type \"%s\"!\n", name().c_str());
			return false;
	}

	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		clearImageData();
		return false;
	}

	// get actuals of created texture
	D3DSURFACE_DESC sfcDesc;
	mD3DTexture->GetLevelDesc(0, &sfcDesc);
	mName = filename;
	mWidth = sfcDesc.Width;
	mHeight = sfcDesc.Height;

	// estimate size
	uint numLevels = mD3DTexture->GetLevelCount();
	mSizeB = 4 * mWidth * mHeight * imgInfo.Depth;	// rough estimate assumes 4Bpp ARGB
	uint levelSize = mSizeB;
	for (uint l = 1; l < numLevels; ++l) {
		levelSize >>= 2;
		mSizeB += levelSize;
	}
	// if using DXT compression, reduce the size
	switch (sfcDesc.Format) {
		case D3DFMT_DXT1:		// DXT1 gives 8:1 compression without alpha, 6:1 with alpha
			mSizeB /= 8; break;	// we'll assume 8:1 because DXT1 with alpha leads to artifacts, so we'll probably not use it
		case D3DFMT_DXT2: case D3DFMT_DXT3: case D3DFMT_DXT4: case D3DFMT_DXT5:
			mSizeB /= 4; break;	// DXT2-5 gives 4:1 compression
	}

	setIsManaged(true);
	mType = (imgInfo.ResourceType == D3DRTYPE_TEXTURE ? TextureType_2D :
		(imgInfo.ResourceType == D3DRTYPE_CUBETEXTURE ? TextureType_Cube : TextureType_3D));

	debugPrintf("Texture \"%s\" loaded: %ux%u, levels=%u, format=%u, sizeKB=%u\n", mName.c_str(), mWidth, mHeight,
		numLevels, sfcDesc.Format, mSizeB/1024);

	return true;
}

bool Texture_D3D9::createTexture(const void *srcData, uint srcChannels, uint srcWidth, uint srcHeight,
								 //TextureDataFormat _format, TextureUsageMode _usage, TextureRepeatMode _repeat, TextureFilterMode _filter,
								 uint mipLevels, bool compress, const string &name)
{
	_ASSERTE(srcChannels >= 1 && srcChannels <= 4);
//	_ASSERTE(_usage == TEX_USAGE_DEFAULT || _usage == TEX_USAGE_DYNAMIC || _usage == TEX_USAGE_DISPLACEMENT_MAP);

	if (initialized()) { return false; }

	/*DWORD usage = 0;
	switch (_usage) {
		case TEX_USAGE_DYNAMIC:
			usage = D3DUSAGE_DYNAMIC; break;
		case TEX_USAGE_DISPLACEMENT_MAP:
			usage = D3DUSAGE_DMAP; break;
	}
	if (_mipmap) usage |= D3DUSAGE_AUTOGENMIPMAP;*/

	D3DFORMAT format = D3DFMT_A8R8G8B8;
//	switch (_format) {
//		case TEX_FMT_UCHAR:
			//if (_compress) format = D3DFMT_DXT1; break;
			switch (srcChannels) {
				case 4: format = D3DFMT_A8R8G8B8; break;	// 32-bit ARGB pixel format with alpha, using 8 bits per channel
				case 3: format = D3DFMT_R8G8B8; break;		// 24-bit RGB pixel format with 8 bits per channel
				case 2: format = D3DFMT_A8L8; break;		// 16-bit alpha and luminance
				case 1: format = D3DFMT_A8; break;			// 8-bit alpha only
//				case 1: format = D3DFMT_L8; break;			// 8-bit luminance only
			}
//			break;

/*		case TEX_FMT_FLOAT:
//			if (_halfFloat) {	// implement this if I need to
//				switch (srcChannels) {	// 16 bits per channel
//					case 4:											// 3 and 4 channels for floating point will result in ABGR
//					case 3: format = D3DFMT_A16B16G16R16F; break;	// 64-bit float format using 16 bits for the each channel (alpha, blue, green, red)
//					case 2: format = D3DFMT_G16R16F; break;			// 32-bit float format using 16 bits for the red channel and 16 bits for the green channel
//					case 1: format = D3DFMT_R16F; break;			// 16-bit float format using 16 bits for the red channel
//				}
//			} else {
				switch (srcChannels) {	// 32 bits per channel
					case 4:
					case 3: format = D3DFMT_A32B32G32R32F; break;
					case 2: format = D3DFMT_G32R32F; break;
					case 1: format = D3DFMT_R32F; break;
				}
//			}
			break;
	}
*/
/*	DWORD filter = D3DTEXF_LINEAR;
	switch (_filter) {
		case TEX_FILTER_NEAREST:
			filter = D3DTEXF_POINT; break;
		case TEX_FILTER_LINEAR:
			filter = D3DTEXF_LINEAR; break;
		case TEX_FILTER_ANISOTROPIC:
			filter = D3DTEXF_ANISOTROPIC; break;
		case TEX_FILTER_PYRAMIDAL_QUAD:
			filter = D3DTEXF_PYRAMIDALQUAD; break;
		case TEX_FILTER_GAUSSIANQUAD:
			filter = D3DTEXF_GAUSSIANQUAD; break;
	}*/

/*	switch (_repeat) {
		case TEX_RPT_MIRROR_U:
			filter |= D3DX_FILTER_MIRROR_U; break;
		case TEX_RPT_MIRROR_V:
			filter |= D3DX_FILTER_MIRROR_V; break;
		case TEX_RPT_MIRROR:
			filter |= D3DX_FILTER_MIRROR; break;
	}*/

	// app should check IDirect3D9::CheckDeviceFormat for D3DUSAGE_RENDERTARGET and D3DUSAGE_DYNAMIC capabilities

	HRESULT hr = spD3DDevice->CreateTexture(
					srcWidth,			// Width
					srcHeight,			// Height
					mipLevels,			// Levels, 0 = full chain
					0, //usage,				// Usage
					D3DFMT_A8R8G8B8, //format,				// Format
					D3DPOOL_MANAGED,	// Pool
					&mD3DTexture,		// ppTexture
					NULL);					// pSharedHandle
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		clearImageData();
		return false;
	}

	// set filter type
//	mD3DTexture->FilterType ?

	// write buffer to texture
	D3DLOCKED_RECT lockedRect;
	hr = mD3DTexture->LockRect(0, &lockedRect, NULL, 0);

//	if (_format == TEX_FMT_UCHAR) {
		uint srcBufferSize = srcChannels * srcWidth * srcHeight;
		uint dstChannels = 4;
		uchar *dstRowData = (uchar*)lockedRect.pBits;
		uchar *srcRowData = (uchar*)srcData;
		for (uint y = 0; y < srcHeight; ++y) {
			uchar *dstAddress = dstRowData;
			uchar *srcAddress = srcRowData;
			for (uint x = 0; x < srcWidth; ++x) {
				for (uint c = 0; c < dstChannels; ++c) {
					if (c < srcChannels) {
						dstAddress[c] = srcAddress[c];
					} else {
						dstAddress[c] = 255;
					}
				}
				dstAddress += dstChannels;
				srcAddress += srcChannels;
			}
			dstRowData += lockedRect.Pitch;
			srcRowData += (srcWidth * srcChannels);
		}
/*	} else if (_format == TEX_FMT_FLOAT) {
		uchar *dstData = (uchar*)lockedRect.pBits;
		const uchar *pSrcData = (const uchar *)srcData;
		uint Bpp = 4;

		for (uint y = 0; y < srcHeight; ++y) {
			for (uint x = 0; x < srcWidth; ++x) {					
				uint dstChannels = lockedRect.Pitch / (srcWidth*Bpp);
				uint srcIndex = (y*srcWidth*srcChannels*Bpp) + (x*srcChannels*Bpp);
				uint dstIndex = (y*lockedRect.Pitch) + (x*dstChannels*Bpp);

				for (uint c = 0; c < dstChannels; ++c) {
					for (uint b = 0; b < Bpp; ++b) {
						if (c < srcChannels) {						
							dstData[dstIndex+(c*4)+b] = pSrcData[srcIndex+(c*4)+b];
						} else {							
							dstData[dstIndex+(c*4)+b] = 0;
						}
					}
				}
			}				
		}
	}
*/
	hr = mD3DTexture->UnlockRect(0);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		clearImageData();
		return false;
	}	

	// should also use a maxLOD parameter to call mD3DTexture->SetLOD() and specifiy # levels in CreateTexture

	// check if auto mipmap generation failed, also check D3DCAPS2_CANAUTOGENMIPMAP in D3DCAPS9 
//	if (_mipmap && mD3DTexture->GetLevelCount() == 1) {
//		mD3DTexture->GenerateMipSubLevels();
//	}

	mName = name;
	mWidth = srcWidth;
	mHeight = srcHeight;
	setIsManaged(true);
	mType = TextureType_2D;

	return true;
}

bool Texture_D3D9::saveTextureToFile(const string &filename) const
{
	if (!initialized()) {
		debugPrintf("Texture_D3D9: cannot save uninitialized texture \"%s\"!\n", filename.c_str());
		return false;
	}

	D3DXIMAGE_FILEFORMAT format;
	// find the last dot in the string, starts the extension
	int dotIdx = filename.find_last_of('.');
	if (dotIdx == string::npos) {
		debugPrintf("Texture_D3D9: extension not found for save!\n");
		return false;
	}
	// save to matching format
	string extStr = filename.substr(dotIdx);
	if (_stricmp(extStr.c_str(), ".dds") == 0) {
		format = D3DXIFF_DDS;
	} else if (_stricmp(extStr.c_str(), ".tga") == 0) {
		format = D3DXIFF_TGA;
	} else if (_stricmp(extStr.c_str(), ".png") == 0) {
		format = D3DXIFF_PNG;
	} else if (_stricmp(extStr.c_str(), ".bmp") == 0) {
		format = D3DXIFF_BMP;
	} else if (_stricmp(extStr.c_str(), ".jpg") == 0) {
		format = D3DXIFF_JPG;
	} else {
		debugPrintf("Texture_D3D9: extension not recognized for save!\n");
		return false;
	}

	HRESULT hr;
	hr = D3DXSaveTextureToFileA(filename.c_str(), format, mD3DTexture, 0);
	
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------
	onLoad is called automatically by the resource caching system when
	a resource is first loaded from disk and added to the cache. This
	function takes important loading parameters directly from the file
	like width, height, and format. The full mipmap chain will be
	created. For DDS files, the number of levels is taken directly from
	the file, so the only way to ensure mipmaps will not be generated
	is to store the texture as DDS with only 1 level. For finer control
	over the number of levels loaded, use one of the other load
	functions and manually inject into the cache.
---------------------------------------------------------------------*/
bool Texture_D3D9::onLoad(const BufferPtr &dataPtr, bool async)
{
	if (initialized()) { return false; }

	// get image info from the file in memory
	D3DXIMAGE_INFO imgInfo;
	HRESULT hr = D3DXGetImageInfoFromFileInMemory(
					(LPCVOID)dataPtr.get(),
					sizeB(),
					&imgInfo);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}
	
	UINT mipLevels = D3DX_DEFAULT;		// same as 0, full mipmap chain by default
	// if a DDS file, specify the number of mip levels to load, taken from the file
	// for other file types, the full mipmap chain is generated by default
	if (imgInfo.ImageFileFormat == D3DXIFF_DDS) {
		mipLevels = imgInfo.MipLevels;	// this will probably be either 1, or the # for a full chain down to 1x1
	}

	// in debug mode, check if the texture is being resized or format changed and dump the
	// before/after values to the console
	#ifdef _DEBUG
	debugCheckReqs(imgInfo, 0);
	#endif

	// Load the right type of texture based on the file
	switch (imgInfo.ResourceType) {
		case D3DRTYPE_TEXTURE: {
			// attempt create the 2d texture
			hr = D3DXCreateTextureFromFileInMemoryEx(
					spD3DDevice,
					(LPCVOID)dataPtr.get(),	// pointer to the data in memory
					sizeB(),				// size in bytes of the file
					D3DX_DEFAULT,			// dimensions taken from file and if not pow2, will be
					D3DX_DEFAULT,			//		resized to next higher pow2 and filtered
					mipLevels,				// load # mipmaps in DDS file, or the full mipmap chain for other file types
					0,						// usage is not rendertarget or dynamic
					D3DFMT_UNKNOWN,			// create texture based on file, but don't fail if changes are needed
					D3DPOOL_MANAGED,		//		also, compression formats would be based on the DDS file
					D3DX_DEFAULT,			// this is D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER
					D3DX_DEFAULT,			// same as D3DX_FILTER_BOX
					0,						// 0 disables color key
					&imgInfo,				// pass in the info that we already obtained, could speed things up??
					NULL,
					&mD3DTexture);
			break;
		}
		case D3DRTYPE_CUBETEXTURE: {
			// attempt create the Cube texture
			hr = D3DXCreateCubeTextureFromFileInMemoryEx(
					spD3DDevice,
					(LPCVOID)dataPtr.get(),	// pointer to the data in memory
					sizeB(),				// size in bytes of the file
					D3DX_DEFAULT,			// dimensions taken from file and if not pow2, will be resized to next higher pow2 and filtered
					mipLevels,				// load # mipmaps in DDS file, or the full mipmap chain for other file types
					0,						// usage is not rendertarget or dynamic
					D3DFMT_UNKNOWN,			// create texture based on file, but don't fail if changes are needed
					D3DPOOL_MANAGED,		//		also, compression formats would be based on the DDS file
					D3DX_DEFAULT,			// this is D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER
					D3DX_DEFAULT,			// same as D3DX_FILTER_BOX
					0,						// 0 disables color key
					&imgInfo,				// pass in the info that we already obtained, could speed things up??
					NULL,
					&mD3DCubeTexture);
			break;
		}
		case D3DRTYPE_VOLUMETEXTURE: {
			// look in MaxVolumeExtent in D3DCAPS9 for compatibility, skip some mip levels if possible to fit
			// attempt create the 3d texture
			hr = D3DXCreateVolumeTextureFromFileInMemoryEx(
					spD3DDevice,
					(LPCVOID)dataPtr.get(),	// pointer to the data in memory
					sizeB(),				// size in bytes of the file
					D3DX_DEFAULT,			// dimensions taken from file and if not pow2, will be
					D3DX_DEFAULT,			//		resized to next higher pow2 and filtered
					D3DX_DEFAULT,			// depth
					mipLevels,				// load # mipmaps in DDS file, or the full mipmap chain for other file types
					0,						// usage is not rendertarget or dynamic
					D3DFMT_UNKNOWN,			// create texture based on file, but don't fail if changes are needed
					D3DPOOL_MANAGED,		//		also, compression formats would be based on the DDS file
					D3DX_DEFAULT,			// this is D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER
					D3DX_DEFAULT,			// same as D3DX_FILTER_BOX
					0,						// 0 disables color key
					&imgInfo,				// pass in the info that we already obtained, could speed things up??
					NULL,
					&mD3DVolumeTexture);
			break;
		}
		default:
			debugPrintf("Texture_D3D9: Error: Incompatible Resource Type \"%s\"!\n", name().c_str());
			return false;
	}

	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		clearImageData();
		return false;
	}

	// get actuals of created texture
	D3DSURFACE_DESC sfcDesc;
	mD3DTexture->GetLevelDesc(0, &sfcDesc);
	mWidth = sfcDesc.Width;
	mHeight = sfcDesc.Height;
	setIsManaged(true);
	mType = (imgInfo.ResourceType == D3DRTYPE_TEXTURE ? TextureType_2D :
		(imgInfo.ResourceType == D3DRTYPE_CUBETEXTURE ? TextureType_Cube : TextureType_3D));

	debugPrintf("Texture \"%s\" loaded: %ux%u, levels=%u, format=%u, sizeKB=%u\n", mName.c_str(), mWidth, mHeight,
		mD3DTexture->GetLevelCount(), sfcDesc.Format, mSizeB/1024);

	return true;
}

void Texture_D3D9::debugCheckReqs(const _D3DXIMAGE_INFO &imgInfo, uint mipLevels) const
{
	UINT checkWidth = imgInfo.Width;
	UINT checkHeight = imgInfo.Height;
	UINT checkMipLvls = mipLevels;
	D3DFORMAT checkFormat = imgInfo.Format;
	HRESULT hr = D3DXCheckTextureRequirements(
					spD3DDevice,
					&checkWidth,
					&checkHeight,
					&checkMipLvls,
					0,
					&checkFormat,
					D3DPOOL_MANAGED);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
	} else if (checkWidth != imgInfo.Width || checkHeight != imgInfo.Height || checkFormat != imgInfo.Format)
	{
		debugPrintf("Texture_D3D9: \"%s\" requirements changed\n", name().c_str());
		debugPrintf("    old: %4ux%4u, format=%i, levels=%u\n", imgInfo.Width, imgInfo.Height, imgInfo.Format, mipLevels);
		debugPrintf("    new: %4ux%4u, format=%i, levels=%u\n", checkWidth, checkHeight, checkFormat, checkMipLvls);
	}
}

/*---------------------------------------------------------------------
	releases the D3D9 texture resource and sets mInitialized false
---------------------------------------------------------------------*/
void Texture_D3D9::clearImageData()
{
	switch (mType) {
		case TextureType_2D: SAFE_RELEASE(mD3DTexture); break;
		case TextureType_3D: SAFE_RELEASE(mD3DVolumeTexture); break;
		case TextureType_Cube: SAFE_RELEASE(mD3DCubeTexture); break;
	}
	mType = TextureType_None;
}

Texture_D3D9::~Texture_D3D9()
{
	clearImageData();
}