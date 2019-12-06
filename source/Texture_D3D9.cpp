/*----==== TEXTURE_D3D9.CPP ====----
	Author: Jeff Kiah
	Date:	4/13/2007
----------------------------------*/

#include <d3d9.h>
#include <d3dx9.h>
#include "Texture_D3D9.h"
#include "D3D9Debug.h"


////////// class Texture_D3D9 //////////

///// STATIC VARIABLES /////

IDirect3DDevice9 *Texture_D3D9::d3dDevice = 0;	// temp solution


///// FUNCTIONS /////

void Texture_D3D9::clearImageData(void)
{
	SAFE_RELEASE(d3dTexture);
	initialized = 0;
}


bool Texture_D3D9::loadFromFile(const std::wstring &_filename, TextureRepeatMode _repeat, TextureFilterMode _filter,
								bool _mipmap, bool _compress)//, IDirect3DDevice9 *d3dDevice)
{
	if (initialized == 1) return false;

	name = _filename;

	DWORD filter = D3DX_DEFAULT;
	UINT mipLevels = _mipmap ? D3DX_DEFAULT : 1;
	D3DFORMAT format = _compress ? D3DFMT_DXT1 : D3DFMT_FROM_FILE;

	switch (_filter) {
		case TEX_FILTER_NONE:
			filter = D3DX_FILTER_NONE; break;
		case TEX_FILTER_NEAREST:
			filter = D3DX_FILTER_POINT; break;
		case TEX_FILTER_LINEAR:
			filter = D3DX_FILTER_LINEAR; break;
		// how do I handle Anisotropic or the others here??
	}

	switch (_repeat) {
		case TEX_RPT_MIRROR_U:
			filter |= D3DX_FILTER_MIRROR_U; break;
		case TEX_RPT_MIRROR_V:
			filter |= D3DX_FILTER_MIRROR_V; break;
		case TEX_RPT_MIRROR:
			filter |= D3DX_FILTER_MIRROR; break;
	}

	HRESULT hr;
	hr = D3DXCreateTextureFromFileEx(	d3dDevice,
										_filename.c_str(),
										D3DX_DEFAULT,
										D3DX_DEFAULT,
										mipLevels,
										0,
										format,
										D3DPOOL_MANAGED,
										filter,
										D3DX_DEFAULT,
										0, 0, 0,
										&d3dTexture);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);	
		clearImageData();
		return false;
	}

	// use pSrcInfo struct to set remaining base class members
	
	initialized = 1;
	return true;
}

bool Texture_D3D9::createTexture(const void *srcData, uint srcChannels, uint srcWidth, uint srcHeight,
				TextureDataFormat _format, TextureUsageMode _usage, TextureRepeatMode _repeat, TextureFilterMode _filter,
				bool _mipmap, bool _compress, const std::wstring &_name)
{
	_ASSERTE(srcChannels >= 1 && srcChannels <= 4);
	_ASSERTE(_usage == TEX_USAGE_DEFAULT || _usage == TEX_USAGE_DYNAMIC || _usage == TEX_USAGE_DISPLACEMENT_MAP);

	if (initialized == 1) return false;

	DWORD usage = 0;
	switch (_usage) {
		case TEX_USAGE_DYNAMIC:
			usage = D3DUSAGE_DYNAMIC; break;
		case TEX_USAGE_DISPLACEMENT_MAP:
			usage = D3DUSAGE_DMAP; break;
	}
	if (_mipmap) usage |= D3DUSAGE_AUTOGENMIPMAP;

	D3DFORMAT format = D3DFMT_A8R8G8B8;
	switch (_format) {
		case TEX_FMT_UCHAR:
			if (_compress) format = D3DFMT_DXT1; break;
			switch (srcChannels) {
				case 4: format = D3DFMT_A8R8G8B8; break;	// 32-bit ARGB pixel format with alpha, using 8 bits per channel
				case 3: format = D3DFMT_R8G8B8; break;		// 24-bit RGB pixel format with 8 bits per channel
				case 2: format = D3DFMT_A8L8; break;		// 16-bit alpha and luminance
				case 1: format = D3DFMT_A8; break;			// 8-bit alpha only
//				case 1: format = D3DFMT_L8; break;			// 8-bit luminance only
			}
			break;

		case TEX_FMT_FLOAT:
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

	DWORD filter = D3DTEXF_LINEAR;
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
	}

/*	switch (_repeat) {
		case TEX_RPT_MIRROR_U:
			filter |= D3DX_FILTER_MIRROR_U; break;
		case TEX_RPT_MIRROR_V:
			filter |= D3DX_FILTER_MIRROR_V; break;
		case TEX_RPT_MIRROR:
			filter |= D3DX_FILTER_MIRROR; break;
	}*/

	// app should check IDirect3D9::CheckDeviceFormat for D3DUSAGE_RENDERTARGET and D3DUSAGE_DYNAMIC capabilities

///// DEBUGGING
	uint dWidth = srcWidth;
	uint dHeight = srcHeight;
	uint dLevels = (_mipmap ? 0 : 1);
	D3DFORMAT dFormat = format;
	HRESULT hr = D3DXCheckTextureRequirements(d3dDevice, &dWidth, &dHeight, &dLevels, usage, &dFormat, D3DPOOL_MANAGED);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
	}
