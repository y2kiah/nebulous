/*----==== RENDERMANAGER_D3D9.CPP ====----
	Author: Jeff Kiah
	Date:	4/13/2007
----------------------------------------*/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN	// defined in project settings
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9math.h>
#include <vector>
#include "RenderManager_D3D9.h"
#include "RenderObject_D3D9.h"
#include "D3D9Debug.h"
#include "Texture_D3D9.h"
#include "Win32Info.h"
#include "Engine.h"
#include "Camera_D3D9.h"
#include "Math Code/FastMath.h"
#include "RenderOptions.h"
#include "RenderObjectOwner.h"


///// FUNCTIONS /////

unsigned int RenderManager_D3D9::createRenderObject()
{
	unsigned int returnID = -1;

	if (freeRenderObjectIDs.size() > 0) {
		// retrieve last ID in list and discard it from list
		returnID = freeRenderObjectIDs[freeRenderObjectIDs.size()-1];
		freeRenderObjectIDs.pop_back();

		// create object at the specified index
		renderObjects[returnID] = new RenderObject_D3D9();

	} else {
		// no open IDs so push it to back of list
		renderObjects.push_back(new RenderObject_D3D9());
		returnID = static_cast<unsigned int>(renderObjects.size()) - 1;
	}

	return returnID;
}


void RenderManager_D3D9::removeRenderObject(unsigned int roID)
{
	_ASSERTE(roID >= 0 && roID < renderObjects.size());

	if (renderObjects[roID] == 0) return;	// object already removed
	// look for ID in the list
	for (unsigned int c = 0; c < freeRenderObjectIDs.size(); ++c) {
		if (roID == freeRenderObjectIDs[c]) { 	// roID already in the list
			delete renderObjects[roID];	// ID is in list but pointer not null so ensure delete
			return;
		}
	}

	// delete object from renderObjects list
	delete renderObjects[roID];
	renderObjects[roID] = 0;

	// add ID to freeRenderObjectIDs list
	freeRenderObjectIDs.push_back(roID);
}


void RenderManager_D3D9::clearRenderObjects()
{
	for (unsigned int c = 0; c < renderObjects.size(); ++c) {
		delete renderObjects[c];
	}
	renderObjects.clear();
	freeRenderObjectIDs.clear();

	for (unsigned int c = 0; c < submittedObjects.size(); ++c) {
		delete submittedObjects[c];
	}
	submittedObjects.clear();
	numSubmitted = 0;
}


// objects submitted will be culled by the camera 3D frustum only if doCameraFrustumCull is true
// external culling algorithms should determine which objects are submitted
void RenderManager_D3D9::submitForRendering(const RenderObjectParams &rop)
{
	_ASSERTE(rop.getRoID() >= 0 && rop.getRoID() < renderObjects.size());
	_ASSERTE(renderObjects[rop.getRoID()]);

	++numSubmitted;

	if (numSubmitted > submittedObjects.size()) {
		submittedObjects.push_back(new RenderObjectParams(rop));
	} else {
		*submittedObjects[numSubmitted - 1] = rop;
	}
}


