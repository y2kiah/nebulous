/*----==== MATERIAL_D3D9.H ====----
	Author:		Jeff Kiah
	Orig.Date:	06/30/2009
	Rev.Date:	06/30/2009
---------------------------------*/

#pragma once

#include <d3d9types.h>
#include <vector>
#include <bitset>
#include <memory>
#include <boost/noncopyable.hpp>
#include "../Resource/ResHandle.h"

using std::vector;
using std::bitset;
using std::shared_ptr;

///// STRUCTURES /////

/*=============================================================================
class Material_D3D9
	A material is defined within mesh subset data. The intention of a material
	is to vary certain parameters to achieve different visual output for a
	given effect (.fx file). Materials are defined at the mesh level and with
	the mesh instance. The mesh contains the default material, but it can be
	overridden at each instance if desired.
=============================================================================*/
class Material_D3D9 {
	friend class CheckResourceLoadedProcess;
	public:
		///// DEFINITIONS /////
		typedef vector<ResPtr>					ResPtrList;
		typedef shared_ptr<D3DXEFFECTDEFAULT>	EffectDefaultPtr;
		typedef vector<EffectDefaultPtr>		EffectDefaultPtrList;

	private:
		///// STRUCTURES /////
		/*=====================================================================
		class CheckResourceLoadedProcess
			A list of spawned processes is maintained for the life of the
			material to protect against dereferencing a bad pointer in this
			process. If the material is destroyed first, it will call finish()
			on all processes in the list that aren't already finished.
		=====================================================================*/
		class CheckResourceLoadedProcess : public CProcess {
			public:
				enum MatResType : int {
					MatResType_Texture = 0,
					MatResType_Effect
				};
			private:
				string			mResourcePath;
				MatResType		mResType;	// MatResType_Texture or MatResType_Effect
				Material_D3D9 *	mpMaterial;

			public:
				virtual void onUpdate(float deltaMillis);
				virtual void onInitialize() {}
				virtual void onFinish() {}
				virtual void onTogglePause() {}

				explicit CheckResourceLoadedProcess(Material_D3D9 *pMat, const string &resourcePath, MatResType resType);
				virtual ~CheckResourceLoadedProcess() {}
		};

		///// VARIABLES /////
		ResPtr					mEffect;			// shared_ptr to Effect_D3D9 (default effect used if null)
		D3DMATERIAL9			mD3DMaterial;		// includes ambient, diffuse, specular, emissive colors, and specular power
		EffectDefaultPtrList	mD3DEffectDefaults;	// list of default effect parameter values
		ResPtrList				mTextures;			// list of shared_ptr to Texture_D3D9
		
		bitset<16>				mTextureFlags;		// need this???
		ushort					mTextureCount;		// number of requested textures added to material
		ProcessList				mProcessList;		// list of processes spawned to load child resources

	public:
		///// FUNCTIONS /////
		// Accessors
		/*---------------------------------------------------------------------
			Check that this returns true before using the effect for rendering.
		---------------------------------------------------------------------*/
		bool	isReadyForRender() const;

		// Mutators
		/*---------------------------------------------------------------------
			Sets material's effect. The effect reference will be retrieved from
			the resource cache. See CPP file for more details.
		---------------------------------------------------------------------*/
		bool	setEffect(const string &filename, bool async, bool assumeCached = false);
		/*---------------------------------------------------------------------
			Sets values in the D3DMATERIAL9 structure
		---------------------------------------------------------------------*/
		void	setD3DMaterial(const D3DMATERIAL9 &mat);
		/*---------------------------------------------------------------------
			Adds an effect parameter default value to the list.
		---------------------------------------------------------------------*/
		void	addEffectDefault(const D3DXEFFECTDEFAULT &def);
		/*---------------------------------------------------------------------
			Adds a texture to the end of list. Texture reference will be
			retrieved from the resource cache. See CPP file for more details.
		---------------------------------------------------------------------*/
		bool	addTexture(const string &filename, bool async, bool assumeCached = false); // SPECIFY WHICH SLOT OF THE 16 YOU'RE ADDING IT TO

		// Constructors
		explicit Material_D3D9() :
			mTextureFlags(0), mTextureCount(0)
		{
			SecureZeroMemory(&mD3DMaterial, sizeof(mD3DMaterial));
		}
		~Material_D3D9();
};