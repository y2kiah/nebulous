/*----==== CAMERA_D3D9.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	06/30/2009
	Rev.Date:	07/08/2009
---------------------------------*/

#include "Camera_D3D9.h"
#include "../Math/FastMath.h"

///// FUNCTIONS /////

void Camera_D3D9::setViewTranslationYawPitchRoll(const Vector3f &pos, const Vector3f &viewRadians)
{
	mPos = pos;
	mViewRadians = viewRadians;

	D3DXMATRIX matTranslation, matRotation;

	D3DXMatrixTranslation(&matTranslation, -mPos.x, -mPos.y, -mPos.z);	
	D3DXMatrixRotationYawPitchRoll(&matRotation, mViewRadians.y, mViewRadians.x, mViewRadians.z);
	D3DXMatrixMultiply(&mViewMatrix, &matTranslation, &matRotation);
}

void Camera_D3D9::setViewTranslationLookUp(const Vector3f &pos, const Vector3f &look, const Vector3f &up)
{
}

void Camera_D3D9::setViewTranslationQuaternion(const Vector3f &pos, const D3DXQUATERNION &q)
{
	mPos = pos;
	mViewQuat.x = q.x;
	mViewQuat.y = q.y;
	mViewQuat.z = q.z;
	mViewQuat.w = q.w;

	D3DXMATRIX matTranslation, matRotation;

	D3DXMatrixTranslation(&matTranslation, -mPos.x, -mPos.y, -mPos.z);
	D3DXMatrixRotationQuaternion(&matRotation, &mViewQuat);
	D3DXMatrixMultiply(&mViewMatrix, &matTranslation, &matRotation);
}

void Camera_D3D9::setProjectionAttributes(float fovXRad, float aspectRatio,
										  float zNear, float zFar, float zoom)
{
	mFovXRad = fovXRad;
	mFovYRad = fovXRad / aspectRatio;
	mAspectRatio = aspectRatio;
	mZNear = zNear;
	mZFar = zFar;
	mZoom = (zoom <= 0.0f) ? 1.0f : ((zoom >= 200.0f) ? 1.0f : zoom);

	D3DXMatrixPerspectiveFovLH(&mProjMatrix, mFovYRad / mZoom, mAspectRatio, mZNear, mZFar);
}
		
void Camera_D3D9::recomputeMatrices()
{
	_ASSERTE(mZoom > 0.0f && "Zoom must be > 0");

	D3DXMatrixAffineTransformation(&mViewMatrix, 1.0f, 0, &mViewQuat, &D3DXVECTOR3(-mPos.x, -mPos.y, -mPos.z));
	D3DXMatrixPerspectiveFovLH(&mProjMatrix, mFovYRad / mZoom, mAspectRatio, mZNear, mZFar);
}

Camera_D3D9::Camera_D3D9() :
	mPos(Vector3f(0.0f, 0.0f, 0.0f)),
	mViewRadians(Vector3f(0.0f, 0.0f, 0.0f)),
	mFovXRad(PIf / 3.0f),
	mFovYRad(PIf / 3.0f),
	mAspectRatio(1.0f),
	mZNear(0.1f),
	mZFar(10000.0f),
	mZoom(1.0f)
{
	D3DXMatrixIdentity(&mViewMatrix);
	D3DXMatrixIdentity(&mProjMatrix);
}

Camera_D3D9::Camera_D3D9(const Vector3f &pos, const Vector3f &viewRadians, float fovXRad,
						 float aspectRatio, float zNear, float zFar, float zoom) :
	mPos(pos),
	mViewRadians(viewRadians),
	mFovXRad(fovXRad),
	mFovYRad(fovXRad / aspectRatio),
	mAspectRatio(aspectRatio),
	mZNear(zNear),
	mZFar(zFar),
	mZoom(zoom)
{
	_ASSERTE(aspectRatio > 0.0f && "Aspect ratio must be > 0");
	recomputeMatrices();
}





