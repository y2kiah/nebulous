/*----==== RENDERMANAGER_D3D9.H ====----
	Author: Jeff Kiah
	Date:	4/13/2007
--------------------------------------*/

#pragma once

#include <d3d9.h>
#include <list>
#include <bitset>
#include <memory>
#include "RenderOptions.h"
#include "RenderObject_D3D9.h"
#include "../Event/EventListener.h"
#include "../Utility/Typedefs.h"

// TEMP UNTIL REDESIGNED
#include "Camera_D3D9.h"

using std::list;
using std::bitset;
using std::shared_ptr;

///// DECLARATIONS /////

enum RenderOption : uint {
	RNDR_OPT_CLEARSCREEN = 0,
	RNDR_OPT_CLEARZBUFFER,
	RNDR_OPT_MAX
};

class RenderEntry_D3D9;
typedef shared_ptr<RenderEntry_D3D9>	RenderEntryPtr;
typedef list<RenderEntryPtr>			RenderEntryList;
class SceneNode;
typedef shared_ptr<SceneNode>			SceneNodePtr;

///// STRUCTURES /////

/*=============================================================================
class RenderManager_D3D9
=============================================================================*/
class RenderManager_D3D9 {
	friend class RenderManagerListener;
	private:
		///// STRUCTURES /////
		/*=====================================================================
		class RenderManagerListener
		=====================================================================*/
		class RenderManagerListener : public EventListener {
			friend class RenderManager_D3D9;
			private:
				RenderManager_D3D9 &	mMgr;
				bool handleDisplayChange(const EventPtr &ePtr);
			public:
				explicit RenderManagerListener(RenderManager_D3D9 &mgr);
		};

		///// VARIABLES /////
		IDirect3D9 *				d3d;
		IDirect3DDevice9 *			d3dDevice;
		D3DPRESENT_PARAMETERS		d3dParams;
		ID3DXEffectPool *			mEffectPool;
		IDirect3DVertexDeclaration9	*mD3DVertexDecl[RNDR_RVF_MAX];

		RenderEntryList				mRenderList;	// list of submitted render objects for next frame
		bitset<RNDR_OPT_MAX>		mRenderOptions;	// various configurable options for the renderer to use
		
		RenderManagerListener		mEventListener;

		// initialization flags
		enum RenderInitFlags {
			INIT_D3D = 0,
			INIT_D3DDEVICE,
			INIT_VERT_DECL,
			INIT_EFFECT_POOL,
			INIT_MAX
		};
		bitset<INIT_MAX>	mInitFlags;

		///// FUNCTIONS /////
		bool	renderEntry(RenderEntry_D3D9 &pEntry, const Camera_D3D9 &pCam) const;
		bool	createVertexDeclarations();
		void	initRenderState();
		bool	buildPresentParams(int bpp, int resX, int resY, int refreshRate,
								   bool fullscreen, bool vsync);
		HRESULT	resetRenderDevice();

	public:
		// Common

		// RenderObject management
		void	submitForRendering();
		void	resetSubmitList() {}
		void	prepareSubmitList();
		bool	fillVertexData(uint roID, const void *_data, int _numVerts, RenderVertexFormat _vFormat) const;
		bool	addIndexBuffer(uint roID, const void *_data, int _numIndices, int _indexSizeInBytes,
							   ushort _numVertices, ushort _numPrimitives, RenderPrimitiveType _primitiveType) const;

		// RenderManager methods
		bool	initRenderDevice(int bpp, int resX, int resY, int refreshRate,
								 bool fullscreen, bool vsync);
		void	freeRenderDevice();
		bool	makeDisplayChanges(int bpp, int resX, int resY, int refreshRate,
								   bool fullscreen, bool vsync);
		bool	render(const Camera_D3D9 &cam) const;
		bool	handleLostDevice();
		
		void	initVolatileResources();
		void	freeVolatileResources();
		const IDirect3DDevice9 *getDevice() const { return d3dDevice; }

		// Constructor / Destructor

		explicit RenderManager_D3D9();
		~RenderManager_D3D9();
};

/*=============================================================================
class RenderEntry_D3D9
	Direct3D9 implementation of a render queue entry. These represent what,
	where, and how something is to be rendered. It will be sorted by the render
	manager based on the material and mesh to maximize efficiency. Order of
	sort:
		1)	render target
		2)	render pass, separate rendering into pre-determined passes
		3)	
=============================================================================*/
class RenderEntry_D3D9 {
	friend class RenderManager_D3D9;
	friend struct SortRenderEntry;
	public:
		///// STRUCTURES /////
		/*=====================================================================
			This functor is for use with sort algorithm
		=====================================================================*/
		struct SortRenderEntry {
			bool operator()(
				const RenderEntry_D3D9 &re1,
				const RenderEntry_D3D9 &re2) const
			{
				if (re1.mSort1 > re2.mSort1) { return false; }
				else if (re1.mSort1 < re2.mSort1) { return true; }
				if (re1.mSort2 > re2.mSort2) { return false; }
				else if (re1.mSort2 < re2.mSort2) { return true; }
				if (re1.mSort3 > re2.mSort3) { return false; }
				else if (re1.mSort3 < re2.mSort3) { return true; }
				if (re1.mSort4 > re2.mSort4) { return false; }
				else if (re1.mSort4 < re2.mSort4) { return true; }
				return false;
			}
		};

	private:
		///// VARIABLES /////
		#pragma pack(1) // ensure byte packing
		union {	// gives 128 bits (16 bytes) to sort by
			struct {
				uint	mSort1;
				uint	mSort2;
				uint	mSort3;
				uint	mSort4;
			};
			struct {
				// the following make up high priority mSort1 values, listed in reverse priority
				ushort	mEffectID;				// 16 bits, up to 65536 unique effects to sort by
				uchar	mDepthSortType	: 4;	// 4 bits, 0 = no sort, 1 = back to front, 2 = front to back
				uchar	mRenderPass		: 4;	// 4 bits, up to 16 passes through the scene graph per render target to sort by
				uchar	mRenderTarget;			// 8 bits, up to 256 render targets to sort by
				// the following make up mid high priority mSort2 values, listed in reverse priority
				ushort	mModelID;				// 16 bits, ID representing a vertex/index buffer combination
				uchar	mModelType;				// 8 bits, 0 = mesh subset, 1 = vertex/index buffer combination
				uchar	mEffectPass;			// 8 bits, up to 256 passes in the effect's technique
				// the following make up mid low priority mSort3 values, listed in reverse priority
				ushort	mModelParam2;			// 16 bits, model subset index, or index buffer index
				ushort	mModelParam1;			// 16 bits, model frame index, or vertex buffer index
				// the following make up low priority mSort4 values, listed in reverse priority
				ushort	mTexture1ID;			// 16 bits, up to 65536 unique texture 1 bindings to sort by
				ushort	mMaterialID;			// 16 bits, up to 65536 unique materials to sort by
			};
		};
		// unsorted fields (
		SceneNodePtr	mSceneNode;

		#pragma pack()

	public:
		///// FUNCTIONS /////
		// Accessors

		// Mutators
		void	clear() {
			mSort1 = mSort2 = mSort3 = mSort4 = 0;
			mSceneNode.reset();
		}

		// Contructor / Destructor
		explicit RenderEntry_D3D9() :
			mSort1(0), mSort2(0), mSort3(0), mSort4(0)
		{}
		~RenderEntry_D3D9() {}
};