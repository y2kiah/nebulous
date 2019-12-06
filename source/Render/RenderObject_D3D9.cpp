/*----==== RENDEROBJECT_D3D9.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	04/13/2007
	Rev.Date:	06/30/2009
---------------------------------------*/

#include "RenderObject_D3D9.h"
#include <d3dx9.h>
#include <dxfile.h>
#include <string>
#include <algorithm>
#include "D3D9Debug.h"
#include "Material_D3D9.h"
#include "Texture_D3D9.h"

using std::string;

///// STRUCTURES /////

/*=============================================================================
struct D3DXFRAME_DERIVED
=============================================================================*/
struct D3DXFRAME_DERIVED : public D3DXFRAME {
	ushort		mFrameIndex;
	ushort		mParentIndex;
	D3DXMATRIX	mCombinedMatrix;

	explicit D3DXFRAME_DERIVED() :
		D3DXFRAME(), mFrameIndex(0), mParentIndex(0)
	{
		Name = NULL; pMeshContainer = NULL;
		pFrameSibling = NULL; pFrameFirstChild = NULL;
		D3DXMatrixIdentity(&mCombinedMatrix);
	}
};

/*=============================================================================
struct D3DXMESHCONTAINER_DERIVED
=============================================================================*/
struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER {
    // Already defined in base class
	/*LPSTR                   Name;
    D3DXMESHDATA            MeshData;
    LPD3DXMATERIAL          pMaterials;
    LPD3DXEFFECTINSTANCE    pEffects;
    DWORD                   NumMaterials;
    DWORD                  *pAdjacency;
    LPD3DXSKININFO          pSkinInfo;
    struct _D3DXMESHCONTAINER *pNextMeshContainer;*/

	// SkinMesh info - From RT3D Terrain Engines book
	//D3DXMESHDATA		 RenderMeshData;
    //DWORD                NumAttributeGroups; 
    //LPD3DXATTRIBUTERANGE pAttributeTable;
    //DWORD                NumBoneInfluences;
    //int*				 pBoneIndexList;
    //LPD3DXBUFFER         pBoneCombinationBuf;
    //DWORD                NumBoneMatrices;
	D3DXMATRIX *			pBoneOffsetMatrices;
	//cRenderMethod**		 ppRenderMethodList;

	// SkinMesh info - from D3D Skinning Sample
    /*LPD3DXMESH           pOrigMesh;
    LPD3DXATTRIBUTERANGE pAttributeTable;
    DWORD                NumAttributeGroups; 
    DWORD                NumInfl;
    LPD3DXBUFFER         pBoneCombinationBuf;
    D3DXMATRIX**         ppBoneMatrixPtrs;
    D3DXMATRIX*          pBoneOffsetMatrices;
    DWORD                NumPaletteEntries;
    bool                 UseSoftwareVP;
    DWORD                iAttributeSW;*/
};

/*=============================================================================
class FrameAllocator_D3DX9
=============================================================================*/
class FrameAllocator_D3DX9 : public ID3DXAllocateHierarchy {
	private:
		///// VARIABLES /////
		uint mFrameCount;
		uint mNumBones;

		///// FUNCTIONS /////
		static HRESULT AllocateName(const char *source, char **dest);

	public:
		// Interface functions
		STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
		STDMETHOD(CreateMeshContainer)(THIS_
			LPCSTR Name,
			CONST D3DXMESHDATA *pMeshData,
			CONST D3DXMATERIAL *pMaterials,
			CONST D3DXEFFECTINSTANCE *pEffectInstances,
			DWORD NumMaterials,
			CONST DWORD *pAdjacency,
			LPD3DXSKININFO pSkinInfo,
			LPD3DXMESHCONTAINER *ppNewMeshContainer);
		STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
		STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);

		// Accessors
		uint	frameCount() const	{ return mFrameCount; }
		uint	numBones() const	{ return mNumBones; }

		// Constructor / destructor
		explicit FrameAllocator_D3DX9() :
			mFrameCount(0), mNumBones(0)
		{}
		~FrameAllocator_D3DX9() {}
};

/*=============================================================================
class UserDataLoader_D3DX9
=============================================================================*/
class UserDataLoader_D3DX9 : public ID3DXLoadUserData {
	STDMETHOD(LoadTopLevelData)(LPD3DXFILEDATA pXofChildData);
	STDMETHOD(LoadFrameChildData)(LPD3DXFRAME pFrame, LPD3DXFILEDATA pXofChildData);
	STDMETHOD(LoadMeshChildData)(LPD3DXMESHCONTAINER pMeshContainer, LPD3DXFILEDATA pXofChildData);
};

///// FUNCTIONS /////

