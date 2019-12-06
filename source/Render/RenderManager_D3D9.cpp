/*----==== RENDERMANAGER_D3D9.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	04/13/2007
	Rev.Date:	07/08/2009
----------------------------------------*/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN	// defined in project settings
#endif

#include "RenderManager_D3D9.h"
#include "../Win32.h"
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9math.h>
#include "D3D9Debug.h"
#include "Resource_D3D9.h"
#include "Effect_D3D9.h"
#include "Scene.h"
#include "../Engine.h"
#include "../Math/FastMath.h"
#include "../Event/RegisteredEvents.h"

#include "RenderObject_D3D9.h"

///// STATIC VARIABLES /////

////////// class Resource_D3D9 //////////
Resource_D3D9List Resource_D3D9::sUnmanagedList;
IDirect3DDevice9 * Resource_D3D9::spD3DDevice = 0;	// make smart pointer

///// FUNCTIONS /////

////////// class RenderManager_D3D9 //////////

// objects submitted will be culled by the camera 3D frustum only if doCameraFrustumCull is true
// external culling algorithms should determine which objects are submitted
void RenderManager_D3D9::submitForRendering()
{
}

bool RenderManager_D3D9::fillVertexData(uint roID, const void *_data, int _numVerts, RenderVertexFormat _vFormat) const
{
/*	_ASSERTE(roID >= 0 && roID < renderObjects.size());
	_ASSERTE(renderObjects[roID]);

	RenderObject_D3D9 *ro = static_cast<RenderObject_D3D9*>(renderObjects[roID]);

	ro->vFormat = _vFormat;
	ro->numVerts = _numVerts;

	// determine vertex format
	int bufferSize = sizeRVF(_vFormat) * _numVerts;

	d3dDevice->SetVertexDeclaration(mD3DVertexDecl[RNDR_RVF_P3]);

	// create vertex buffer
	HRESULT hr = d3dDevice->CreateVertexBuffer(	bufferSize,
												D3DUSAGE_WRITEONLY,
												0,
												D3DPOOL_MANAGED,
												&ro->vertexBuffer,
												0);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}

	// lock the vertex buffer
	hr = ro->vertexBuffer->Lock(0, 0, &ro->lpVertices, 0);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}

	// copy data into vertex buffer
	memcpy_s(ro->lpVertices, bufferSize, _data, bufferSize);

	// unlock vertex buffer
	hr = ro->vertexBuffer->Unlock();
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}
*/
	return true;
}

bool RenderManager_D3D9::addIndexBuffer(uint roID, const void *_data, int _numIndices, int _indexSizeInBytes,
										ushort _numVertices, ushort _numPrimitives, RenderPrimitiveType _primitiveType) const
{
/*	_ASSERTE(roID >= 0 && roID < renderObjects.size());
	_ASSERTE(renderObjects[roID]);
	_ASSERTE(_indexSizeInBytes == 2 || _indexSizeInBytes == 4);

	RenderObject_D3D9 *ro = static_cast<RenderObject_D3D9*>(renderObjects[roID]);

	// add an index buffer to the list
	D3DPRIMITIVETYPE _d3dPrimitiveType;
	switch (_primitiveType) {
		case RNDR_PT_POINTLIST:		_d3dPrimitiveType = D3DPT_POINTLIST;		break;
		case RNDR_PT_LINELIST:		_d3dPrimitiveType = D3DPT_LINELIST;			break;
		case RNDR_PT_LINESTRIP:		_d3dPrimitiveType = D3DPT_LINESTRIP;		break;
		case RNDR_PT_TRIANGLELIST:	_d3dPrimitiveType = D3DPT_TRIANGLELIST;		break;
		case RNDR_PT_TRIANGLESTRIP:	_d3dPrimitiveType = D3DPT_TRIANGLESTRIP;	break;
		case RNDR_PT_TRIANGLEFAN:	_d3dPrimitiveType = D3DPT_TRIANGLEFAN;		break;
		default:					_d3dPrimitiveType = D3DPT_TRIANGLESTRIP;	break;
	}	
	RO_D3D9_IndexBuffer *newIBuffer = new RO_D3D9_IndexBuffer(_numVertices, _numPrimitives, _d3dPrimitiveType);
	ro->indexBuffer.push_back(newIBuffer);
	++(ro->numIndexBuffers);
	int vectorIndex = ro->numIndexBuffers - 1;
	int bufferSize = _numIndices * _indexSizeInBytes;

	// create index buffer
	HRESULT hr = d3dDevice->CreateIndexBuffer(	bufferSize,
												D3DUSAGE_WRITEONLY,
												(_indexSizeInBytes == 2) ? D3DFMT_INDEX16 : D3DFMT_INDEX32,
												D3DPOOL_MANAGED,
												&newIBuffer->indexBuffer,
												0);

	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}

	// lock the index buffer
	hr = newIBuffer->indexBuffer->Lock(0, 0, &newIBuffer->lpIndices, 0);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}

	// copy data into index buffer
	memcpy_s(newIBuffer->lpIndices, bufferSize, _data, bufferSize);

	// unlock index buffer
	hr = newIBuffer->indexBuffer->Unlock();
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}
*/
	return true;
}

