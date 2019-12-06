/*----==== MATERIAL_D3D9.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	09/14/2009
	Rev.Date:	09/14/2009
-----------------------------------*/

#include <d3dx9.h>
#include "Material_D3D9.h"
#include "Effect_D3D9.h"
#include "Texture_D3D9.h"

////////// class Material_D3D9 //////////

/*---------------------------------------------------------------------
	Check that this returns true before using the effect for rendering.
---------------------------------------------------------------------*/
bool Material_D3D9::isReadyForRender() const
{
	if (mTextureCount == mTextures.size() && mEffect.get()) {
		// also check if effect defaults and textures match effect requirements
		Effect_D3D9 *pEff = (Effect_D3D9 *)mEffect.get();
		return pEff->isReadyForRender();
	}
	return false;
}

/*---------------------------------------------------------------------
	Sets material's effect. assumeCached true will assume that the
	texture is already in the ResCache_Material cache and will attempt
	to retrieve it. If false, async determines the method used to load
	it from a registered source.
---------------------------------------------------------------------*/
bool Material_D3D9::setEffect(const string &filename, bool async, bool assumeCached)
{
	// assumeCached is true when, for example, an effect is injected after loading it from disk,
	// or otherwise, anytime you're sure it's already present in the ResCache_Material cache
	if (assumeCached) {
		ResHandle h;
		if (!h.getFromCache(filename, ResCache_Material)) {
			debugPrintf("Material_D3D9: Error: failed to load effect \"%s\" from cache in material\n", filename.c_str());
			return false;
		}
		mEffect = h.getResPtr(); // store a reference to the effect
		return true;
	}
	// if assumeCached is false, load it from a registered source, where filename should
	// already contain the path of the source, e.g. "sourcename/filename.fxo"
	if (async) {
		// for asynchronous loading, try to load the resource
		ResHandle h;
		ResLoadResult retVal = h.tryLoad<Effect_D3D9>(filename);
		if (retVal == ResLoadResult_Success) {
			mEffect = h.getResPtr(); // store a reference to the texture

		} else if (retVal == ResLoadResult_Error) {
			debugPrintf("Material_D3D9: Error: failed to load effect \"%s\" in material\n", filename.c_str());
			return false;

		} else { // waiting
			// Submits a job that will run until the resource is available and loaded, or errors.
			// The job performs the same actions as above when the resource is loaded.
			CProcessPtr crProcPtr(new Material_D3D9::CheckResourceLoadedProcess(this, filename,
				Material_D3D9::CheckResourceLoadedProcess::MatResType_Effect));
			procMgr.attach(crProcPtr);
			// add to the local list, the process won't be destroyed when finished, but this is neccesary
			// incase the material is destroyed before the process finishes. In the destructor, the list is
			// traversed and all processes are killed
			mProcessList.push_back(crProcPtr);
		}

	} else {
		// for synchronous loading, false will be returned if any of the child
		// resources fails to load
		ResHandle h;
		if (h.load<Effect_D3D9>(filename)) {
			mEffect = h.getResPtr(); // store a reference to the texture

		} else {
			debugPrintf("Material_D3D9: Error: failed to load effect \"%s\" in material\n", filename.c_str());
			return false;
		}
	}
	return true;
}

/*---------------------------------------------------------------------
	Sets values in the D3DMATERIAL9 structure
---------------------------------------------------------------------*/
void Material_D3D9::setD3DMaterial(const D3DMATERIAL9 &mat)
{
	memcpy_s(&mD3DMaterial, sizeof(D3DMATERIAL9), &mat, sizeof(D3DMATERIAL9));
}

/*---------------------------------------------------------------------
	Adds an effect parameter default value to the list.
---------------------------------------------------------------------*/
void Material_D3D9::addEffectDefault(const D3DXEFFECTDEFAULT &def)
{
	EffectDefaultPtr ePtr = EffectDefaultPtr(new D3DXEFFECTDEFAULT);
	D3DXEFFECTDEFAULT *pDef = ePtr.get();
	// copy the string over
	pDef->NumBytes = def.NumBytes;
	pDef->Type = def.Type;
	pDef->pParamName = 0;
	if (def.pParamName) {
		int strLen = strlen(def.pParamName) + 1;
		if (strLen > 1) {
			pDef->pParamName = new char[strLen];
			strcpy_s(pDef->pParamName, strLen, def.pParamName);
		}
	}
	pDef->pValue = 0;
	if (def.NumBytes > 0 && def.pValue) {
		pDef->pValue = new char[def.NumBytes];
		memcpy_s(pDef->pValue, def.NumBytes, def.pValue, def.NumBytes);
	}

	mD3DEffectDefaults.push_back(ePtr);
}

