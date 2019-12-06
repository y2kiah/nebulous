/*----==== TEXTURE_D3D9.H ====----
	Author:		Jeff Kiah
	Orig.Date:	06/21/2009
	Rev.Date:	07/07/2009
--------------------------------*/

#pragma once

#include <string>
#include "Resource_D3D9.h"
#include "../Utility/Typedefs.h"

using std::string;

///// STRUCTURES /////

struct IDirect3DTexture9;
struct IDirect3DCubeTexture9;
struct IDirect3DVolumeTexture9;
struct _D3DXIMAGE_INFO;

/*=============================================================================
class Texture_D3D9
=============================================================================*/
class Texture_D3D9 : public Resource_D3D9 {
	public:
		///// DEFINITIONS /////
		enum TextureType : int {
			TextureType_None = 0,
			TextureType_2D,
			TextureType_3D,
			TextureType_Cube
		};
	private:
		///// VARIABLES /////
		union {
			IDirect3DTexture9 *			mD3DTexture;
			IDirect3DCubeTexture9 *		mD3DCubeTexture;
			IDirect3DVolumeTexture9 *	mD3DVolumeTexture;
		};

		uint			mWidth, mHeight;
		TextureType		mType;

		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Called in a lost device situation for non-managed textures only.
			These will free and restore the resource.
		---------------------------------------------------------------------*/
		virtual void	onDeviceLost() {}
		virtual void	onDeviceRestore() {}

	public:
		///// VARIABLES /////
		/*---------------------------------------------------------------------
			specifies the cache that will manager the resource
		---------------------------------------------------------------------*/
		static const ResCacheType	sCacheType = ResCache_Material;

		///// FUNCTIONS /////
		// Accessors
		bool	initialized() const	{ return (mType != TextureType_None); }
		uint	width() const		{ return mWidth; }
		uint	height() const		{ return mHeight; }

		IDirect3DTexture9 *	getD3DTexture() const { return mD3DTexture; }

		// Mutators
		/*---------------------------------------------------------------------
			Loads a texture directly from a file (not through the Resource-
			Source interface. Use this with the cache injection method.
		---------------------------------------------------------------------*/
		bool	loadFromFile(const string &filename, uint mipLevels, bool compress);

		bool	createTexture(const void *srcData, uint srcChannels, uint srcWidth, uint srcHeight,
					//TextureDataFormat _format, TextureUsageMode _usage, TextureRepeatMode _repeat, TextureFilterMode _filter,
					uint mipLevels, bool compress, const string &name);

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
		virtual bool	onLoad(const BufferPtr &dataPtr, bool async);

		/*---------------------------------------------------------------------
			releases the D3D9 texture resource and sets mInitialized false
		---------------------------------------------------------------------*/
		void	clearImageData();

		// Misc functions
		void	debugCheckReqs(const _D3DXIMAGE_INFO &imgInfo, uint mipLevels) const;
		bool	saveTextureToFile(const string &filename) const;

		// Constructors
		/*---------------------------------------------------------------------
			constructor with this signature is required for the resource system
		---------------------------------------------------------------------*/
		explicit Texture_D3D9(const string &name, uint sizeB, const ResCachePtr &resCachePtr) :
			Resource_D3D9(name, sizeB, resCachePtr),
			mWidth(0), mHeight(0), mD3DTexture(0), mType(TextureType_None)
		{}

		/*---------------------------------------------------------------------
			use this default constructor to create the texture without caching,
			or for cache injection method.
		---------------------------------------------------------------------*/
		explicit Texture_D3D9() :
			Resource_D3D9(),
			mWidth(0), mHeight(0), mD3DTexture(0), mType(TextureType_None)
		{}

		// Destructor
		~Texture_D3D9();
};