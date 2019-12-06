/* ----==== RENDEROBJECT_D3D9.H ====---- */

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN	// defined in project settings
#endif

#include <d3d9.h>
#include <vector>
#include <memory>
#include "Resource_D3D9.h"
#include "RenderOptions.h"
#include "Scene.h"
#include "../Utility/TypeDefs.h"

using std::vector;
using std::shared_ptr;

///// STRUCTURES /////

// forward declarations
class Mesh_D3D9;
typedef shared_ptr<Mesh_D3D9>		MeshPtr;
class Material_D3D9;
typedef shared_ptr<Material_D3D9>	MaterialPtr;
typedef vector<MaterialPtr>			MaterialPtrList;
struct D3DXFRAME_DERIVED;
struct D3DXMESHCONTAINER_DERIVED;
struct ID3DXAnimationController;

/*=============================================================================
class VertexBuffer_D3D9
=============================================================================*/
class VertexBuffer_D3D9 {
	friend class RenderManager_D3D9;
	friend class Mesh_D3D9;
	private:
		///// VARIABLES /////
		IDirect3DVertexBuffer9 *	m_pVertexBuffer;
		void *						m_lpVertices;
		ushort						mNumVerts;
		RenderVertexFormat			mFormat;

	public:
		///// FUNCTIONS /////
		// Constructor / destructor
		explicit VertexBuffer_D3D9() :
			m_pVertexBuffer(0), m_lpVertices(0), mNumVerts(0), mFormat(RNDR_RVF_UNSPECIFIED)
		{}
		~VertexBuffer_D3D9() { SAFE_RELEASE(m_pVertexBuffer); }
};

/*=============================================================================
class IndexBuffer_D3D9
=============================================================================*/
class IndexBuffer_D3D9 {
	friend class RenderManager_D3D9;
	friend class Mesh_D3D9;
	private:
		///// VARIABLES /////
		IDirect3DIndexBuffer9 *	m_pIndexBuffer;
		void *					m_lpIndices;
		ushort					mNumVertices;	// number of unique vertices referenced by this buffer
		ushort					mNumPrimitives;	// number of primitives drawn with this buffer
		D3DPRIMITIVETYPE		mPrimitiveType;

	public:
		///// FUNCTIONS /////
		// Constructor / destructor
		explicit IndexBuffer_D3D9() :
			m_pIndexBuffer(0), m_lpIndices(0), mNumVertices(0),
			mNumPrimitives(0), mPrimitiveType(D3DPT_TRIANGLESTRIP)
		{}
		explicit IndexBuffer_D3D9(ushort numVertices, ushort numPrimitives, D3DPRIMITIVETYPE primitiveType) :
			m_pIndexBuffer(0), m_lpIndices(0), mNumVertices(numVertices),
			mNumPrimitives(numPrimitives), mPrimitiveType(primitiveType)
		{}
		~IndexBuffer_D3D9() { SAFE_RELEASE(m_pIndexBuffer); }
};

/*=============================================================================
class Mesh_D3D9
	A mesh represents a model subset
=============================================================================*/
class Mesh_D3D9 : public Resource_D3D9 {
	private:
		///// VARIABLES /////
		D3DXFRAME_DERIVED *			mRootFrame;		// the frame tree, each frame has a unique index for the list
		D3DXFRAME_DERIVED **		mpFrameList;	// array of pointers to frames contained in the frame tree
		ushort						mNumFrames;
		ID3DXAnimationController *	mpAnimController;

		MaterialPtrList				mDefaultMaterial;	// List of MaterialPtr's includes effect defaults, EffectPtr's,
														// and TexturePtr's required by the mesh.
		bool	mInitialized;		// true if initialization succeeds
		bool	mLoadAsync;			// true if onLoad passed 'true' in async, load child resources asynchronously

		// initialization flags
		enum MeshInitFlags {
			INIT_MESH = 0,
			INIT_FRAMELIST,
			INIT_MAX
		};
		bitset<INIT_MAX>	mInitFlags;

		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Traverse the frame hierarchy in sibling - child order, assigning
			index values to the frames and adding to the list.
		---------------------------------------------------------------------*/
		void	enumFrames(D3DXFRAME_DERIVED *pFrame, ushort parentIndex, ushort &index);

		/*---------------------------------------------------------------------
			Called once after loading to assign frame index values and build
			the frame list from the tree.
		---------------------------------------------------------------------*/
		void	buildFrameList();

		/*---------------------------------------------------------------------
			Called once after loading to build default materials from the mesh.
			See CPP file description for more detail on loadResourcesFromDisk.
		---------------------------------------------------------------------*/
		void	buildDefaultMaterialList(bool async, bool loadResourcesFromDisk = false);

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
		static const ResCacheType	sCacheType = ResCache_Mesh;

		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Loads a mesh directly from a file (not through the ResourceSource
			interface. Use this with the cache injection method. See CPP file
			for more details.
		---------------------------------------------------------------------*/
		bool	loadFromXFile(const string &filename);

		/*---------------------------------------------------------------------
			Fixes the references to textures and effects in a file loaded from
			disk to refer to those child resources within the caching system.
			If null is passed, that path or extension will not be replaced.
		---------------------------------------------------------------------*/
		void	cleanResourceReferences(const char *texturePath = 0, const char *textureExtension = 0,
										const char *effectPath = 0, const char *effectExtension = 0);
		
		/*---------------------------------------------------------------------
			This will save the stored mesh to a .X file compatible with this
			engine.
		---------------------------------------------------------------------*/
		bool	saveToXFile(const string &newFilename) const;

		/*---------------------------------------------------------------------
			onLoad is called automatically by the resource caching system when
			a resource is first loaded from disk and added to the cache.
		---------------------------------------------------------------------*/
		virtual bool	onLoad(const BufferPtr &dataPtr, bool async);

		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		HRESULT	setRenderMethod(D3DXMESHCONTAINER_DERIVED *pMeshContainer,
								int materialIndex/*, cRenderMethod *pMethodSet*/);
		
		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		void	renderSubset(int iFrame, int iSubset) const;

		// Constructors
		/*---------------------------------------------------------------------
			constructor with this signature is required for the resource system
		---------------------------------------------------------------------*/
		explicit Mesh_D3D9(const string &name, uint sizeB, const ResCachePtr &resCachePtr);
		
		/*---------------------------------------------------------------------
			use this default constructor to create the texture without caching,
			or for cache injection method.
		---------------------------------------------------------------------*/
		explicit Mesh_D3D9();

		// Destructor
		~Mesh_D3D9();
};

/*=============================================================================
class ModelInstance_D3D9
=============================================================================*/
class ModelInstance_D3D9 : public SceneNode {
	private:
		///// VARIABLES /////
		MeshPtr				mMeshPtr;	// contains mesh geometry
		MaterialPtrList		mMaterial;	// one instance material per mesh subset
		// bone hierarchy data, instance transformations

	public:
		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			the callback submitted to the render queue to render the model
			instance using unique animation and materials
		---------------------------------------------------------------------*/
		void drawSubset(uint i) const
		{
			// for each subset
				// bind material vars to effect params
				// create skinning matrix array, pass to effect also
				// draw the index/vertex buffer combination for the subset
					// * draws in object space because the object to world matrix created by scene graph already
			// next
		}

		// Constructor / destructor
		explicit ModelInstance_D3D9() : SceneNode() {}
		~ModelInstance_D3D9() {}
};