bool RenderManager_D3D9::buildPresentParams(int bpp, int resX, int resY, int refreshRate,
											bool fullscreen, bool vsync)
{
	D3DFORMAT dispFormat = D3DFMT_UNKNOWN;
	D3DFORMAT backFormat = D3DFMT_UNKNOWN;

	// set up fullscreen mode	
	if (fullscreen) {		
		switch (bpp) {
			case 32:
				dispFormat = D3DFMT_X8R8G8B8;
				backFormat = D3DFMT_A8R8G8B8;
				break;
			case 16:			
				dispFormat = D3DFMT_R5G6B5;
				backFormat = D3DFMT_R5G6B5;
				break;
			default:
				dispFormat = D3DFMT_X8R8G8B8;
				backFormat = D3DFMT_A8R8G8B8;
				break;
		}

		// check for pixel format
		HRESULT hr;
		hr = d3d->CheckDeviceType(	D3DADAPTER_DEFAULT,	// Adapter
									D3DDEVTYPE_HAL,		// Device Type
									dispFormat,			// Display Format
									backFormat,			// BackBuffer Format
									!fullscreen);		// Windowed
		if (FAILED(hr)) {
			debugPrintf("Display mode not supported\n");
			Win32::instance().showErrorBox(TEXT("Display mode not supported"));
			return false;
		}

		// check for display formats given pixel format
		bool formatFound = false;
		D3DDISPLAYMODE pMode;
		UINT numModes = d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, dispFormat);
		for (UINT m = 0; m < numModes; ++m) {
			if (SUCCEEDED(d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, dispFormat, m, &pMode))) {
				if (pMode.Width == resX && pMode.Height == resY && pMode.RefreshRate == refreshRate) {
					formatFound = true;
					break;
				}
			}
		}
		if (!formatFound) {
			debugPrintf("Display mode not supported\n");
			Win32::instance().showErrorBox(TEXT("Display mode not supported"));
			return false;
		}
	}

	// define D3D parameters
	d3dParams.BackBufferCount				= 1;
	d3dParams.BackBufferFormat				= backFormat;
	d3dParams.MultiSampleType				= D3DMULTISAMPLE_NONE;
	d3dParams.MultiSampleQuality			= 0;
	d3dParams.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	d3dParams.hDeviceWindow					= Win32::instance().hWnd;
	d3dParams.Flags							= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL/* | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER*/;
	d3dParams.EnableAutoDepthStencil		= TRUE;								 // uncomment above to support GDI message boxes in fullscreen (performance hit??)
	d3dParams.AutoDepthStencilFormat		= D3DFMT_D24X8;
	d3dParams.Windowed						= (fullscreen ? FALSE : TRUE);
	d3dParams.BackBufferWidth				= (fullscreen ? resX : 0);
	d3dParams.BackBufferHeight				= (fullscreen ? resY : 0);
	d3dParams.FullScreen_RefreshRateInHz	= (fullscreen ? refreshRate : 0);
	d3dParams.PresentationInterval			= (vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE);

	return true;
}

void RenderManager_D3D9::initRenderState()
{
	// set up renderer state vars
	d3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	
	d3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
}