////////// class Mesh_D3D9 //////////
/*---------------------------------------------------------------------
	Loads a mesh directly from a file (not through the ResourceSource
	interface. Use this with the cache injection method, or to process
	raw .X files then save to the native .PX format. Any referenced
	textures or effects will be loaded from the data/texture/ and
	data/effect/ directories respectively, with the filename as
	described in the .X file, and those resources will be injected into
	the cache. You should load models from the data/model/ directory,
	but that is not required.
---------------------------------------------------------------------*/
bool Mesh_D3D9::loadFromXFile(const string &filename)
{
	FrameAllocator_D3DX9 frameAlloc;
	UserDataLoader_D3DX9 userDataLoader;
	
	// Load the mesh directly from .x file
	HRESULT hr = D3DXLoadMeshHierarchyFromXA(
					filename.c_str(),
					D3DXMESH_MANAGED,
					spD3DDevice,
					(ID3DXAllocateHierarchy *)&frameAlloc,
					(ID3DXLoadUserData *)&userDataLoader,
					(LPD3DXFRAME *)&mRootFrame,
					&mpAnimController
				);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		debugPrintf("Mesh_D3D9: Error: cannot load mesh \"%s\"\n", filename.c_str());
		return false;
	}
	mInitFlags[INIT_MESH] = true; // set the init flag
	
	mName = filename;

	// rip information from the allocator objects
	mNumFrames = frameAlloc.frameCount();
	buildFrameList();
	cleanResourceReferences("data/texture", 0, "data/effect", 0); // assume paths to textures and effects
	buildDefaultMaterialList(false, true); // cause child resources to be loaded from files on disk

	setIsManaged(true);
	mInitialized = true;
	
	debugPrintf("Mesh_D3D9: Mesh \"%s\" loaded!\n", filename.c_str());
	return true;
}

/*---------------------------------------------------------------------
	Non member function to help cleanResourceReferences()
---------------------------------------------------------------------*/
void translateFilename(char **pFilename, const char *replacePath = 0,
					   const char *replaceExtension = 0)
{
	if (*pFilename && (replacePath || replaceExtension)) {
		debugPrintf("  changed \"%s\" ", *pFilename);
		// find start and stop indexes for substring
		string filename(*pFilename);
		int start = filename.find_last_of("/\\");
		start = (!replacePath || start == filename.npos ? 0 : start+1);
		int stop = filename.rfind('.');
		stop = (!replaceExtension || stop == filename.npos ? filename.npos : stop);
		// create the new string
		ostringstream ossFilename;
		if (replacePath) { ossFilename << replacePath << '/'; }
		ossFilename << filename.substr(start, stop-start);
		if (replaceExtension) { ossFilename << '.' << replaceExtension; }
		// delete the old string within the frame hierarchy and create new
		int strLen = ossFilename.str().size() + 1;
		if (strLen > 1) {
			delete [] *pFilename;
			*pFilename = new char[strLen];
			strcpy_s(*pFilename, strLen, ossFilename.str().c_str());
		}
		debugPrintf("to \"%s\"\n", *pFilename);
	}
}

/*---------------------------------------------------------------------
	Fixes the references to textures and effects in a file loaded from
	disk to refer to those child resources within the caching system.
	If null is passed, that path or extension will not be replaced.
	E.g. cleanResourceReferences("texture", "dds", "effect", "fxo")
	will cause "C:\Users\Default\Desktop\skin.tga" to become
	"texture/skin.dds" and "C:\Users\Default\Desktop\phong.fx" to
	become "effect/phong.fxo" - this would assume that two sources,
	"texture" and "effect", have been registered with the cache manager
---------------------------------------------------------------------*/
void Mesh_D3D9::cleanResourceReferences(const char *texturePath, const char *textureExtension,
										const char *effectPath, const char *effectExtension)
{
	if (!texturePath && !textureExtension && !effectPath && !effectExtension) { return; }
	debugPrintf("Mesh_D3D9: cleaning resource references...\n");

	// check all frames for meshcontainers
	for (uint f = 0; f < mNumFrames; ++f) {
		if (mpFrameList[f]->pMeshContainer) {
			// check all materials for texture filenames
			for (uint m = 0; m < mpFrameList[f]->pMeshContainer->NumMaterials; ++m) {
				// process texture filenames
				translateFilename(&mpFrameList[f]->pMeshContainer->pMaterials[m].pTextureFilename,
									texturePath, textureExtension);
				// process effect filenames
				translateFilename(&mpFrameList[f]->pMeshContainer->pEffects[m].pEffectFilename,
									effectPath, effectExtension);
			}
		}
	}
}

/*---------------------------------------------------------------------
	This will save the stored mesh to a .X file compatible with this
	engine. Generally, a higher level function (possibly for calling
	from script) would process a new, raw file by first loadFromXFile,
	then cleanResourceReferences, then optimizeMesh, then saveToXFile.
	The processed file saved to disk would be ready for inclusion in
	one of the resource packs. Use a .PX extension to indicate that the
	file is a processed .X file, saved in binary form, much like .FXO
	indicates a pre-compiled .FX effect.
---------------------------------------------------------------------*/
bool Mesh_D3D9::saveToXFile(const string &newFilename) const
{
	return true;
}

/*---------------------------------------------------------------------
	onLoad is called automatically by the resource caching system when
	a resource is first loaded from disk and added to the cache.
---------------------------------------------------------------------*/
bool Mesh_D3D9::onLoad(const BufferPtr &dataPtr, bool async)
{
	setIsManaged(true);
	mInitialized = true;

	return true;
}

