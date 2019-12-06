/*----==== EFFECT_D3D9.H ====----
	Author:		Jeff Kiah
	Orig.Date:	06/30/2009
	Rev.Date:	07/08/2009
-------------------------------*/

#pragma once

#include "Resource_D3D9.h"
#include <d3dx9shader.h>
#include <bitset>
#include <vector>
#include <memory>

using std::bitset;
using std::vector;
using std::shared_ptr;

///// STRUCTURES /////

// forward declarations
class CheckResourceLoadedProcess;
struct ID3DXEffect;
struct ID3DXEffectPool;

class Matrix4x4;
class Vector3;
class Vector4;
class Texture_D3D9;

/*=============================================================================
class Effect_D3D9
=============================================================================*/
class Effect_D3D9 : public Resource_D3D9 {
	friend class RenderManager_D3D9;	// allows access to spEffectPool
	friend class CheckResourceLoadedProcess;
	public:
		///// DEFINITIONS /////
		typedef vector<ResPtr>			ResPtrList;
		typedef vector<D3DXSEMANTIC>	VertexInputList;

		// enums of SAS semantic names for indexing handle tables
		enum MatrixHandle : int {
			MatrixHndl_world = 0,		MatrixHndl_worldInv,		MatrixHndl_worldTrans,			MatrixHndl_worldInvTrans,
			MatrixHndl_view,			MatrixHndl_viewInv,			MatrixHndl_viewTrans,			MatrixHndl_viewInvTrans,
			MatrixHndl_proj,			MatrixHndl_projInv,			MatrixHndl_projTrans,			MatrixHndl_projInvTrans,
			MatrixHndl_worldView,		MatrixHndl_worldViewInv,	MatrixHndl_worldViewTrans,		MatrixHndl_worldViewInvTrans,
			MatrixHndl_viewProj,		MatrixHndl_viewProjInv,		MatrixHndl_viewProjTrans,		MatrixHndl_viewProjInvTrans,
			MatrixHndl_worldViewProj,	MatrixHndl_worldViewProjInv,MatrixHndl_worldViewProjTrans,	MatrixHndl_worldViewProjInvTrans,
			MatrixHndl_MAX
		};
		enum VectorHandle : int {
			VectorHndl_lightPos = 0,	VectorHndl_lightDir,		VectorHndl_diffuse,				VectorHndl_specular,
			VectorHndl_ambient,			VectorHndl_emissive,		VectorHndl_viewportPixelSize,
			VectorHndl_MAX
		};
		enum ScalarHandle : int {
			ScalarHndl_power = 0,		ScalarHndl_specularPower,	ScalarHndl_constAtten,			ScalarHndl_linearAtten,
			ScalarHndl_quadAtten,		ScalarHndl_falloffAngle,	ScalarHndl_falloffExponent,		ScalarHndl_emission,
			ScalarHndl_opacity,			ScalarHndl_refraction,
			ScalarHndl_MAX
		};
		enum TextureHandle : int {
			TextureHndl_rndrDSTarget=0,	TextureHndl_rndrClrTarget,	TextureHndl_diffuseMap,			TextureHndl_specularMap,
			TextureHndl_normalMap,		TextureHndl_environment,	TextureHndl_envMap,				TextureHndl_envNormal,
			TextureHndl_MAX
		};
		
		static const char *sSASMatrixSemanticNames[];
		static const char *sSASVectorSemanticNames[];
		static const char *sSASScalarSemanticNames[];
		static const char *sSASTextureSemanticNames[];

	private:
		/*=====================================================================
		class CheckResourceLoadedProcess
			A list of spawned processes is maintained for the life of the
			effect to protect against dereferencing a bad pointer in this
			process. If the effect is destroyed first, it will call finish()
			on all processes in the list that aren't already finished.
		=====================================================================*/
		class CheckResourceLoadedProcess : public CProcess {
			private:
				string			mResourcePath;
				D3DXHANDLE		mParamHandle;
				Effect_D3D9 *	mpEffect;

			public:
				virtual void onUpdate(float deltaMillis);
				virtual void onInitialize() {}
				virtual void onFinish() {}
				virtual void onTogglePause() {}

				explicit CheckResourceLoadedProcess(Effect_D3D9 *pEff, D3DXHANDLE paramHandle,
													const string &resourcePath);
				virtual ~CheckResourceLoadedProcess() {}
		};

		///// VARIABLES /////
		ID3DXEffect *		mD3DEffect;		// effect interface
		D3DXHANDLE			mhTechnique;	// handle to the effect's technique used to render
		D3DXEFFECT_DESC		mEffectDesc;	// effect description structure
		D3DXTECHNIQUE_DESC	mTechniqueDesc;	// description of the technique chosen at load time

		D3DXHANDLE			mMatrixHandle[MatrixHndl_MAX];	// handles for fast access to properties with
		D3DXHANDLE			mVectorHandle[VectorHndl_MAX];	// DXSAS compliant semantics
		D3DXHANDLE			mScalarHandle[ScalarHndl_MAX];
		D3DXHANDLE			mTextureHandle[TextureHndl_MAX];// see CPP file comments in parseKnownHandles for
															// special-case texture semantics
		ResPtrList			mResources;	// optional list of resources (textures, scripts) that are specified in the
										// .fx file annotations. These can be overridden by material-specific textures,
										// but can act as proxy textures, or for any effect-constant textures where the
										// texture semantic is not set to a pre-determined value
		VertexInputList		mVertexSemantics;	// vector of D3DXSEMANTIC, determines input format for vertex shader
												// http://msdn.microsoft.com/en-us/library/bb172872%28VS.85%29.aspx
		bool		mInitialized;	// true if initialization succeeds
		bool		mLoadAsync;		// true if onLoad passed 'true' in async, load child resources asynchronously
		int			mResourceCount;	// number of child resources (textures, scripts) found in parameter annotations
		ProcessList	mProcessList;	// list of processes spawned to load child resources