bool RenderManager_D3D9::fillVertexData(unsigned int roID, const void *_data, int _numVerts, RenderVertexFormat _vFormat) const
{
	_ASSERTE(roID >= 0 && roID < renderObjects.size());
	_ASSERTE(renderObjects[roID]);

	RenderObject_D3D9 *ro = static_cast<RenderObject_D3D9*>(renderObjects[roID]);

	ro->vFormat = _vFormat;
	ro->numVerts = _numVerts;

	// determine vertex format
	int bufferSize = sizeRVF(_vFormat) * _numVerts;

	// create vertex buffer
	HRESULT hr = d3dDevice->CreateVertexBuffer(	bufferSize,
												D3DUSAGE_WRITEONLY,
												D3DFVF_XYZ,///// TEMP/////0,
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

	return true;
}


bool RenderManager_D3D9::addIndexBuffer(unsigned int roID, const void *_data, int _numIndices, int _indexSizeInBytes,
										unsigned short _numVertices, unsigned short _numPrimitives, RenderPrimitiveType _primitiveType) const
{
	_ASSERTE(roID >= 0 && roID < renderObjects.size());
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
	++ro->numIndexBuffers;
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

	return true;
}


bool RenderManager_D3D9::initRenderDevice(int bpp, int resX, int resY, int refreshRate, bool fullscreen)
{
	HRESULT hr;

	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d) {
		SAFE_RELEASE(d3d);
		MessageBox(0, L"Failed to initialize Direct3D", L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST);		
		return false;
	}

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
		hr = d3d->CheckDeviceType(	D3DADAPTER_DEFAULT,	// Adapter
									D3DDEVTYPE_HAL,		// Device Type
									dispFormat,			// Display Format
									backFormat,			// BackBuffer Format
									!fullscreen);		// Windowed
		if (FAILED(hr)) {			
			SAFE_RELEASE(d3d);
			MessageBox(0, L"Display mode not supported", L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST);		
			return false;
		}

		// check for display formats given pixel format
		bool formatFound = false;
		D3DDISPLAYMODE pMode;
		for (UINT c = 0; c < d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, dispFormat) - 1; ++c) {
			if (SUCCEEDED(d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, dispFormat, c, &pMode))) {
				if (pMode.Width == resX && pMode.Height == resY && pMode.RefreshRate == refreshRate) {
					formatFound = true;
					break;
				}
			}
		}

		if (!formatFound) {			
			SAFE_RELEASE(d3d);
			MessageBox(0, L"Display mode not supported", L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST);		
			return false;
		}

	} else { // not fullscreen
		
	}

	// define D3D parameters
	d3dParams.BackBufferCount				= 1;
	d3dParams.BackBufferFormat				= backFormat;
	d3dParams.MultiSampleType				= D3DMULTISAMPLE_NONE;
	d3dParams.MultiSampleQuality			= 0;
	d3dParams.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	d3dParams.hDeviceWindow					= win32.hWnd;
	d3dParams.Flags							= 0;
	d3dParams.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	d3dParams.PresentationInterval			= D3DPRESENT_INTERVAL_DEFAULT;	
	d3dParams.EnableAutoDepthStencil		= TRUE;
	d3dParams.AutoDepthStencilFormat		= D3DFMT_D24X8;

	if (fullscreen) {
		d3dParams.Windowed			= FALSE;
		d3dParams.BackBufferWidth	= resX;
		d3dParams.BackBufferHeight	= resY;
	} else {
		d3dParams.Windowed			= TRUE;
		d3dParams.BackBufferWidth	= 0;
		d3dParams.BackBufferHeight	= 0;
	}

	hr = d3d->CreateDevice(	D3DADAPTER_DEFAULT,
							D3DDEVTYPE_HAL,
							win32.hWnd,
							D3DCREATE_HARDWARE_VERTEXPROCESSING,                          
							&d3dParams,                          
							&d3dDevice);

	if (FAILED(hr)) {
		SAFE_RELEASE(d3dDevice);
		SAFE_RELEASE(d3d);
		if (hr == D3DERR_NOTAVAILABLE) {
			MessageBox(0, L"Direct3D device does not support these settings", L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
		} else {
			MessageBox(0, L"Failed to create Direct3D device", L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
		}		
		return false;
	}

	// Create the vertex declarations for each RVF type
	if (!createVertexDeclarations()) {
		SAFE_RELEASE(d3dDevice);
		SAFE_RELEASE(d3d);
		MessageBox(0, L"Failed to create Direct3D vertex declarations", L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
		return false;
	}

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

	// Send device info to the texture class
	Texture_D3D9::setIDirect3DDevice9(d3dDevice);

	return true;
}


bool RenderManager_D3D9::createVertexDeclarations()
{
	// RVF_V3
	D3DVERTEXELEMENT9 rvfDec0[] =
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};

	if (FAILED(d3dDevice->CreateVertexDeclaration(rvfDec0, &d3dVertexDeclaration[0]))) {
		SAFE_RELEASE(d3dVertexDeclaration[0]);
		return false;
	}

	// RVF_V3_N3_UV1
	D3DVERTEXELEMENT9 rvfDec1[] =
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	if (FAILED(d3dDevice->CreateVertexDeclaration(rvfDec1, &d3dVertexDeclaration[1]))) {
		SAFE_RELEASE(d3dVertexDeclaration[1]);
		SAFE_RELEASE(d3dVertexDeclaration[0]);
		return false;
	}

	// RVF_V3_N3_UV1_C2
	D3DVERTEXELEMENT9 rvfDec2[] =
	{
		{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 32, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0},
		{0, 36, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    1},
		D3DDECL_END()
	};

	if (FAILED(d3dDevice->CreateVertexDeclaration(rvfDec2, &d3dVertexDeclaration[2]))) {
		SAFE_RELEASE(d3dVertexDeclaration[2]);
		SAFE_RELEASE(d3dVertexDeclaration[1]);
		SAFE_RELEASE(d3dVertexDeclaration[0]);
		return false;
	}

	return true;
}


void RenderManager_D3D9::freeRenderDevice()
{
	clearRenderObjects();

	for (int c = 0; c < RNDR_RVF_COUNT; ++c) SAFE_RELEASE(d3dVertexDeclaration[c]);

	HRESULT hr = d3dDevice->EvictManagedResources();
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
	}

	SAFE_RELEASE(d3dDevice);
	SAFE_RELEASE(d3d);
}