bool RenderManager_D3D9::initRenderDevice(int bpp, int resX, int resY, int refreshRate,
										  bool fullscreen, bool vsync)
{
	// test that the D3DX version needed is installed to the system
	HRESULT hr = D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION);
	if (FAILED(hr)) {
		std::wostringstream wss;
		wss << TEXT("Unable to find the required D3DX dll version d3dx9_") << D3DX_SDK_VERSION <<
			TEXT(".dll.\nPlease install the latest version of the D3DX library.");
		debugTPrintf("%S\n", wss.str().c_str());
		Win32::instance().showErrorBox(wss.str().c_str());
		return false;
	}

	// Create the D3D9 device
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d) {
		debugTPrintf("Failed to initialize Direct3D 9.\nPlease install the latest version of the DirectX runtime.\n");
		Win32::instance().showErrorBox(TEXT("Failed to initialize Direct3D 9.\nPlease install the latest version of the DirectX runtime."));
		return false;
	}
	mInitFlags[INIT_D3D] = true; // set the init flag

	// build windowed mode present params on startup - fullscreen mode params set on toggle
	if (!buildPresentParams(bpp, resX, resY, refreshRate, false, vsync)) {
		return false;
	}

	hr = d3d->CreateDevice(	D3DADAPTER_DEFAULT,
							D3DDEVTYPE_HAL,
							Win32::instance().hWnd,
							D3DCREATE_HARDWARE_VERTEXPROCESSING,                          
							&d3dParams,                          
							&d3dDevice);

	if (FAILED(hr)) {
		if (hr == D3DERR_NOTAVAILABLE) {
			debugPrintf("Direct3D device does not support these settings\n");
			Win32::instance().showErrorBox(TEXT("Direct3D device does not support these settings"));
		} else {
			debugPrintf("Failed to create Direct3D device\n");
			Win32::instance().showErrorBox(TEXT("Failed to create Direct3D device"));
		}		
		return false;
	}
	mInitFlags[INIT_D3DDEVICE] = true; // set the init flag

	// Uncomment this to enable GDI dialog boxes in fullscreen mode
	//d3dDevice->SetDialogBoxMode(TRUE); // requires D3DPRESENTFLAG_LOCKABLE_BACKBUFFER in pparams

	// Create the vertex declarations for each RVF type
	if (!createVertexDeclarations()) {
		debugPrintf("Failed to create Direct3D vertex declarations\n");
		Win32::instance().showErrorBox(TEXT("Failed to create Direct3D vertex declarations"));
		return false;
	}
	mInitFlags[INIT_VERT_DECL] = true; // set the init flag

	// create the effect pool
	hr = D3DXCreateEffectPool(&mEffectPool);
	if (FAILED(hr)) {
		debugPrintf("Failed to create effect pool\n");
		Win32::instance().showErrorBox(TEXT("Failed to create effect pool"));
		return false;
	}
	mInitFlags[INIT_EFFECT_POOL] = true; // set the init flag

	// set up renderer state vars
	initRenderState();

	// inject device info to the Resource_D3D9 class
	Resource_D3D9::spD3DDevice = d3dDevice;
	// inject effect pool info the Effect_D3D9 class
	Effect_D3D9::spEffectPool = mEffectPool;

	// if fullscreen mode is requested initially, go to it now
	if (fullscreen) Win32::instance().toggleFullScreen();

	return true;
}

bool RenderManager_D3D9::makeDisplayChanges(int bpp, int resX, int resY, int refreshRate,
											bool fullscreen, bool vsync)
{
	if (!buildPresentParams(bpp, resX, resY, refreshRate, fullscreen, vsync)) {
		debugPrintf("RenderManager_D3D9: Error: Failed to build PresentParams!\n");
		return false;
	}
	HRESULT hr = resetRenderDevice();
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		debugPrintf("RenderManager_D3D9: Error: Failed to reset render device!\n");
		return handleLostDevice();
	}
	handleLostDevice();
	return true;
}

void RenderManager_D3D9::freeRenderDevice()
{
	if (mInitFlags[INIT_EFFECT_POOL]) { SAFE_RELEASE(mEffectPool); }
	if (mInitFlags[INIT_VERT_DECL]) {
		for (int v = 0; v < RNDR_RVF_MAX; ++v) { SAFE_RELEASE(mD3DVertexDecl[v]); }
	}
	if (mInitFlags[INIT_D3DDEVICE]) { SAFE_RELEASE(d3dDevice); }
	if (mInitFlags[INIT_D3D]) { SAFE_RELEASE(d3d); }
}