		// initialization flags
		enum EffectInitFlags {
			INIT_EFFECT = 0,
			INIT_TECHNIQUE,
			INIT_MAX
		};
		bitset<INIT_MAX>	mInitFlags;

		// static
		static ID3DXEffectPool *	spEffectPool; // make smart (weak) pointer

		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Called in a lost device situation for non-managed textures only.
			These will free and restore the resource.
		---------------------------------------------------------------------*/
		virtual void	onDeviceLost();
		virtual void	onDeviceRestore();

		/*---------------------------------------------------------------------
			This function finds known variables with predetermined names and
			stores a handle to them for faster access. The recognized semantics
			from the .fx file follows the DXSAS 0.86 spec. Known handles can be
			found in the CPP file. Returns true if initialization succeeds,
			false on failure.
		---------------------------------------------------------------------*/
		bool	parseKnownHandles();

		/*---------------------------------------------------------------------
			Takes an (assumed) valid parameter handle and description and loops
			through the parameter's annotations looking for recognized values.
			Child resources will be loaded when found.
			-- if both ResourceName and SourceName are found, it attempts to
				load from a registered IResourceSource object (async or sync)
		---------------------------------------------------------------------*/
		bool	parseParamAnnotations(D3DXHANDLE paramHndl, const D3DXPARAMETER_DESC &paramDesc);

		/*---------------------------------------------------------------------
			Called from parseParamAnnotations. Returns true if load successful,
			or for asynchronous loading, if waiting for load to complete.
			Returns false for synchronous loading when load fails, or on error
			for asynchronous loading.
		---------------------------------------------------------------------*/
		bool	loadTexture(D3DXHANDLE paramHndl, const string &resourcePath, D3DXPARAMETER_TYPE textureType);
		
		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		bool	loadScript(D3DXHANDLE paramHndl, const string &resourcePath);

	public:
		///// VARIABLES /////
		/*---------------------------------------------------------------------
			specifies the cache that will manager the resource
		---------------------------------------------------------------------*/
		static const ResCacheType	sCacheType = ResCache_Material;

		///// FUNCTIONS /////
		// Accessors
		/*---------------------------------------------------------------------
			Check that this returns true before using the effect for rendering.
		---------------------------------------------------------------------*/
		bool	isReadyForRender() const { return (mInitialized && (mResourceCount == mResources.size())); }

		// Mutators
		/*---------------------------------------------------------------------
			Call begin to render using the effect. Returns the number of passes
			or 0 on error. The client should loop the specified number of times
			returned and call beginPass / endPass for each iteration. endPass
			is called when finished rendering with this effect.
		---------------------------------------------------------------------*/
		uint	beginEffect() const;
		void	endEffect() const;
		void	beginPass(uint pass) const;
		void	endPass() const;

		/*---------------------------------------------------------------------
			These functions set effect parameters according to type and pre-
			determined handle
		---------------------------------------------------------------------*/
		bool	setMatrixParam(MatrixHandle h, const Matrix4x4 &m) const;
		bool	setMatrixParam(MatrixHandle h, const D3DXMATRIX *m) const;
		bool	setVectorParam(VectorHandle h, const Vector3 &v) const;
		bool	setVectorParam(VectorHandle h, const Vector4 &v) const;
		bool	setVectorParam(VectorHandle h, const D3DXVECTOR3 *v) const;
		bool	setVectorParam(VectorHandle h, const D3DXVECTOR4 *v) const;
		bool	setScalarParam(ScalarHandle h, float f) const;
		bool	setScalarParam(ScalarHandle h, int i) const;
		bool	setScalarParam(ScalarHandle h, bool b) const;
		bool	setTextureParam(uint i, const Texture_D3D9 *t) const;
		bool	setTextureParam(uint i, IDirect3DBaseTexture9 *t) const;

		/*---------------------------------------------------------------------
			Loads an effect directly from a .fx file (not through the Resource-
			Source interface. Use this with the cache injection method.
		---------------------------------------------------------------------*/
		bool	loadFromFile(const string &filename);

		/*---------------------------------------------------------------------
			onLoad is called automatically by the resource caching system when
			a resource is first loaded from disk and added to the cache.
		---------------------------------------------------------------------*/
		virtual bool	onLoad(const BufferPtr &dataPtr, bool async);

		/*---------------------------------------------------------------------
			releases the D3D9 effect resource and sets mInitialized false
		---------------------------------------------------------------------*/
		void	clearEffectData();

		// Constructors
		/*---------------------------------------------------------------------
			constructor with this signature is required for the resource system
		---------------------------------------------------------------------*/
		explicit Effect_D3D9(const string &name, uint sizeB, const ResCachePtr &resCachePtr);

		/*---------------------------------------------------------------------
			use this default constructor to create the texture without caching,
			or for cache injection method.
		---------------------------------------------------------------------*/
		explicit Effect_D3D9();

		// Destructor
		~Effect_D3D9();
};