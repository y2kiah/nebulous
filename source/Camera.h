/* ----==== CAMERA.H ====---- */

#pragma once

#include "Math Code/Vector3f.h"


///// STRUCTURES /////

class Camera abstract {
	protected:
		///// VARIABLES /////

		Vector3f		p, va;
		float			fov, aspectRatio, zNear, zFar, zoom;

		///// FUNCTIONS /////

		// Constructors
		explicit Camera();
		explicit Camera(const Vector3f &_p, const Vector3f &_va, float _fovRad, float _ar, float _zNear, float _zFar, float _zoom);

	public:
		///// ACCESSORS /////

		virtual const Vector3f	&getP() const			{ return p; }
		virtual const Vector3f	&getViewAngle() const	{ return va; }	// returns axis angles in radians
		virtual float			getFOV() const			{ return fov; }		// returns horizontal fov in radians
		virtual float			getAR() const			{ return aspectRatio; }
		virtual float			getZNear() const		{ return zNear; }
		virtual float			getZFar() const			{ return zFar; }
		virtual float			getZoom() const			{ return zoom; }

		///// FUNCTIONS /////

		virtual void	setViewTranslationYawPitchRoll(const Vector3f &_p, const Vector3f &_va) = 0;
		virtual void	setViewTranslationLookUp(const Vector3f &_p, const Vector3f &_look, const Vector3f &_up) = 0;
		virtual void	setProjectionAttributes(float _fovRad, float _ar, float _zNear, float _zFar, float _zoom) = 0;
		
		virtual void	recomputeMatrices() = 0;

		// Destructor
		virtual ~Camera() {}
};

