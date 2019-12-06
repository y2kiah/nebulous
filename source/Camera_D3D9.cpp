/* ----==== CAMERA_D3D9.CPP ====---- */

#include "Camera_D3D9.h"
#include "Math Code/FastMath.h"


///// FUNCTIONS /////

void Camera_D3D9::setViewTranslationYawPitchRoll(const Vector3f &_p, const Vector3f &_va)
{
	p = _p;
	va = _va;

	D3DXMATRIX matTranslation, matRotation;

	D3DXMatrixTranslation(&matTranslation, -p.x, -p.y, -p.z);	
	D3DXMatrixRotationYawPitchRoll(&matRotation, va.y, va.x, va.z);
	D3DXMatrixMultiply(&viewMatrix, &matTranslation, &matRotation);
}

void Camera_D3D9::setViewTranslationLookUp(const Vector3f &_p, const Vector3f &_look, const Vector3f &_up)
{
}

void Camera_D3D9::setViewTranslationQuaternion(const Vector3f &_p, const D3DXQUATERNION &_q)
{
	p = _p;
	viewQuat.x = _q.x;
	viewQuat.y = _q.y;
	viewQuat.z = _q.z;
	viewQuat.w = _q.w;

	D3DXMATRIX matTranslation, matRotation;

	D3DXMatrixTranslation(&matTranslation, -p.x, -p.y, -p.z);
	D3DXMatrixRotationQuaternion(&matRotation, &viewQuat);
	D3DXMatrixMultiply(&viewMatrix, &matTranslation, &matRotation);
}

void Camera_D3D9::setProjectionAttributes(float _fovRad, float _ar, float _zNear, float _zFar, float _zoom)
{
	fov = _fovRad;
	aspectRatio = _ar;
	zNear = _zNear;
	zFar = _zFar;
	zoom = (_zoom <= 0.0f) ? 1.0f : ((_zoom >= 200.0f) ? 1.0f : _zoom);

	D3DXMatrixPerspectiveFovLH(&projMatrix, fov / zoom, aspectRatio, zNear, zFar);
}
		
void Camera_D3D9::recomputeMatrices()
{
	_ASSERTE(zoom != 0.0f);

	D3DXMatrixAffineTransformation(&viewMatrix, 1.0f, 0, &viewQuat, &D3DXVECTOR3(-p.x, -p.y, -p.z));
	D3DXMatrixPerspectiveFovLH(&projMatrix, fov / zoom, aspectRatio, zNear, zFar);
}

Camera_D3D9::Camera_D3D9() : Camera()
{
	D3DXMatrixIdentity(&viewMatrix);
	D3DXMatrixIdentity(&projMatrix);
}

Camera_D3D9::Camera_D3D9(const Vector3f &_p, const Vector3f &_va, float _fovRad, float _ar, float _zNear, float _zFar, float _zoom) :
	Camera(_p, _va, _fovRad, _ar, _zNear, _zFar, (_zoom <= 0.0f) ? 1.0f : ((_zoom >= 200.0f) ? 1.0f : _zoom))
{
	recomputeMatrices();
}





