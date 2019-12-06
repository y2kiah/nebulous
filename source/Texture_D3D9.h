/* ----==== TEXTURE_D3D9.H ====---- */

#pragma once

#include "Texture.h"
#include "Utility Code\Typedefs.h"


///// STRUCTURES /////

class Texture_D3D9 : public TextureBase {
	private:
		///// VARIABLES /////

		static IDirect3DDevice9		*d3dDevice; // temporary solution

		IDirect3DTexture9	*d3dTexture;

	public:
		///// FUNCTIONS /////

		// Mutators

		// temporary solution until I can figure out how to share this info amongst D3D classes
		static void		setIDirect3DDevice9(IDirect3DDevice9 *pDevice) { d3dDevice = pDevice; }

		void	clearImageData(void);

		bool	loadFromFile(const std::wstring &_filename, TextureRepeatMode _repeat, TextureFilterMode _filter,
					bool _mipmap, bool _compress);//, IDirect3DDevice9 *d3dDevice);

		bool	createTexture(const void *srcData, uint srcChannels, uint srcWidth, uint srcHeight,
					TextureDataFormat _format, TextureUsageMode _usage, TextureRepeatMode _repeat, TextureFilterMode _filter,
					bool _mipmap, bool _compress, const std::wstring &_name);

		bool	createRenderTarget() {}
		bool	createDepthStencil() {}

		bool	saveTextureToFile(const std::wstring &_filename, TextureFileFormat _format) const;

		// Constructor / Destructor
		explicit Texture_D3D9();
		~Texture_D3D9();
};