/*---------------------------------------------------------------------
	Adds a texture to the end of list. assumeCached true will assume
	that the texture is already in the ResCache_Material cache and will
	attempt to retrieve it. If false, async determines the method used
	to load it from a registered source.
---------------------------------------------------------------------*/
bool Material_D3D9::addTexture(const string &filename, bool async, bool assumeCached)
{
	++mTextureCount;
	// assumeCached is true when, for example, a texture is injected after loading it from disk,
	// or otherwise, anytime you're sure it's already present in the ResCache_Material cache
	if (assumeCached) {
		ResHandle h;
		if (!h.getFromCache(filename, ResCache_Material)) {
			debugPrintf("Material_D3D9: Error: failed to load child resource \"%s\" from cache in material\n", filename.c_str());
			return false;
		}
		mTextureFlags[mTextures.size()] = true;
		mTextures.push_back(h.getResPtr());	// store a reference to the texture
		return true;
	}
	// if assumeCached is false, load it from a registered source, where filename should
	// already contain the path of the source, e.g. "sourcename/filename.dds"
	if (async) {
		// for asynchronous loading, try to load the resource
		ResHandle h;
		ResLoadResult retVal = h.tryLoad<Texture_D3D9>(filename);
		if (retVal == ResLoadResult_Success) {
			mTextureFlags[mTextures.size()] = true;
			mTextures.push_back(h.getResPtr());	// store a reference to the texture

		} else if (retVal == ResLoadResult_Error) {
			debugPrintf("Material_D3D9: Error: failed to load child resource \"%s\" in material\n", filename.c_str());
			return false;

		} else { // waiting
			// Submits a job that will run until the resource is available and loaded, or errors.
			// The job performs the same actions as above when the resource is loaded.
			CProcessPtr crProcPtr(new Material_D3D9::CheckResourceLoadedProcess(this, filename, 
				Material_D3D9::CheckResourceLoadedProcess::MatResType_Texture));
			procMgr.attach(crProcPtr);
			mProcessList.push_back(crProcPtr);
		}

	} else {
		// for synchronous loading, false will be returned if any of the child
		// resources fails to load
		ResHandle h;
		if (h.load<Texture_D3D9>(filename)) {
			mTextureFlags[mTextures.size()] = true;
			mTextures.push_back(h.getResPtr());	// store a reference to the texture

		} else {
			debugPrintf("Material_D3D9: Error: failed to load child resource \"%s\" in material\n", filename.c_str());
			return false;
		}
	}
	return true;
}

Material_D3D9::~Material_D3D9()
{
	// for each process in the list, if it's not already finished, kill it
	ProcessList::const_iterator i, end = mProcessList.end();
	for (i = mProcessList.begin(); i != end; ++i) {
		if (!(*i)->isFinished()) { (*i)->finish(); }
	}
}

////////// class CheckResourceLoadedProcess //////////

void Material_D3D9::CheckResourceLoadedProcess::onUpdate(float deltaMillis) {
	ResHandle h;
	// handle texture loading
	if (mResType == MatResType_Texture) {
		ResLoadResult retVal = h.tryLoad<Texture_D3D9>(mResourcePath);
		if (retVal == ResLoadResult_Success) {
			mpMaterial->mTextureFlags[mpMaterial->mTextures.size()] = true;
			mpMaterial->mTextures.push_back(h.getResPtr());	// store a reference to the texture
			finish();

		} else if (retVal == ResLoadResult_Error) {
			debugPrintf("Material_D3D9: Error: failed to load child resource \"%s\" in material\n", mResourcePath.c_str());
			finish();
		}
	// handle effect loading
	} else if (mResType == MatResType_Effect) {
		ResLoadResult retVal = h.tryLoad<Effect_D3D9>(mResourcePath);
		if (retVal == ResLoadResult_Success) {
			mpMaterial->mEffect = h.getResPtr(); // store a reference to the texture
			finish();

		} else if (retVal == ResLoadResult_Error) {
			debugPrintf("Material_D3D9: Error: failed to load effect \"%s\" in material\n", mResourcePath.c_str());
			finish();
		}
	}
}

Material_D3D9::CheckResourceLoadedProcess::CheckResourceLoadedProcess(
		Material_D3D9 *pMat, const string &resourcePath, MatResType resType) :
	CProcess("CheckResourceLoadedProcess", CProcess_Run_CanDelay, CProcess_Queue_Multiple),
	mpMaterial(pMat),
	mResourcePath(resourcePath),
	mResType(resType)
{}
