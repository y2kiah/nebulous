/* ----==== CAMERA.CPP ====---- */

#include "Camera.h"


///// FUNCTIONS /////

Camera::Camera() :
	p(Vector3f(0.0f, 0.0f, 0.0f)),
	va(Vector3f(0.0f, 0.0f, 0.0f)),
	fov(0.0f),
	aspectRatio(1.0f),
	zNear(1.0f),
	zFar(1000.0f),
	zoom(1.0f)
{
}

Camera::Camera(const Vector3f &_p, const Vector3f &_va, float _fovRad, float _ar, float _zNear, float _zFar, float _zoom) :
	p(_p),
	va(_va),
	fov(_fovRad),
	aspectRatio(_ar),
	zNear(_zNear),
	zFar(_zFar),
	zoom(_zoom)
{
}