bool RenderManager_D3D9::createVertexDeclarations()
{
	// RVF_P3
	D3DVERTEXELEMENT9 rvfDecl0[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};

	// RVF_P3_N3_UV2
	D3DVERTEXELEMENT9 rvfDecl1[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	// RVF_P3_N3_UV2_C2
	D3DVERTEXELEMENT9 rvfDecl2[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 32, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0},	// diffuse
		{0, 36, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    1},	// specular
		D3DDECL_END()
	};

	// RVF_P3_N3_T3_BN3_UV2
	D3DVERTEXELEMENT9 rvfDecl3[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
		{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
		{0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	if (FAILED(d3dDevice->CreateVertexDeclaration(rvfDecl0, &mD3DVertexDecl[0])) ||
		FAILED(d3dDevice->CreateVertexDeclaration(rvfDecl1, &mD3DVertexDecl[1])) ||
		FAILED(d3dDevice->CreateVertexDeclaration(rvfDecl2, &mD3DVertexDecl[2])) ||
		FAILED(d3dDevice->CreateVertexDeclaration(rvfDecl3, &mD3DVertexDecl[3])))
	{
		for (int v = 0; v < RNDR_RVF_MAX; ++v) {
			// switch to using smart pointer and avoid this type of call
			SAFE_RELEASE(mD3DVertexDecl[v]);
		}
		return false;
	}

	return true;
}

bool RenderManager_D3D9::renderEntry(RenderEntry_D3D9 &pEntry, const Camera_D3D9 &pCam) const
{
	// set to translate from model space to world space
	//D3DXMATRIX scaleMat, rotMat, transMat, worldMat;
	//D3DXMatrixIdentity(&worldMat);
	
	/*D3DXMatrixScaling(&scaleMat, rop.scale.x, rop.scale.y, rop.scale.z);
	D3DXMatrixRotationYawPitchRoll(&rotMat, rop.angle.y, rop.angle.x, rop.angle.z);
	D3DXMatrixTranslation(&transMat, rop.worldPos.x, rop.worldPos.y, rop.worldPos.z);
	D3DXMatrixMultiply(&worldMat, &scaleMat, &rotMat);
	D3DXMatrixMultiply(&worldMat, &worldMat, &transMat);*/
	const Matrix4x4f &toWorldMat = pEntry.mSceneNode.get()->getProperties().toWorld;

	const D3DXMATRIX *worldMat = toWorldMat.toD3DXMATRIX();

	d3dDevice->SetTransform(D3DTS_WORLD, worldMat);

	pEntry.mSceneNode.get()->render();
//	d3dDevice->SetVertexDeclaration(d3dVertexDeclaration[ro->vFormat]);
//	d3dDevice->SetVertexShader(0);	// this should set appropriate vertex shader
//	HRESULT hr = d3dDevice->SetFVF(D3DFVF_XYZ);	// TEMP
//	hr = d3dDevice->SetStreamSource(0, ro->vertexBuffer, 0, sizeRVF(ro->vFormat));
	
//	RO_D3D9_IndexBuffer *ibRef = ro->indexBuffer[rop.indexBufferID];
//	hr = d3dDevice->SetIndices(ibRef->indexBuffer);
//	hr = d3dDevice->DrawIndexedPrimitive(ibRef->primitiveType, 0, 0, ibRef->numVertices, 0, ibRef->numPrimitives);
//	hr = d3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, ro->numVerts);
//	hr = d3dDevice->SetVertexShader(0);

	return true;
}

bool RenderManager_D3D9::render(const Camera_D3D9 &cam) const {
	if (!d3d || !d3dDevice) return false;

	// Clear buffer - may not need this
	HRESULT hr = d3dDevice->Clear(	0, NULL,
									D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
									D3DCOLOR_ARGB(255,255,0,255),
									1.0f, 0);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}

	// set view/projection matrices from Camera
	d3dDevice->SetTransform(D3DTS_PROJECTION, cam.getProjMatrix());
	d3dDevice->SetTransform(D3DTS_VIEW, cam.getViewMatrix());

	hr = d3dDevice->BeginScene();
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}

	// render all objects
	
	RenderEntryList::const_iterator i, ei = mRenderList.end();
	for (i = mRenderList.begin(); i != ei; ++i) {
		RenderEntry_D3D9 *pEntry = (*i).get();

		renderEntry(*pEntry, cam);
	}

	hr = d3dDevice->EndScene();
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}

	// Show the results
	hr = d3dDevice->Present(0, 0, 0, 0);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}

	return true;
}