/*---------------------------------------------------------------------
	Traverse the frame hierarchy in sibling - child order, assigning
	index values to the frames and adding to the list.
---------------------------------------------------------------------*/
void Mesh_D3D9::enumFrames(D3DXFRAME_DERIVED *pFrame, ushort parentIndex, ushort &index)
{
	_ASSERTE(index < USHRT_MAX && "Too many frames in file");

	ushort localIndex = index;
	++index;

	pFrame->mFrameIndex = localIndex;
	pFrame->mParentIndex = parentIndex;
	mpFrameList[localIndex] = pFrame;

	// traverse through siblings first, which share the same parent index
	if (pFrame->pFrameSibling) {
		enumFrames((D3DXFRAME_DERIVED*)pFrame->pFrameSibling, parentIndex, index);
    }

	// then traverse the children, setting the parent index to local
	if (pFrame->pFrameFirstChild) {
		enumFrames((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild, localIndex, index);
    }
}

/*---------------------------------------------------------------------
	Called once after loading to assign frame index values and build
	the frame list from the tree.
---------------------------------------------------------------------*/
void Mesh_D3D9::buildFrameList()
{
	mpFrameList = new D3DXFRAME_DERIVED*[mNumFrames];
	ushort startIndex = 0;

	// the root frame has no parent, so local and parent index values are both 0
	enumFrames(mRootFrame, startIndex, startIndex);
	mInitFlags[INIT_FRAMELIST] = true;
}

/*---------------------------------------------------------------------
	Called once after loading to build default materials from the mesh.
	loadResourcesFromDisk true causes child textures and effects to be
	loaded from files on disk as they are referenced in the mesh file,
	rather than from a registered IResourceSource. Once loaded from
	disk, the resources will be injected into the cache. This should
	only be used when loading a mesh from disk, and not from the cache.
	Before adding to the cache, the references should be cleaned up to
	directly point to the resource as it exists in an IResourceSource.
	This will limit processing needed for primetime use. So basically,
	onLoad() calls this method with loadResourcesFromDisk false, and
	expects that the references in the file will resolve correctly to
	textures and effects using the cache.
---------------------------------------------------------------------*/
void Mesh_D3D9::buildDefaultMaterialList(bool async, bool loadResourcesFromDisk)
{
	for (uint f = 0; f < mNumFrames; ++f) {
		if (mpFrameList[f]->pMeshContainer) {
			for (uint m = 0; m < mpFrameList[f]->pMeshContainer->NumMaterials; ++m) {
				D3DXMATERIAL &srcMat = mpFrameList[f]->pMeshContainer->pMaterials[m];
				D3DXEFFECTINSTANCE &srcEff = mpFrameList[f]->pMeshContainer->pEffects[m];

				// create the new material, push it to the list
				MaterialPtr mPtr(new Material_D3D9());
				mDefaultMaterial.push_back(mPtr);
				Material_D3D9 &mat = (*mPtr.get());
				// initialize the material values
				mat.setD3DMaterial(srcMat.MatD3D);
				if (srcEff.pEffectFilename) {
					mat.setEffect(srcEff.pEffectFilename, async);
				}
				for (uint d = 0; d < srcEff.NumDefaults; ++d) {
					mat.addEffectDefault(srcEff.pDefaults[d]);
				}
				if (srcMat.pTextureFilename) {
					if (loadResourcesFromDisk) {
						// The Material_D3D9::addTexture function will cause a request from the
						// resource cache for a reference to the texture, loading it from a registered
						// IResourceSource if not already present. Since we want the texture to be loaded
						// from disk for this function, we'll inject the texture into the cache so the
						// material can then pick it up naturally
						ResPtr texPtr(new Texture_D3D9());
						Texture_D3D9 *tex = static_cast<Texture_D3D9 *>(texPtr.get());
						if (tex->loadFromFile(srcMat.pTextureFilename, 0, true)) {
							Texture_D3D9::injectIntoCache(texPtr, ResCache_Material);
						}
					}
					// tell the material to add the texture to its list, assuming it is already cached
					// this means we don't need to use any source path to retrieve it
					mat.addTexture(srcMat.pTextureFilename, async, true);
				}
			}
		}
	}
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
HRESULT Mesh_D3D9::setRenderMethod(D3DXMESHCONTAINER_DERIVED *pMeshContainer,
								   int iMaterial/*, cRenderMethod *pRenderMethod*/)
{
    HRESULT hr = S_OK;
/*
	SAFE_RELEASE(pMeshContainer->ppRenderMethodList[iMaterial]);
	pMeshContainer->ppRenderMethodList[iMaterial] = pRenderMethod;

	if (pRenderMethod) {
		pRenderMethod->AddRef();
		bool bUsesTangents = true;
		bool bUsesBinormal = true;

		for (int iMethod = 0; iMethod < cRenderMethod::k_max_render_stages; ++iMethod) {
			cEffectFile* pEffectFile = pRenderMethod->getEffect(iMethod);

			if (pEffectFile && pEffectFile->effect()) {
				LPD3DXEFFECT pEffect = pEffectFile->effect();

				// Look for tangents semantic
				D3DXEFFECT_DESC EffectDesc;
				D3DXHANDLE hTechnique;
				D3DXTECHNIQUE_DESC TechniqueDesc;
				D3DXHANDLE hPass;
				D3DXPASS_DESC PassDesc;

				pEffect->GetDesc(&EffectDesc);
				for (uint iTech = 0; iTech < EffectDesc.Techniques; ++iTech) {
					hTechnique = pEffect->GetTechnique(iTech);
					pEffect->GetTechniqueDesc(hTechnique, &TechniqueDesc);
					
					for (uint iPass = 0; iPass < TechniqueDesc.Passes; ++iPass) {
						hPass = pEffect->GetPass( hTechnique, iPass );
						pEffect->GetPassDesc( hPass, &PassDesc );

						uint NumVSSemanticsUsed;
						D3DXSEMANTIC pVSSemantics[MAXD3DDECLLENGTH];

						if (!PassDesc.pVertexShaderFunction ||
							FAILED(D3DXGetShaderInputSemantics(PassDesc.pVertexShaderFunction, pVSSemantics, &NumVSSemanticsUsed))) {
							continue;
						}

						for (uint iSem = 0; iSem < NumVSSemanticsUsed; ++iSem) {
							if (pVSSemantics[iSem].Usage == D3DDECLUSAGE_TANGENT) {
								bUsesTangents = TRUE;
							}
							if (pVSSemantics[iSem].Usage == D3DDECLUSAGE_BINORMAL) {
								bUsesBinormal = TRUE;
							}
						}
					}
				}
			}
		}

		// compute binormal and tanget data if needed
		// LPD3DXMESH& pOrigMesh = pMeshContainer->MeshData.pMesh;
        // get the base mesh interface
		LPD3DXBASEMESH pOrigMesh = 0;
		if (pMeshContainer->MeshData.Type == D3DXMESHTYPE_MESH) {
			pOrigMesh = (LPD3DXBASEMESH)pMeshContainer->MeshData.pMesh;
		} else {
			pOrigMesh = (LPD3DXBASEMESH)pMeshContainer->MeshData.pPMesh;
		}

		bool bRebuildRenderMesh = !pMeshContainer->RenderMeshData.pMesh;
		if (bUsesTangents || bUsesBinormal) {
			D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
			D3DVERTEXELEMENT9 End = D3DDECL_END();
 
			pOrigMesh->GetDeclaration( Declaration );
 
			bool bHasTangents = false;
			bool bHasBinormal = false;
			for (int iElem = 0; Declaration[iElem].Stream != End.Stream; ++iElem) {
				if (Declaration[iElem].Usage == D3DDECLUSAGE_TANGENT) {
					bHasTangents = true;
				}
				if (Declaration[iElem].Usage == D3DDECLUSAGE_BINORMAL) {
					bHasBinormal = true;
				}
			}
 
			bool cloneMesh = false;
			WORD currentOffset = (WORD)pOrigMesh->GetNumBytesPerVertex();

			// Update Mesh Semantics if changed
			if (bUsesTangents && !bHasTangents) {
				Declaration[iElem].Stream = 0;
				Declaration[iElem].Offset = currentOffset;
				Declaration[iElem].Type = D3DDECLTYPE_FLOAT3;
				Declaration[iElem].Method = D3DDECLMETHOD_DEFAULT;
				Declaration[iElem].Usage = D3DDECLUSAGE_TANGENT;
				Declaration[iElem].UsageIndex = 0;
				Declaration[iElem+1] = End;
				++iElem;
				currentOffset += sizeof(float)*3;
				cloneMesh = true;
			}

			if (bUsesBinormal && !bHasBinormal) {
				Declaration[iElem].Stream = 0;
				Declaration[iElem].Offset = currentOffset;
				Declaration[iElem].Type = D3DDECLTYPE_FLOAT3;
				Declaration[iElem].Method = D3DDECLMETHOD_DEFAULT;
				Declaration[iElem].Usage = D3DDECLUSAGE_BINORMAL;
				Declaration[iElem].UsageIndex = 0;
				Declaration[iElem+1] = End;
				++iElem;
				currentOffset += sizeof(float)*3;
				cloneMesh = true;
			}
			Declaration[iElem] = End;

			if (cloneMesh) {
				if (pMeshContainer->MeshData.Type == D3DXMESHTYPE_PMESH) {
					// set to the highest possible LOD
					pMeshContainer->MeshData.pPMesh->SetNumVertices(0xffffffff);
				}

				// clone a mesh with a shared VB
				if (pMeshContainer->MeshData.Type == D3DXMESHTYPE_MESH) {
					LPD3DXMESH pTempMesh;
					hr = pMeshContainer->MeshData.pMesh->CloneMesh(D3DXMESH_SYSTEMMEM, Declaration, TheGameHost.d3dDevice(), &pTempMesh);
					if (SUCCEEDED(hr)) {
						// do the work on the clone. The shared VB will ensure that
						// the original mesh is updated as well
						D3DXComputeTangent(pTempMesh, 0, 0, 0, FALSE, pMeshContainer->pAdjacency);

						SAFE_RELEASE(pOrigMesh);
						pMeshContainer->MeshData.pMesh = pTempMesh;
						pOrigMesh = (LPD3DXBASEMESH)pTempMesh;
					}
				}
				bRebuildRenderMesh = true;
			}
		}

		if (bRebuildRenderMesh) {
			if (pMeshContainer->RenderMeshData.Type == D3DXMESHTYPE_PMESH) {
				SAFE_RELEASE(pMeshContainer->RenderMeshData.pPMesh);
			} else {
				SAFE_RELEASE(pMeshContainer->RenderMeshData.pMesh);
			}
			SAFE_RELEASE(pMeshContainer->pBoneCombinationBuf);

			if (pMeshContainer->pSkinInfo != NULL) {
				if (pMeshContainer->MeshData.Type == D3DXMESHTYPE_PMESH) {
					// set to the highest possible LOD
					pMeshContainer->MeshData.pPMesh->SetNumVertices(0xffffffff);
				}

				// clone a mesh with a shared VB
				D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
				pOrigMesh->GetDeclaration(Declaration);
				LPD3DXMESH pTempMesh;
				hr = pOrigMesh->CloneMesh(D3DXMESH_SYSTEMMEM, Declaration, TheGameHost.d3dDevice(), &pTempMesh);
				if (SUCCEEDED(hr)) {
					// Get palette size
					// First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
					// (96 - 9) /3 i.e. Maximum constant count - used constants 
					uint MaxMatrices = 26; 
					pMeshContainer->NumBoneMatrices = min(MaxMatrices, pMeshContainer->pSkinInfo->GetNumBones());

					DWORD Flags = D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE | D3DXMESH_MANAGED | D3DXMESH_WRITEONLY;
					LPD3DXMESH pNewMesh;
					hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh(
							pTempMesh,
							Flags, 
							pMeshContainer->NumBoneMatrices, 
							pMeshContainer->pAdjacency, 
							NULL, NULL, NULL,             
							&pMeshContainer->NumBoneInfluences,
							&pMeshContainer->NumAttributeGroups, 
							&pMeshContainer->pBoneCombinationBuf, 
							&pNewMesh);

					if (SUCCEEDED(hr)) {
						// Build semantics for the new mesh
						D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
						LPD3DVERTEXELEMENT9 pDeclCur;
						hr = pNewMesh->GetDeclaration(pDecl);
						if (SUCCEEDED(hr)) {
							// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
							//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
							//          this is more of a "cast" operation
							pDeclCur = pDecl;
							while (pDeclCur->Stream != 0xff) {
								if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0)) {
									pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
								}
								pDeclCur++;
							}

							hr = pNewMesh->UpdateSemantics(pDecl);

							// do we copy the new mesh or convert to a PMesh?
							if (pMeshContainer->MeshData.Type == D3DXMESHTYPE_MESH) {
								pMeshContainer->RenderMeshData.pMesh = pNewMesh;
							} else {
								hr = D3DXGeneratePMesh(          
										pNewMesh,
										pMeshContainer->pAdjacency,
										NULL,
										NULL,
										pMeshContainer->MeshData.pPMesh->GetMinFaces(),
										D3DXMESHSIMP_FACE,
										&pMeshContainer->RenderMeshData.pPMesh);

								if (SUCCEEDED(hr)) {
									pMeshContainer->RenderMeshData.Type = 
										pMeshContainer->MeshData.Type;

									SAFE_RELEASE(pNewMesh);
								} else {
									pMeshContainer->RenderMeshData.Type = D3DXMESHTYPE_MESH;
									pMeshContainer->RenderMeshData.pMesh = pNewMesh;
								}
							}
						}
					}
					SAFE_RELEASE(pTempMesh);
				}
			} else {
				DWORD Flags = D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE | D3DXMESH_MANAGED;

				if (pMeshContainer->MeshData.Type == D3DXMESHTYPE_MESH) {
			
//					hr = pMeshContainer->MeshData.pMesh->Optimize(	
//												Flags, 
//												pMeshContainer->pAdjacency,
//												NULL, 
//												NULL, 
//												NULL,
//												&pMeshContainer->RenderMeshData.pMesh);
//
					D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
					pMeshContainer->MeshData.pMesh->GetDeclaration( Declaration );

					hr = pMeshContainer->MeshData.pMesh->CloneMesh(	
												D3DXMESH_MANAGED ,
												Declaration, 
												TheGameHost.d3dDevice(),
												&pMeshContainer->RenderMeshData.pMesh);

					if (pMeshContainer->pAdjacency) {
						if (FAILED(pMeshContainer->RenderMeshData.pMesh->OptimizeInplace(
								D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, 
								(DWORD*)pMeshContainer->pAdjacency, 
								NULL, NULL, NULL)))
						{
							MessageBox(NULL, "Optomizing Model failed", "model_resource.exe", MB_OK);
						}
					}
				} else {
					D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
					pMeshContainer->MeshData.pPMesh->GetDeclaration( Declaration );
				
					hr = pMeshContainer->MeshData.pPMesh->ClonePMesh(	
							D3DXMESH_MANAGED ,
							Declaration, 
							TheGameHost.d3dDevice(),
							&pMeshContainer->RenderMeshData.pPMesh);
				}

				pMeshContainer->RenderMeshData.Type = pMeshContainer->MeshData.Type;
			}
		}

		if (FAILED(hr)) {
			debug_error(hr);
		}
	}
*/
	return hr;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
void Mesh_D3D9::renderSubset(int iFrame, int iSubset) const
{
	_ASSERTE(iFrame < mNumFrames && "Frame index out of range");

	D3DXMESHCONTAINER_DERIVED *pMeshContainer =
		(D3DXMESHCONTAINER_DERIVED*)mpFrameList[iFrame]->pMeshContainer;

	if (pMeshContainer) {
		if (pMeshContainer->MeshData.Type == D3DXMESHTYPE_MESH) {
			if (pMeshContainer->MeshData.pMesh) { // RenderMeshData.pMesh) {
				pMeshContainer->MeshData.pMesh->DrawSubset(iSubset); // RenderMeshData.pMesh->DrawSubset(iSubset);
			}
		} else {
			if (pMeshContainer->MeshData.pPMesh) { // RenderMeshData.pPMesh) {
				pMeshContainer->MeshData.pPMesh->DrawSubset(iSubset); // RenderMeshData.pPMesh->DrawSubset(iSubset);
			}
		}
	}
}

/*---------------------------------------------------------------------
	constructor with this signature is required for the resource system
---------------------------------------------------------------------*/
Mesh_D3D9::Mesh_D3D9(const string &name, uint sizeB, const ResCachePtr &resCachePtr) :
	Resource_D3D9(name, sizeB, resCachePtr),
	mRootFrame(0), mpFrameList(0), mNumFrames(0), mpAnimController(0),
	mInitialized(false), mLoadAsync(false),	mInitFlags(0)
{}
		
/*---------------------------------------------------------------------
	use this default constructor to create the texture without caching,
	or for cache injection method.
---------------------------------------------------------------------*/
Mesh_D3D9::Mesh_D3D9() :
	Resource_D3D9(),
	mRootFrame(0), mpFrameList(0), mNumFrames(0), mpAnimController(0),
	mInitialized(false), mLoadAsync(false),	mInitFlags(0)
{}

// Destructor
Mesh_D3D9::~Mesh_D3D9()
{
	if (mInitFlags[INIT_FRAMELIST]) {
		delete [] mpFrameList;
	}
	if (mInitFlags[INIT_MESH]) {
		FrameAllocator_D3DX9 frameAlloc;
		HRESULT hr = D3DXFrameDestroy(mRootFrame, &frameAlloc);
		SAFE_RELEASE(mpAnimController);
		debugPrintf("Mesh_D3D9: Mesh \"%s\" destroyed, return code %li\n", name().c_str(), hr);
	}
}

////////// class FrameAllocator_D3DX9 //////////

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
HRESULT FrameAllocator_D3DX9::AllocateName(const char *source, char **dest)
{
	int strLen = strlen(source) + 1;
	if (strLen > 1) {
		*dest = new char[strLen];
		strcpy_s(*dest, strLen, source);
	} else {
		*dest = NULL;
	}
	return S_OK;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
HRESULT FrameAllocator_D3DX9::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
	D3DXFRAME_DERIVED *pFrame = new D3DXFRAME_DERIVED;
	*ppNewFrame = NULL;

	// sets the name of this frame for scripting id and debugging
	AllocateName(Name, &pFrame->Name);

	// initialize other data members of the frame
	D3DXMatrixIdentity(&pFrame->TransformationMatrix);

	*ppNewFrame = pFrame;
	++mFrameCount;
	debugPrintf("   frame created, name=\"%s\", total=%i\n", pFrame->Name, mFrameCount);

    return S_OK;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
HRESULT FrameAllocator_D3DX9::CreateMeshContainer(
			LPCSTR Name, 
			CONST D3DXMESHDATA *pMeshData,
			CONST D3DXMATERIAL *pMaterials, 
			CONST D3DXEFFECTINSTANCE *pEffectInstances, 
			DWORD NumMaterials, 
			CONST DWORD *pAdjacency, 
			LPD3DXSKININFO pSkinInfo, 
			LPD3DXMESHCONTAINER *ppNewMeshContainer)
{
	*ppNewMeshContainer = NULL;

    // this app does not handle patch meshes, 
	// so fail when one is found
	if (pMeshData->Type == D3DXMESHTYPE_PATCHMESH) {
		debugPrintf("FrameAllocator_D3D9::CreateMeshContainer: \"%s\" is a patch mesh!\n", Name);
		return E_FAIL;
	}

	ID3DXBaseMesh *pMesh = NULL;
	if (pMeshData->Type == D3DXMESHTYPE_MESH) {
		// get the pMesh interface pointer out of the mesh data structure
		pMesh = (ID3DXBaseMesh*)pMeshData->pMesh;
	} else { // D3DXMESHTYPE_PMESH
		// get the pMesh interface pointer out of the mesh data structure
		pMesh = (ID3DXBaseMesh*)pMeshData->pPMesh;
	}

	// allocate the overloaded structure to return as a D3DXMESHCONTAINER
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	// initialize the mesh container vars (consider moving this responsibility to the object)
	SecureZeroMemory(pMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));
	AllocateName(Name, &pMeshContainer->Name);
	debugPrintf("   creating MeshContainer \"%s\"\n", pMeshContainer->Name);

	//IDirect3DDevice9 *pD3DDevice = NULL;
	//pMesh->GetDevice(&pD3DDevice); // this increments the internal reference count
	
	uint numFaces = pMesh->GetNumFaces();

	if (pMeshData->Type == D3DXMESHTYPE_MESH) {
		// get the pMesh interface pointer out of the mesh data structure
		pMeshContainer->MeshData.pMesh = pMeshData->pMesh;
	} else {
		// get the pMesh interface pointer out of the mesh data structure
		pMeshContainer->MeshData.pPMesh = pMeshData->pPMesh;
	}
	pMeshContainer->MeshData.Type = pMeshData->Type;

	pMesh->AddRef();

	// allocate memory to contain the material information.  This sample uses
	// the D3D9 materials and texture names instead of the EffectInstance style materials
	pMeshContainer->NumMaterials = std::max((DWORD)1, NumMaterials);
	pMeshContainer->pAdjacency = new DWORD[numFaces*3];
    
	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->pEffects = new D3DXEFFECTINSTANCE[pMeshContainer->NumMaterials];
	//pMeshContainer->ppRenderMethodList = new cRenderMethod*[pMeshContainer->NumMaterials];

	memcpy_s(pMeshContainer->pAdjacency, sizeof(DWORD)*numFaces*3, pAdjacency, sizeof(DWORD)*numFaces*3);
	//SecureZeroMemory(pMeshContainer->ppRenderMethodList, sizeof(cRenderMethod*)*pMeshContainer->NumMaterials);
	SecureZeroMemory(pMeshContainer->pEffects, sizeof(D3DXEFFECTINSTANCE)*pMeshContainer->NumMaterials);

	// if materials provided, copy them
	if (NumMaterials > 0) {
		// copy all the material data in bulk
		memcpy_s(pMeshContainer->pMaterials, sizeof(D3DXMATERIAL) * NumMaterials,
				 pMaterials, sizeof(D3DXMATERIAL)*NumMaterials);

		// individually copy the material names
		for (uint iMaterial = 0; iMaterial < NumMaterials; ++iMaterial) {
			AllocateName(pMaterials[iMaterial].pTextureFilename, &pMeshContainer->pMaterials[iMaterial].pTextureFilename);
		}

		// copy the effect instances (if any)
		if (pEffectInstances) {
			for (uint iMaterial = 0; iMaterial < NumMaterials; ++iMaterial) {
				const D3DXEFFECTINSTANCE *pSrcEffect = &pEffectInstances[iMaterial];
				D3DXEFFECTINSTANCE *pDestEffect = &pMeshContainer->pEffects[iMaterial];
				
				if (pSrcEffect && pDestEffect && pSrcEffect->pEffectFilename) {
					AllocateName(pSrcEffect->pEffectFilename, &pDestEffect->pEffectFilename);

					pDestEffect->NumDefaults = pSrcEffect->NumDefaults;

					if (pSrcEffect->NumDefaults > 0) {
						pDestEffect->pDefaults = new D3DXEFFECTDEFAULT[pSrcEffect->NumDefaults];
						memcpy_s(pDestEffect->pDefaults, sizeof(D3DXEFFECTDEFAULT)*pSrcEffect->NumDefaults,
								 pSrcEffect->pDefaults, sizeof(D3DXEFFECTDEFAULT)*pSrcEffect->NumDefaults);
					
						for (uint iDefault = 0; iDefault < pSrcEffect->NumDefaults; ++iDefault) {
							pDestEffect->pDefaults[iDefault].pParamName = 0;
							AllocateName(pSrcEffect->pDefaults[iDefault].pParamName, &pDestEffect->pDefaults[iDefault].pParamName);
						}
					}
				}
			}
		}
	} else { // if no materials provided, use a default one
		pMeshContainer->pMaterials[0].pTextureFilename = NULL;
		SecureZeroMemory(&pMeshContainer->pMaterials[0].MatD3D, sizeof(D3DMATERIAL9));
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
	}

	// if there is skinning information, save off the required data and then setup for HW skinning
	if (pSkinInfo) {
		// first save off the SkinInfo and original mesh data
		pMeshContainer->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		// Will need an array of offset matrices to move the vertices from the figure space to the bone's space
		mNumBones = pSkinInfo->GetNumBones();
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[mNumBones];

		// get each of the bone offset matrices so that we don't need to get them later
		for (uint iBone = 0; iBone < mNumBones; ++iBone) {
			memcpy_s(&pMeshContainer->pBoneOffsetMatrices[iBone], sizeof(D3DXMATRIX),
					 pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone), sizeof(D3DXMATRIX));
		}
	}

	*ppNewMeshContainer = pMeshContainer;
	//SAFE_RELEASE(pD3DDevice); // this undoes the increment from pMesh->GetDevice()

	return S_OK;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
HRESULT FrameAllocator_D3DX9::DestroyFrame(LPD3DXFRAME pFrameToFree) 
{
	D3DXFRAME_DERIVED *dFrame = (D3DXFRAME_DERIVED*)pFrameToFree;
	delete [] dFrame->Name;
	delete dFrame;
	return S_OK; 
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
HRESULT FrameAllocator_D3DX9::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	delete [] pMeshContainer->Name;
	delete [] pMeshContainer->pAdjacency;
	delete [] pMeshContainer->pBoneOffsetMatrices;

	/*if (pMeshContainer->ppRenderMethodList != NULL) {
		for (uint iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; ++iMaterial) {
			SAFE_RELEASE(pMeshContainer->ppRenderMethodList[iMaterial]);
		}
	}
	delete [] pMeshContainer->ppRenderMethodList;*/

	if (pMeshContainer->pEffects) {
		for (uint iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; ++iMaterial) {
			D3DXEFFECTINSTANCE *pEffect = &pMeshContainer->pEffects[iMaterial];
			delete [] pEffect->pEffectFilename;

			for (uint iDefault = 0; iDefault < pEffect->NumDefaults; ++iDefault) {
				delete [] pEffect->pDefaults[iDefault].pParamName;
			}
			delete [] pEffect->pDefaults;
		}
	}
	delete [] pMeshContainer->pEffects;

	for (uint iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; ++iMaterial) {
		delete [] pMeshContainer->pMaterials[iMaterial].pTextureFilename;
	}
	delete [] pMeshContainer->pMaterials;

	//delete [] pMeshContainer->pBoneIndexList;
	//SAFE_RELEASE(pMeshContainer->pBoneCombinationBuf);
	SAFE_RELEASE(pMeshContainer->pSkinInfo);

	if (pMeshContainer->MeshData.Type == D3DXMESHTYPE_MESH) {
		SAFE_RELEASE(pMeshContainer->MeshData.pMesh);
	} else {
		SAFE_RELEASE(pMeshContainer->MeshData.pPMesh);
	}

	/*if (pMeshContainer->RenderMeshData.Type == D3DXMESHTYPE_MESH) {
		SAFE_RELEASE(pMeshContainer->RenderMeshData.pMesh);
	} else {
		SAFE_RELEASE(pMeshContainer->RenderMeshData.pPMesh);
	}*/
 
	delete pMeshContainer;

    return S_OK;
}

////////// class UserDataLoader_D3DX9 //////////

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
HRESULT UserDataLoader_D3DX9::LoadTopLevelData(LPD3DXFILEDATA pXofChildData)
{
	return S_OK;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
HRESULT UserDataLoader_D3DX9::LoadFrameChildData(LPD3DXFRAME pFrame, 
								LPD3DXFILEDATA pXofChildData)
{
	HRESULT hr = S_OK;
/*
	const GUID *pGuid;
	hr=pXofChildData->GetType(&pGuid);

	if (SUCCEEDED(hr))
	{
		if (memcmp(pGuid, &DXFILEOBJ_ExtendedMaterial, sizeof(GUID)) == 0)
		{
			if (pFrame->pMeshContainer)
			{
				LoadMeshChildData(pFrame->pMeshContainer, pXofChildData);	
			}
		}
	}*/
	return hr;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
HRESULT UserDataLoader_D3DX9::LoadMeshChildData(LPD3DXMESHCONTAINER pMeshContainerBase, 
												LPD3DXFILEDATA pXofChildData)
{
/*	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	const GUID *pGuid;

	HRESULT hr=pXofChildData->GetType(&pGuid);

	if (SUCCEEDED(hr))
	{
		if (memcmp(pGuid, &DXFILEOBJ_ExtendedMaterial, sizeof(GUID)) == 0)
		{
			DWORD dataSize;
			void* pData;

			hr=pXofChildData->GetData(NULL, &dataSize, &pData);

			if (SUCCEEDED(hr) && dataSize == sizeof(GaiaExtendedData))
			{
				GaiaExtendedData extendedData;
				memcpy_s(&extendedData, dataSize, pData, dataSize);

				SetCurrentDirectory(TheGameHost.rootPath());

				cRenderMethod* pRenderMethod = 
					DisplayManager.renderMethodPool().loadResource(extendedData.renderMethodName);

				if (pRenderMethod)
				{
					safe_release(pMeshContainer->ppRenderMethodList[extendedData.materialIndex]);
					pMeshContainer->ppRenderMethodList[extendedData.materialIndex] = pRenderMethod;
				}
			}
		}
	}
	return hr;*/
	return S_OK;
}