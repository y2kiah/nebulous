/* ----==== CAMERA_D3D9.H ====---- */

#pragma once

#include <d3dx9.h>
#include "Camera.h"


///// STRUCTURES /////

class Camera_D3D9 : public Camera {
	private:
		///// VARIABLES /////

		D3DXQUATERNION	viewQuat;
		D3DXMATRIX		viewMatrix;
		D3DXMATRIX		projMatrix;

	public:
		///// ACCESSORS /////

		const D3DXQUATERNION	&getViewQuat() const	{ return viewQuat; }
		const D3DXMATRIX		*getViewMatrix() const	{ return &viewMatrix; }
		const D3DXMATRIX		*getProjMatrix() const	{ return &projMatrix; }


		///// FUNCTIONS /////

		void		setViewTranslationYawPitchRoll(const Vector3f &_p, const Vector3f &_va);
		void		setViewTranslationLookUp(const Vector3f &_p, const Vector3f &_look, const Vector3f &_up);
		void		setViewTranslationQuaternion(const Vector3f &_p, const D3DXQUATERNION &_q);

		void		setProjectionAttributes(float _fovRad, float _ar, float _zNear, float _zFar, float _zoom);
		
		void		recomputeMatrices();

		// Constructors / Destructor

		explicit Camera_D3D9();
		explicit Camera_D3D9(const Vector3f &_p, const Vector3f &_va, float _fovRad, float _ar, float _zNear, float _zFar, float _zoom);
		~Camera_D3D9() {}
};

