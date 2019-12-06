/*----==== CAMERA_D3D9.H ====----
	Author:		Jeff Kiah
	Orig.Date:	06/30/2009
	Rev.Date:	07/08/2009
-------------------------------*/

#pragma once

#include <d3dx9math.h>
#include <boost/noncopyable.hpp>
#include "../Math/TVector3.h"

///// STRUCTURES /////

/*=============================================================================
class Camera_D3D9
=============================================================================*/
class Camera_D3D9 : private boost::noncopyable {
	private:
		///// VARIABLES /////
		D3DXQUATERNION	mViewQuat;
		D3DXMATRIX		mViewMatrix;
		D3DXMATRIX		mProjMatrix;

		Vector3f		mPos, mViewRadians;
		float			mFovXRad, mFovYRad, mAspectRatio, mZNear, mZFar, mZoom;

	public:
		///// FUNCTIONS /////
		// Accessors
		const D3DXQUATERNION &	getViewQuat() const   { return mViewQuat; }
		const D3DXMATRIX *		getViewMatrix() const { return &mViewMatrix; }
		const D3DXMATRIX *		getProjMatrix() const { return &mProjMatrix; }

		const Vector3f &	getPos() const { return mPos; }
		const Vector3f &	getViewRads() const { return mViewRadians; } // returns axis angles in radians
		float	getFovXRad() const	{ return mFovXRad; } // returns horizontal fov in radians
		float	getFovYRad() const	{ return mFovYRad; } // returns vertical fov in radians
		float	getAR() const		{ return mAspectRatio; }
		float	getZNear() const	{ return mZNear; }
		float	getZFar() const		{ return mZFar; }
		float	getZoom() const		{ return mZoom; }

		// Mutators
		void	setViewTranslationYawPitchRoll(const Vector3f &pos, const Vector3f &viewRadians);
		void	setViewTranslationLookUp(const Vector3f &pos, const Vector3f &look, const Vector3f &up);
		void	setViewTranslationQuaternion(const Vector3f &pos, const D3DXQUATERNION &q);

		void	setProjectionAttributes(float fovXRad, float aspectRatio, float zNear, float zFar, float zoom);
		
		void	recomputeMatrices();

		// Constructors / Destructor

		explicit Camera_D3D9();
		explicit Camera_D3D9(const Vector3f &pos, const Vector3f &viewRadians, float fovXRad,
							 float aspectRatio, float zNear, float zFar, float zoom);
		~Camera_D3D9() {}
};