void RenderManager_D3D9::prepareSubmitList()
{
	// Here loop through submitted and check for frustum cull
	// also sort for least state changes and use passed priority
}
/*

    // Call the app's OnDeviceReset callback
    GetDXUTState().SetInsideDeviceCallback( true );
    const D3DSURFACE_DESC* pBackBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
    LPDXUTCALLBACKD3D9DEVICERESET pCallbackDeviceReset = GetDXUTState().GetD3D9DeviceResetFunc();
    hr = S_OK;
    if( pCallbackDeviceReset != NULL )
        hr = pCallbackDeviceReset( pd3dDevice, pBackBufferSurfaceDesc, GetDXUTState().GetD3D9DeviceResetFuncUserContext() );
    GetDXUTState().SetInsideDeviceCallback( false );
    if( FAILED(hr) )
    {
        // If callback failed, cleanup
        DXUT_ERR( L"DeviceResetCallback", hr );
        if( hr != DXUTERR_MEDIANOTFOUND )
            hr = DXUTERR_RESETTINGDEVICEOBJECTS;

        GetDXUTState().SetInsideDeviceCallback( true );
        LPDXUTCALLBACKD3D9DEVICELOST pCallbackDeviceLost = GetDXUTState().GetD3D9DeviceLostFunc();
        if( pCallbackDeviceLost != NULL )
            pCallbackDeviceLost( GetDXUTState().GetD3D9DeviceLostFuncUserContext() );
        GetDXUTState().SetInsideDeviceCallback( false );
        return hr;
    }

    // Success
    GetDXUTState().SetDeviceObjectsReset( true );

    return S_OK;
}

*/
bool RenderManager_D3D9::handleLostDevice()
{
	// Device is lost and cannot be reset yet
	HRESULT hr = d3dDevice->TestCooperativeLevel();
	switch (hr) {
		case D3DERR_DEVICELOST: {
			// the device has been lost but cannot be reset at this time.
			// so wait until it can be reset.
			Sleep(0);	// sleep so we don't hog CPU while waiting to reset
			return false;
			break;
		}
		case D3DERR_DEVICENOTRESET: {
			if (FAILED(resetRenderDevice())) {
				return false;
			}
			initVolatileResources();
			break;
		}
		case D3DERR_DRIVERINTERNALERROR: {
			// some other problem occurred, need to bail out
			engine.exit();
			Win32::instance().showErrorBox(L"Driver Internal Error attempting to reset D3D9 device.\nContact your display adapter manufacturer.");
			return false;
			break;
		}
	}
	// returned D3D_OK
	return true;
}

void RenderManager_D3D9::initVolatileResources()
{
	// set up renderer state vars
	initRenderState();
	// run through the unmanaged resource list and call init on each
	Resource_D3D9List::const_iterator i = Resource_D3D9::sUnmanagedList.begin();
	Resource_D3D9List::const_iterator iEnd = Resource_D3D9::sUnmanagedList.end();
	while (i != iEnd) {
		(*i)->initVolatileResource();	// call the polymorphic virtual function to initialize the resource
		++i;
	}
}

void RenderManager_D3D9::freeVolatileResources()
{
	// run through the unmanaged resource list and call init on each
	Resource_D3D9List::const_iterator i = Resource_D3D9::sUnmanagedList.begin();
	Resource_D3D9List::const_iterator iEnd = Resource_D3D9::sUnmanagedList.end();
	while (i != iEnd) {
		(*i)->freeVolatileResource();	// call the polymorphic virtual function to Release the resource
		++i;
	}
}

HRESULT RenderManager_D3D9::resetRenderDevice()
{
	// here, release all resources in volatile memory
	freeVolatileResources();

	Sleep(0);	// sleep so we don't hog CPU while waiting to reset

	HRESULT hr = d3dDevice->Reset(&d3dParams);
	return hr;
}

// Constructor / destructor
RenderManager_D3D9::RenderManager_D3D9() :
	d3d(0),
	d3dDevice(0),
	mEventListener(*this)
{
	ZeroMemory(mD3DVertexDecl, sizeof(mD3DVertexDecl[0])*RNDR_RVF_MAX);
	ZeroMemory(&d3dParams, sizeof(D3DPRESENT_PARAMETERS));
}

RenderManager_D3D9::~RenderManager_D3D9()
{
	freeRenderDevice();
}

////////// class RenderManagerListener //////////

#include "../EngineEvents.h"

/*---------------------------------------------------------------------
	Some display setting was changed, requiring a reset of the device
---------------------------------------------------------------------*/
bool RenderManager_D3D9::RenderManagerListener::handleDisplayChange(const EventPtr &ePtr)
{
	debugPrintf("%s: Handled the Change Settings event!\n", name().c_str());
	return false; // allow event to propagate
}

RenderManager_D3D9::RenderManagerListener::RenderManagerListener(RenderManager_D3D9 &mgr) :
	EventListener("RenderManagerListener"),
	mMgr(mgr)
{
	// register event handlers
	IEventHandlerPtr p(new EventHandler<RenderManagerListener>(this, &RenderManagerListener::handleDisplayChange));
	registerEventHandler(ChangeSettingsEvent::sEventType, p, 2); // this is the #2 priority listener for this event after EngineListener
}