/////

	//HRESULT hr;
	hr = d3dDevice->CreateTexture(	srcWidth,			// Width
									srcHeight,			// Height
									(_mipmap ? 0 : 1),	// Levels
									usage,				// Usage
									format,				// Format
									D3DPOOL_MANAGED,	// Pool
									&d3dTexture,		// ppTexture
									0);					// pSharedHandle

	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		clearImageData();
		return false;
	}

	// set filter type
//	d3dTexture->FilterType ?

	// write buffer to texture
	D3DLOCKED_RECT lockedRect;
	hr = d3dTexture->LockRect(0, &lockedRect, 0, 0);

	if (_format == TEX_FMT_UCHAR) {
		uchar *dstData = (uchar*)lockedRect.pBits;
		const uchar *pSrcData = (const uchar *)srcData;

		for (uint y = 0; y < srcHeight; ++y) {
			for (uint x = 0; x < srcWidth; ++x) {					
				int srcIndex = (y*srcWidth*srcChannels) + (x*srcChannels);
				int dstIndex = (y*lockedRect.Pitch) + (x*(lockedRect.Pitch/srcWidth));

				uint dstChannels = lockedRect.Pitch / srcWidth;
				for (uint c = 0; c < dstChannels; ++c) {
					if (c < srcChannels) {
						dstData[dstIndex+c] = pSrcData[srcIndex+c];
					} else {
						dstData[dstIndex+c] = 255;
					}
				}
			}				
		}
	} else if (_format == TEX_FMT_FLOAT) {
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

	hr = d3dTexture->UnlockRect(0);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		clearImageData();
		return false;
	}	

	// should also use a maxLOD parameter to call d3dTexture->SetLOD() and specifiy # levels in CreateTexture

	// check if auto mipmap generation failed, also check D3DCAPS2_CANAUTOGENMIPMAP in D3DCAPS9 
	if (_mipmap && d3dTexture->GetLevelCount() == 1) {
		d3dTexture->GenerateMipSubLevels();
	}

	name = _name;
	width = srcWidth;
	height = srcHeight;

	initialized = 1;
	return true;
}

bool Texture_D3D9::saveTextureToFile(const std::wstring &_filename, TextureFileFormat _format) const
{
	D3DXIMAGE_FILEFORMAT format = D3DXIFF_TGA;
	switch (_format) {		
		case TEX_FILE_JPG:
			format = D3DXIFF_JPG; break;
		case TEX_FILE_BMP:
			format = D3DXIFF_BMP; break;
		case TEX_FILE_DDS:
			format = D3DXIFF_DDS; break;
		case TEX_FILE_PNG:
			format = D3DXIFF_PNG; break;
	}

	HRESULT hr;
	hr = D3DXSaveTextureToFile(_filename.c_str(), format, d3dTexture, 0);
	
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}

	return true;
}

Texture_D3D9::Texture_D3D9() : TextureBase(), d3dTexture(0)
{
}

Texture_D3D9::~Texture_D3D9()
{
	clearImageData();
}