bool RenderManager_D3D9::renderObject(const RenderObjectParams &rop, const Camera *cam) const
{
	RenderObject_D3D9 *ro = static_cast<RenderObject_D3D9*>(renderObjects[rop.getRoID()]);

	// set to translate from model space to world space
	D3DXMATRIX scaleMat, rotMat, transMat, worldMat;
	D3DXMatrixIdentity(&worldMat);
	
	D3DXMatrixScaling(&scaleMat, rop.scale.x, rop.scale.y, rop.scale.z);
	D3DXMatrixRotationYawPitchRoll(&rotMat, rop.angle.y, rop.angle.x, rop.angle.z);
	D3DXMatrixTranslation(&transMat, rop.worldPos.x, rop.worldPos.y, rop.worldPos.z);
	D3DXMatrixMultiply(&worldMat, &scaleMat, &rotMat);
	D3DXMatrixMultiply(&worldMat, &worldMat, &transMat);

	d3dDevice->SetTransform(D3DTS_WORLD, &worldMat);

//	d3dDevice->SetVertexDeclaration(d3dVertexDeclaration[ro->vFormat]);
//	d3dDevice->SetVertexShader(0);	// this should set appropriate vertex shader
	HRESULT hr = d3dDevice->SetFVF(D3DFVF_XYZ);	// TEMP
	hr = d3dDevice->SetStreamSource(0, ro->vertexBuffer, 0, sizeRVF(ro->vFormat));
	
	RO_D3D9_IndexBuffer *ibRef = ro->indexBuffer[rop.indexBufferID];
	hr = d3dDevice->SetIndices(ibRef->indexBuffer);
	hr = d3dDevice->DrawIndexedPrimitive(ibRef->primitiveType, 0, 0, ibRef->numVertices, 0, ibRef->numPrimitives);
	hr = d3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, ro->numVerts);
//	hr = d3dDevice->SetVertexShader(0);

	return true;
}


bool RenderManager_D3D9::render(const Camera *cam) const {
	const Camera_D3D9 *camD3D9 = static_cast<const Camera_D3D9*>(cam);

	// Clear buffer - may not need this
	HRESULT hr = d3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}

	// set view/projection matrices from Camera
	d3dDevice->SetTransform(D3DTS_PROJECTION, camD3D9->getProjMatrix());
	d3dDevice->SetTransform(D3DTS_VIEW, camD3D9->getViewMatrix());

	hr = d3dDevice->BeginScene();
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		return false;
	}

	// render all objects
	for (unsigned int r = 0; r < numSubmitted; ++r) {
		renderObject(*submittedObjects[r], cam);
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


HRESULT RenderManager_D3D9::resetRenderDevice(void)
{
	// Device is lost and cannot be reset yet
	if (d3dDevice->TestCooperativeLevel() == D3DERR_DEVICELOST) {
		freeVolatileResources();

		while (d3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
			Sleep(0);	// don't burn CPU cycles while waiting
		}

		// Lost but we can reset it now
		HRESULT hr = d3dDevice->Reset(&d3dParams);
		if (SUCCEEDED(hr)) {
			initVolatileResources();
		}

		return hr;
	}

	return D3D_OK;
}


///// CONSTRUCTOR / DESTRUCTOR /////

RenderManager_D3D9::RenderManager_D3D9() : RenderManager("DIRECT3D9"),
	d3d(0),
	d3dDevice(0)
{
	for (int c = 0; c < RNDR_RVF_COUNT; ++c) d3dVertexDeclaration[c] = 0;

	ZeroMemory(&d3dParams, sizeof(D3DPRESENT_PARAMETERS));
}


RenderManager_D3D9::~RenderManager_D3D9()
{
	clearRenderObjects();
}
