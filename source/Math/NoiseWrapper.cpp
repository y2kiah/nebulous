/* ----==== NOISEWRAPPER.CPP ====---- */

#include <crtdbg.h>
#include "NoiseWrapper.h"

///// DEFINES /////

#define PERLIN1_MULT	0.188f	// used to rescale noise output into range of classic Perlin noise
#define PERLIN2_MULT	0.507f
//#define PERLIN3_MULT	0.936f	// 3d multiplier not used for performance benefit
#define PERLIN4_MULT	0.87f

///// STATIC VARIABLES /////

CoherentNoiseType NoiseWrapper::cNoiseType = NOISE_TYPE_PERLIN;
MultiFractalOperation NoiseWrapper::fOperation = NOISE_MFO_ADD;

int NoiseWrapper::xSeed = 0;
int NoiseWrapper::ySeed = 0;
int NoiseWrapper::zSeed = 0;
int NoiseWrapper::wSeed = 0;

const unsigned char NoiseWrapper::p[512] = {
	151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,140, 36,103, 30,
	 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,247,120,234, 75,  0, 26,197, 62,
	 94,252,219,203,117, 35, 11, 32, 57,177, 33, 88,237,149, 56, 87,174, 20,125,136,
	171,168, 68,175, 74,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,
	 60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54, 65, 25, 63,161,
	  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,200,196,135,130,116,188,159, 86,
	164,100,109,198,173,186,  3, 64, 52,217,226,250,124,123,  5,202, 38,147,118,126,
	255, 82, 85,212,207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
	119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,129, 22, 39,253,
	 19, 98,108,110, 79,113,224,232,178,185,112,104,218,246, 97,228,251, 34,242,193,
	238,210,144, 12,191,179,162,241, 81, 51,145,235,249, 14,239,107, 49,192,214, 31,
	181,199,106,157,184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
	222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180,

	151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,140, 36,103, 30,
	 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,247,120,234, 75,  0, 26,197, 62,
	 94,252,219,203,117, 35, 11, 32, 57,177, 33, 88,237,149, 56, 87,174, 20,125,136,
	171,168, 68,175, 74,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,
	 60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54, 65, 25, 63,161,
	  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,200,196,135,130,116,188,159, 86,
	164,100,109,198,173,186,  3, 64, 52,217,226,250,124,123,  5,202, 38,147,118,126,
	255, 82, 85,212,207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
	119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,129, 22, 39,253,
	 19, 98,108,110, 79,113,224,232,178,185,112,104,218,246, 97,228,251, 34,242,193,
	238,210,144, 12,191,179,162,241, 81, 51,145,235,249, 14,239,107, 49,192,214, 31,
	181,199,106,157,184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
	222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180
};

// lookup table to traverse the simplex around a given point in 4D
const unsigned char NoiseWrapper::simplex[64][4] = {
	{0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
	{0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
	{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
	{1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
	{1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
	{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
	{2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
	{2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0}
};

// TEMP
float NoiseWrapper::grad4[512];
float NoiseWrapper::n[65536];
void NoiseWrapper::initGrad4()
{
	float kkf[256];
	for (int i = 0; i < 256; ++i) kkf[i] = -1.0f + 2.0f * ((float)i / 255.0f);

	for (int i = 0; i < 256; ++i) {
		grad4[i] = kkf[p[i]] * 0.7f;
	}

	for (int y = 0; y < 256; ++y) {
		for (int x = 0; x < 256; ++x) {
			n[y*256+x] = integerNoise2(x, y);
		}
	}
}

// TEMP

///// FUNCTIONS /////

// Noise functions

//----------------------------------------------------------------------------------------
// Linear interpolated noise is the most basic type of coherent noise, good for fast
// interpolation of noise values when high visual quality is not necessary.
//----------------------------------------------------------------------------------------
float NoiseWrapper::linearNoise1(float x)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	return FastMath::lerp(integerNoise1(ix), integerNoise1(ix+1), x - ix);
}

float NoiseWrapper::linearNoise2(float x, float y)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int ix1 = ix+1; int iy1 = iy+1;

	x -= ix; y -= iy;

	return	FastMath::lerp(	FastMath::lerp(	integerNoise2(ix ,iy ),
											integerNoise2(ix1,iy ), x),
							FastMath::lerp(	integerNoise2(ix ,iy1),
											integerNoise2(ix1,iy1), x), y);
}

float NoiseWrapper::linearNoise3(float x, float y, float z)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int iz = FastMath::f2iRoundFast(z - 0.5f);
	int ix1 = ix+1; int iy1 = iy+1; int iz1 = iz+1;

	x -= ix; y -= iy; z -= iz;

	return	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	integerNoise3(ix ,iy ,iz ),
															integerNoise3(ix1,iy ,iz ), x),
											FastMath::lerp(	integerNoise3(ix ,iy1,iz ),
															integerNoise3(ix1,iy1,iz ), x), y),
							FastMath::lerp(	FastMath::lerp(	integerNoise3(ix ,iy ,iz1),
															integerNoise3(ix1,iy ,iz1), x),
											FastMath::lerp(	integerNoise3(ix ,iy1,iz1),
															integerNoise3(ix1,iy1,iz1), x), y), z);
}

float NoiseWrapper::linearNoise4(float x, float y, float z, float w)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int iz = FastMath::f2iRoundFast(z - 0.5f);
	int iw = FastMath::f2iRoundFast(w - 0.5f);
	int ix1 = ix+1; int iy1 = iy+1; int iz1 = iz+1; int iw1 = iw+1;

	x -= ix; y -= iy; z -= iz; w -= iw;

	return	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz ,iw ),
																			integerNoise4(ix1,iy ,iz ,iw ), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz ,iw ),
																			integerNoise4(ix1,iy1,iz ,iw ), x), y),
											FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz1,iw ),
																			integerNoise4(ix1,iy ,iz1,iw ), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz1,iw ),
																			integerNoise4(ix1,iy1,iz1,iw ), x), y), z),
							FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz ,iw1),
																			integerNoise4(ix1,iy ,iz ,iw1), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz ,iw1),
																			integerNoise4(ix1,iy1,iz ,iw1), x), y),
											FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz1,iw1),
																			integerNoise4(ix1,iy ,iz1,iw1), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz1,iw1),
																			integerNoise4(ix1,iy1,iz1,iw1), x), y), z), w);
}

//----------------------------------------------------------------------------------------
// Cosine noise uses the a sine curve to interpolate between integer values, and is
// implemented to use the lookup tables of the FastMath class to avoid the slow cos
// calculation. This function still appears slower than Perlin noise.
//----------------------------------------------------------------------------------------
float NoiseWrapper::cosineNoise1(float x)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	return FastMath::lerp(integerNoise1(ix), integerNoise1(ix+1), FastMath::cosCurve(x - ix));
}

float NoiseWrapper::cosineNoise2(float x, float y)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int ix1 = ix+1; int iy1 = iy+1;

	x = FastMath::cosCurve(x - ix);
	y = FastMath::cosCurve(y - iy);

	return	FastMath::lerp(	FastMath::lerp(	integerNoise2(ix ,iy ),
											integerNoise2(ix1,iy ), x),
							FastMath::lerp(	integerNoise2(ix ,iy1),
											integerNoise2(ix1,iy1), x), y);
}

float NoiseWrapper::cosineNoise3(float x, float y, float z)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int iz = FastMath::f2iRoundFast(z - 0.5f);
	int ix1 = ix+1; int iy1 = iy+1; int iz1 = iz+1;

	x = FastMath::cosCurve(x - ix);
	y = FastMath::cosCurve(y - iy);
	z = FastMath::cosCurve(z - iz);

	return	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	integerNoise3(ix ,iy ,iz ),
															integerNoise3(ix1,iy ,iz ), x),
											FastMath::lerp(	integerNoise3(ix ,iy1,iz ),
															integerNoise3(ix1,iy1,iz ), x), y),
							FastMath::lerp(	FastMath::lerp(	integerNoise3(ix ,iy ,iz1),
															integerNoise3(ix1,iy ,iz1), x),
											FastMath::lerp(	integerNoise3(ix ,iy1,iz1),
															integerNoise3(ix1,iy1,iz1), x), y), z);
}

float NoiseWrapper::cosineNoise4(float x, float y, float z, float w)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int iz = FastMath::f2iRoundFast(z - 0.5f);
	int iw = FastMath::f2iRoundFast(w - 0.5f);
	int ix1 = ix+1; int iy1 = iy+1; int iz1 = iz+1; int iw1 = iw+1;

	x = FastMath::cosCurve(x - ix);
	y = FastMath::cosCurve(y - iy);
	z = FastMath::cosCurve(z - iz);
	w = FastMath::cosCurve(w - iw);

	return	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz ,iw ),
																			integerNoise4(ix1,iy ,iz ,iw ), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz ,iw ),
																			integerNoise4(ix1,iy1,iz ,iw ), x), y),
											FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz1,iw ),
																			integerNoise4(ix1,iy ,iz1,iw ), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz1,iw ),
																			integerNoise4(ix1,iy1,iz1,iw ), x), y), z),
							FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz ,iw1),
																			integerNoise4(ix1,iy ,iz ,iw1), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz ,iw1),
																			integerNoise4(ix1,iy1,iz ,iw1), x), y),
											FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz1,iw1),
																			integerNoise4(ix1,iy ,iz1,iw1), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz1,iw1),
																			integerNoise4(ix1,iy1,iz1,iw1), x), y), z), w);
}

//----------------------------------------------------------------------------------------
// Cubic noise interpolates with an s-curve, 3t^2 - 2t^3, and improves upon the visual
// quality of linear interpolated noise. The hermite curve has C1 continuity at integer
// boundaries.
//----------------------------------------------------------------------------------------
float NoiseWrapper::cubicNoise1(float x)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	return FastMath::lerp(integerNoise1(ix), integerNoise1(ix+1), FastMath::sCurve(x - ix));
}

float NoiseWrapper::cubicNoise2(float x, float y)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int ix1 = ix+1; int iy1 = iy+1;

	x = FastMath::sCurve(x - ix);
	y = FastMath::sCurve(y - iy);

	return	FastMath::lerp(	FastMath::lerp(	integerNoise2(ix ,iy ),
											integerNoise2(ix1,iy ), x),
							FastMath::lerp(	integerNoise2(ix ,iy1),
											integerNoise2(ix1,iy1), x), y);
}

float NoiseWrapper::cubicNoise3(float x, float y, float z)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int iz = FastMath::f2iRoundFast(z - 0.5f);
	int ix1 = ix+1; int iy1 = iy+1; int iz1 = iz+1;

	x = FastMath::sCurve(x - ix);
	y = FastMath::sCurve(y - iy);
	z = FastMath::sCurve(z - iz);

	return	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	integerNoise3(ix ,iy ,iz ),
															integerNoise3(ix1,iy ,iz ), x),
											FastMath::lerp(	integerNoise3(ix ,iy1,iz ),
															integerNoise3(ix1,iy1,iz ), x), y),
							FastMath::lerp(	FastMath::lerp(	integerNoise3(ix ,iy ,iz1),
															integerNoise3(ix1,iy ,iz1), x),
											FastMath::lerp(	integerNoise3(ix ,iy1,iz1),
															integerNoise3(ix1,iy1,iz1), x), y), z);
}

float NoiseWrapper::cubicNoise4(float x, float y, float z, float w)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int iz = FastMath::f2iRoundFast(z - 0.5f);
	int iw = FastMath::f2iRoundFast(w - 0.5f);
	int ix1 = ix+1; int iy1 = iy+1; int iz1 = iz+1; int iw1 = iw+1;

	x = FastMath::sCurve(x - ix);
	y = FastMath::sCurve(y - iy);
	z = FastMath::sCurve(z - iz);
	w = FastMath::sCurve(w - iw);

	return	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz ,iw ),
																			integerNoise4(ix1,iy ,iz ,iw ), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz ,iw ),
																			integerNoise4(ix1,iy1,iz ,iw ), x), y),
											FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz1,iw ),
																			integerNoise4(ix1,iy ,iz1,iw ), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz1,iw ),
																			integerNoise4(ix1,iy1,iz1,iw ), x), y), z),
							FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz ,iw1),
																			integerNoise4(ix1,iy ,iz ,iw1), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz ,iw1),
																			integerNoise4(ix1,iy1,iz ,iw1), x), y),
											FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz1,iw1),
																			integerNoise4(ix1,iy ,iz1,iw1), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz1,iw1),
																			integerNoise4(ix1,iy1,iz1,iw1), x), y), z), w);
}

//----------------------------------------------------------------------------------------
// Quintic noise interpolates with the function 6t^5 - 15t^4 + 10t^3, providing C2
// continuity at integer boundaries. This eliminates visual artifacts in the normal map of
// a height field generated by the noise.
//----------------------------------------------------------------------------------------
float NoiseWrapper::quinticNoise1(float x)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	return FastMath::lerp(integerNoise1(ix), integerNoise1(ix+1), FastMath::qCurve(x - ix));
}

float NoiseWrapper::quinticNoise2(float x, float y)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int ix1 = ix+1; int iy1 = iy+1;
	
	x = FastMath::qCurve(x - ix);
	y = FastMath::qCurve(y - iy);

	return	FastMath::lerp(	FastMath::lerp(	integerNoise2(ix ,iy ),
											integerNoise2(ix1,iy ), x),
							FastMath::lerp(	integerNoise2(ix ,iy1),
											integerNoise2(ix1,iy1), x), y);
}

float NoiseWrapper::quinticNoise3(float x, float y, float z)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int iz = FastMath::f2iRoundFast(z - 0.5f);
	int ix1 = ix+1; int iy1 = iy+1; int iz1 = iz+1;

	x = FastMath::qCurve(x - ix);
	y = FastMath::qCurve(y - iy);
	z = FastMath::qCurve(z - iz);

	return	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	integerNoise3(ix ,iy ,iz ),
															integerNoise3(ix1,iy ,iz ), x),
											FastMath::lerp(	integerNoise3(ix ,iy1,iz ),
															integerNoise3(ix1,iy1,iz ), x), y),
							FastMath::lerp(	FastMath::lerp(	integerNoise3(ix ,iy ,iz1),
															integerNoise3(ix1,iy ,iz1), x),
											FastMath::lerp(	integerNoise3(ix ,iy1,iz1),
															integerNoise3(ix1,iy1,iz1), x), y), z);
}

float NoiseWrapper::quinticNoise4(float x, float y, float z, float w)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int iz = FastMath::f2iRoundFast(z - 0.5f);
	int iw = FastMath::f2iRoundFast(w - 0.5f);
	int ix1 = ix+1; int iy1 = iy+1; int iz1 = iz+1; int iw1 = iw+1;

	x = FastMath::qCurve(x - ix);
	y = FastMath::qCurve(y - iy);
	z = FastMath::qCurve(z - iz);
	w = FastMath::qCurve(w - iw);

	return	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz ,iw ),
																			integerNoise4(ix1,iy ,iz ,iw ), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz ,iw ),
																			integerNoise4(ix1,iy1,iz ,iw ), x), y),
											FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz1,iw ),
																			integerNoise4(ix1,iy ,iz1,iw ), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz1,iw ),
																			integerNoise4(ix1,iy1,iz1,iw ), x), y), z),
							FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz ,iw1),
																			integerNoise4(ix1,iy ,iz ,iw1), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz ,iw1),
																			integerNoise4(ix1,iy1,iz ,iw1), x), y),
											FastMath::lerp(	FastMath::lerp(	integerNoise4(ix ,iy ,iz1,iw1),
																			integerNoise4(ix1,iy ,iz1,iw1), x),
															FastMath::lerp(	integerNoise4(ix ,iy1,iz1,iw1),
																			integerNoise4(ix1,iy1,iz1,iw1), x), y), z), w);
}

//----------------------------------------------------------------------------------------
// The following functions output "improved" Perlin noise, and do not require the use of
// the integer noise basis function, so are therefor not affected by the seed values. The
// multipliers for return values are present to scale the results into the range of
// classic Perlin noise, and prevent oversaturation of fBm results.
//----------------------------------------------------------------------------------------
float NoiseWrapper::perlinNoise1(float x)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);

	x -= ix;
	ix &= 255;

	return FastMath::lerp(grad(p[ix], x), grad(p[ix+1], x-1), FastMath::qCurve(x)) * PERLIN1_MULT;
}

float NoiseWrapper::perlinNoise2(float x, float y)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);

	x -= ix; y -= iy;
	float x1 = x-1.0f; float y1 = y-1.0f;

	ix &= 255; iy &= 255;

	float s = FastMath::qCurve(x);
	float t = FastMath::qCurve(y);

	int a = p[ix  ]+iy;
	int b = p[ix+1]+iy;

	return	FastMath::lerp(	FastMath::lerp(	grad(p[a  ],x ,y ),
											grad(p[b  ],x1,y ), s),
							FastMath::lerp(	grad(p[a+1],x ,y1),
											grad(p[b+1],x1,y1), s), t) * PERLIN2_MULT;
}

float NoiseWrapper::perlinNoise3(float x, float y, float z)
{
	// Find unit grid cell containing point
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int iz = FastMath::f2iRoundFast(z - 0.5f);

	// Find relative x y z of point in cube
	x -= ix; y -= iy; z -= iz;
	float x1 = x-1.0f; float y1 = y-1.0f; float z1 = z-1.0f;

	// Wrap integer cells at a 255 period
	ix &= 255; iy &= 255; iz &= 255;

    float s = FastMath::qCurve(x);
	float t = FastMath::qCurve(y);
	float r = FastMath::qCurve(z);

	// Hash coordinates of the 8 cube corners
	int a = p[ix  ]+iy; int aa = p[a]+iz; int ab = p[a+1]+iz;
	int b = p[ix+1]+iy; int ba = p[b]+iz; int bb = p[b+1]+iz;

	// Add blended results from 8 cube corners
	return	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	grad(p[aa  ],x ,y ,z ),
															grad(p[ba  ],x1,y ,z ), s),
											FastMath::lerp(	grad(p[ab  ],x ,y1,z ),
															grad(p[bb  ],x1,y1,z ), s), t),
							FastMath::lerp(	FastMath::lerp(	grad(p[aa+1],x ,y ,z1),
															grad(p[ba+1],x1,y ,z1), s),
											FastMath::lerp(	grad(p[ab+1],x ,y1,z1),
															grad(p[bb+1],x1,y1,z1), s), t), r); // * PERLIN3_MULT;
}

float NoiseWrapper::perlinNoise4(float x, float y, float z, float w)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int iz = FastMath::f2iRoundFast(z - 0.5f);
	int iw = FastMath::f2iRoundFast(w - 0.5f);

	x -= ix; y -= iy; z -= iz; w -= iw;
	float x1 = x-1.0f; float y1 = y-1.0f; float z1 = z-1.0f; float w1 = w-1.0f;

	ix &= 255; iy &= 255; iz &= 255; iw &= 255;

    float s = FastMath::qCurve(x);
	float t = FastMath::qCurve(y);
	float r = FastMath::qCurve(z);
	float q = FastMath::qCurve(w);

	// Hash coordinates of the 16 corners
	int a = p[ix  ]+iy; int aa = p[a]+iz; int ab = p[a+1]+iz;
	int b = p[ix+1]+iy; int ba = p[b]+iz; int bb = p[b+1]+iz;
	int aaa = p[aa]+iw; int aba = p[ab]+iw; int aab = p[aa+1]+iw; int abb = p[ab+1]+iw;
	int	baa = p[ba]+iw; int bba = p[bb]+iw; int bab = p[ba+1]+iw; int bbb = p[bb+1]+iw;

	// Add blended results from 16 corners
	return	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	grad(p[aaa  ],x ,y ,z ,w ),
																			grad(p[baa  ],x1,y ,z ,w ), s),
															FastMath::lerp(	grad(p[aba  ],x ,y1,z ,w ),
																			grad(p[bba  ],x1,y1,z ,w ), s), t),
											FastMath::lerp(	FastMath::lerp(	grad(p[aab  ],x ,y ,z1,w ),
																			grad(p[bab  ],x1,y ,z1,w ), s),
															FastMath::lerp(	grad(p[abb  ],x ,y1,z1,w ),
																			grad(p[bbb  ],x1,y1,z1,w ), s), t), r),
							FastMath::lerp(	FastMath::lerp(	FastMath::lerp(	grad(p[aaa+1],x ,y ,z ,w1),
																			grad(p[baa+1],x1,y ,z ,w1), s),
															FastMath::lerp(	grad(p[aba+1],x ,y1,z ,w1),
																			grad(p[bba+1],x1,y1,z ,w1), s), t),
											FastMath::lerp(	FastMath::lerp(	grad(p[aab+1],x ,y ,z1,w1),
																			grad(p[bab+1],x1,y ,z1,w1), s),
															FastMath::lerp(	grad(p[abb+1],x ,y1,z1,w1),
																			grad(p[bbb+1],x1,y1,z1,w1), s), t), r), q) * PERLIN4_MULT;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
float NoiseWrapper::simplexNoise1(float x)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	
	x -= ix;
	float x1 = x - 1.0f;
	
	ix &= 255;

	float t0 = 1.0f - x*x;   t0 *= t0;
	float t1 = 1.0f - x1*x1; t1 *= t1;

	// the maximum value of this noise is 8*(3/4)^4 = 2.53125
	// so a factor of 0.395 scales it to fit exactly within [-1,1]
	return 0.395f * ((t0 * t0 * grad(p[ix], x)) + (t1 * t1 * grad(p[ix+1], x1)));
}

float NoiseWrapper::simplexNoise2(float x, float y)
{
	#define F2	0.366025403f	// F2 = 0.5*(sqrt(3.0)-1.0)
	#define G2	0.211324865f	// G2 = (3.0-sqrt(3.0))/6.0

	float s = (x + y) * F2;
	int ix = FastMath::f2iRoundFast(x+s - 0.5f);
	int iy = FastMath::f2iRoundFast(y+s - 0.5f);

	float t = (ix + iy) * G2;
	x -= (ix - t);
	y -= (iy - t);

	int ix1, iy1;
	if (x > y) {
		ix1 = 1; iy1 = 0;
	} else {
		ix1 = 0; iy1 = 1;
	}

	float x1 = x - ix1 + G2;
	float y1 = y - iy1 + G2;
	float x2 = x - 1.0f + (2.0f * G2);
	float y2 = y - 1.0f + (2.0f * G2);

	ix &= 255; iy &= 255;

	float n = 0.0f;
	t = 0.5f - x*x - y*y;
	if (t >= 0.0f) {
		t *= t;
		n = t * t * grad(p[ix + p[iy]], x, y);
	}

	t = 0.5f - x1*x1 - y1*y1;
	if (t >= 0.0f) {
		t *= t;
		n += t * t * grad(p[ix + ix1 + p[iy + iy1]], x1, y1);
	}

	t = 0.5f - x2*x2 - y2*y2;
	if (t >= 0.0f) {
		t *= t;
		n += t * t * grad(p[ix + 1 + p[iy + 1]], x2, y2);
	}

	return 40.0f * n;
}

float NoiseWrapper::simplexNoise3(float x, float y, float z)
{
	#define F3	0.333333333f
	#define G3	0.166666667f

	float s = (x+y+z) * F3;
	int ix = FastMath::f2iRoundFast(x+s - 0.5f);
	int iy = FastMath::f2iRoundFast(y+s - 0.5f);
	int iz = FastMath::f2iRoundFast(z+s - 0.5f);

	float t = (ix + iy + iz) * G3; 
	x -= (ix - t);
	y -= (iy - t);
	z -= (iz - t);

	int ix1, iy1, iz1;
	int ix2, iy2, iz2;

	if (x >= y) {
		if (y >= z) {
			ix1=1; iy1=0; iz1=0; ix2=1; iy2=1; iz2=0;
		} else if (x >= z) {
			ix1=1; iy1=0; iz1=0; ix2=1; iy2=0; iz2=1;
		} else {
			ix1=0; iy1=0; iz1=1; ix2=1; iy2=0; iz2=1;
		}
	} else {
		if (y < z) {
			ix1=0; iy1=0; iz1=1; ix2=0; iy2=1; iz2=1;
		} else if (x < z) {
			ix1=0; iy1=1; iz1=0; ix2=0; iy2=1; iz2=1;
		} else {
			ix1=0; iy1=1; iz1=0; ix2=1; iy2=1; iz2=0;
		}
	}

	float x1 = x - ix1 + G3;
	float y1 = y - iy1 + G3;
	float z1 = z - iz1 + G3;
	float x2 = x - ix2 + (2.0f * G3);
	float y2 = y - iy2 + (2.0f * G3);
	float z2 = z - iz2 + (2.0f * G3);
	float x3 = x - 1.0f + (3.0f * G3);
	float y3 = y - 1.0f + (3.0f * G3);
	float z3 = z - 1.0f + (3.0f * G3);

	ix &= 255; iy &= 255; iz &= 255;

	float n = 0.0f;
	t = 0.6f - x*x - y*y - z*z;
	if (t >= 0.0f) {
		t *= t;
		n = t * t * grad(p[ix + p[iy + p[iz]]], x, y, z);
	}

	t = 0.6f - x1*x1 - y1*y1 - z1*z1;
	if (t >= 0.0f) {
		t *= t;
		n += t * t * grad(p[ix + ix1 + p[iy + iy1 + p[iz + iz1]]], x1, y1, z1);
	}

	t = 0.6f - x2*x2 - y2*y2 - z2*z2;
	if (t >= 0.0f) {
		t *= t;
		n += t * t * grad(p[ix + ix2 + p[iy + iy2 + p[iz + iz2]]], x2, y2, z2);
	}

	t = 0.6f - x3*x3 - y3*y3 - z3*z3;
	if (t >= 0.0f) {
		t *= t;
		n += t * t * grad(p[ix + 1 + p[iy + 1 + p[iz + 1]]], x3, y3, z3);
	}

	return 32.0f * n;
}

float NoiseWrapper::simplexNoise4(float x, float y, float z, float w)
{
	// the skewing and unskewing factors are hairy again for the 4D case
	#define F4	0.309016994f	// F4 = (sqrt(5.0)-1.0)/4.0
	#define G4	0.138196601f	// G4 = (5.0-sqrt(5.0))/20.0

	// skew the (x,y,z,w) space to determine which cell of 24 simplices we're in
	float s = (x + y + z + w) * F4;	// factor for 4D skewing
	int ix = FastMath::f2iRoundFast(x+s - 0.5f);
	int iy = FastMath::f2iRoundFast(y+s - 0.5f);
	int iz = FastMath::f2iRoundFast(z+s - 0.5f);
	int iw = FastMath::f2iRoundFast(w+s - 0.5f);

	float t = (ix + iy + iz + iw) * G4;	// factor for 4D unskewing
	x -= (ix - t);  // unskew the cell origin back to (x,y,z,w) space
	y -= (iy - t);	// the x,y,z,w distances from the cell origin
	z -= (iz - t);
	w -= (iw - t);

	// The method below finds the ordering of x,y,z,w and then finds the correct
	// traversal order for the simplex we�re in.
	int c =	((x > y) ? 32 : 0) +
			((x > z) ? 16 : 0) +
			((y > z) ? 8  : 0) +
			((x > w) ? 4  : 0) +
			((y > w) ? 2  : 0) +
			((z > w) ? 1  : 0);

	// simplex[c] is a 4-vector with the numbers 0, 1, 2 and 3 in some order
	// many values of c will never occur, since e.g. x>y>z>w makes x<z, y<w and x<w
	// impossible. Only the 24 indices which have non-zero entries make any sense.
	// We use a thresholding to set the coordinates in turn from the largest magnitude.
	// the number 3 in the "simplex" array is at the position of the largest coordinate.
	int ix1 = (simplex[c][0] >= 3) ? 1 : 0;	// the integer offsets for the second simplex corner
	int iy1 = (simplex[c][1] >= 3) ? 1 : 0;
	int iz1 = (simplex[c][2] >= 3) ? 1 : 0;
	int iw1 = (simplex[c][3] >= 3) ? 1 : 0;
	// the number 2 in the "simplex" array is at the second largest coordinate.
	int ix2 = (simplex[c][0] >= 2) ? 1 : 0; // the integer offsets for the third simplex corner
	int iy2 = (simplex[c][1] >= 2) ? 1 : 0;
	int iz2 = (simplex[c][2] >= 2) ? 1 : 0;
	int iw2 = (simplex[c][3] >= 2) ? 1 : 0;
	// the number 1 in the "simplex" array is at the second smallest coordinate.
	int ix3 = (simplex[c][0] >= 1) ? 1 : 0; // the integer offsets for the fourth simplex corner
	int iy3 = (simplex[c][1] >= 1) ? 1 : 0;
	int iz3 = (simplex[c][2] >= 1) ? 1 : 0;
	int iw3 = (simplex[c][3] >= 1) ? 1 : 0;
	// the fifth corner has all coordinate offsets = 1, so no need to look that up.

	float x1 = x - ix1 + G4;			// offsets for second corner in (x,y,z,w) coords
	float y1 = y - iy1 + G4;
	float z1 = z - iz1 + G4;
	float w1 = w - iw1 + G4;
	float x2 = x - ix2 + (2.0f * G4);	// offsets for third corner in (x,y,z,w) coords
	float y2 = y - iy2 + (2.0f * G4);
	float z2 = z - iz2 + (2.0f * G4);
	float w2 = w - iw2 + (2.0f * G4);
	float x3 = x - ix3 + (3.0f * G4);	// offsets for fourth corner in (x,y,z,w) coords
	float y3 = y - iy3 + (3.0f * G4);
	float z3 = z - iz3 + (3.0f * G4);
	float w3 = w - iw3 + (3.0f * G4);
	float x4 = x - 1.0f + (4.0f * G4);	// offsets for last corner in (x,y,z,w) coords
	float y4 = y - 1.0f + (4.0f * G4);
	float z4 = z - 1.0f + (4.0f * G4);
	float w4 = w - 1.0f + (4.0f * G4);

	// wrap the integer indices at 256, to avoid indexing p[] out of bounds
	ix &= 255; iy &= 255; iz &= 255; iw &= 255;

	// calculate the contribution from the five corners
	float n = 0.0f;
	t = 0.6f - x*x - y*y - z*z - w*w;
	if (t >= 0.0f) {
		t *= t;
		n = t * t * grad(p[ix + p[iy + p[iz + p[iw]]]], x, y, z, w);
	}

	t = 0.6f - x1*x1 - y1*y1 - z1*z1 - w1*w1;
	if (t >= 0.0f) {
		t *= t;
		n += t * t * grad(p[ix + ix1 + p[iy + iy1 + p[iz + iz1 + p[iw + iw1]]]], x1, y1, z1, w1);
	}

	t = 0.6f - x2*x2 - y2*y2 - z2*z2 - w2*w2;
	if (t >= 0.0f) {
		t *= t;
		n += t * t * grad(p[ix + ix2 + p[iy + iy2 + p[iz + iz2 + p[iw + iw2]]]], x2, y2, z2, w2);
	}

	t = 0.6f - x3*x3 - y3*y3 - z3*z3 - w3*w3;
	if (t >= 0.0f) {
		t *= t;
		n += t * t * grad(p[ix + ix3 + p[iy + iy3 + p[iz + iz3 + p[iw + iw3]]]], x3, y3, z3, w3);
	}

	t = 0.6f - x4*x4 - y4*y4 - z4*z4 - w4*w4;
	if (t >= 0.0f) {
		t *= t;
		n += t * t * grad(p[ix + 1 + p[iy + 1 + p[iz + 1 + p[iw + 1]]]], x4, y4, z4, w4);
	}

	return 27.0f * n;
}

//----------------------------------------------------------------------------------------
// The following uses the CoherentNoiseType flag to determine output
// numDimensions is 1, 2 or 3, with equal elements in v
//----------------------------------------------------------------------------------------
float NoiseWrapper::coherentNoise(int numDimensions, float *v)
{
	_ASSERTE(numDimensions >= 1 && numDimensions <= 4);

	#define switchDimensions(func1,func2,func3,func4)		switch (numDimensions) {\
																case 1: return func1(v[0]);\
																case 2: return func2(v[0],v[1]);\
																case 3: return func3(v[0],v[1],v[2]);\
																case 4: return func4(v[0],v[1],v[2],v[3]);}

	switch (cNoiseType) {
		case NOISE_TYPE_LINEAR:
			switchDimensions(linearNoise1, linearNoise2, linearNoise3, linearNoise4)
			
		case NOISE_TYPE_COSINE:
			switchDimensions(cosineNoise1, cosineNoise2, cosineNoise3, cosineNoise4)

		case NOISE_TYPE_CUBIC:
			switchDimensions(cubicNoise1, cubicNoise2, cubicNoise3, cubicNoise4)
			
		case NOISE_TYPE_QUINTIC:
			switchDimensions(quinticNoise1, quinticNoise2, quinticNoise3, quinticNoise4)
			
		case NOISE_TYPE_PERLIN:
			switchDimensions(perlinNoise1, perlinNoise2, perlinNoise3, perlinNoise4)

		case NOISE_TYPE_SIMPLEX:
			switchDimensions(simplexNoise1, simplexNoise2, simplexNoise3, simplexNoise4)

case NOISE_TYPE_TEST:
	switchDimensions(testNoise1, testNoise2, testNoise3, testNoise4)

case NOISE_TYPE_CACHED:
	switchDimensions(cachedNoise1, cachedNoise2, cachedNoise3, cachedNoise4)

	}
	return 0.0f;
}

// Fractal methods

//----------------------------------------------------------------------------------------
// Fractional Brownian Motion
// persistence is aka gain
//----------------------------------------------------------------------------------------
float NoiseWrapper::fBm(int numDimensions, float *v, int octaves, float lacunarity, float persistence, float amplitudeStart)
{
	_ASSERTE(octaves >= 1 && octaves <= NOISE_MAX_OCTAVES);

	float amplitude = amplitudeStart;

	float value = 0.0f;
	if (fOperation == NOISE_MFO_MULTIPLY || fOperation == NOISE_MFO_MULTIPLY_ABS || fOperation == NOISE_MFO_EXP) value = 1.0f;

	for (int o = 0; o < octaves; ++o) {
		switch (fOperation) { // pick the fractal operation to perform at each octave
			case NOISE_MFO_ADD:
				value += coherentNoise(numDimensions, v) * amplitude;
				break;
			case NOISE_MFO_MULTIPLY:				
				value *= coherentNoise(numDimensions, v) * amplitude;
				break;
			case NOISE_MFO_ADD_ABS:
				value += FastMath::abs(coherentNoise(numDimensions, v)) * amplitude;
				break;
			case NOISE_MFO_MULTIPLY_ABS:
				value *= FastMath::abs(coherentNoise(numDimensions, v)) * amplitude;
				break;
			case NOISE_MFO_POW:
				value = powf(value, coherentNoise(numDimensions, v)) * amplitude;
				break;
			case NOISE_MFO_EXP:
				value *= expf(coherentNoise(numDimensions, v)) * amplitude;
				break;
		}		

		amplitude *= persistence;
		for (int i = 0; i < numDimensions; ++i) v[i] *= lacunarity;
	}

	return value;
}

float NoiseWrapper::multiFractal(int numDimensions, float *v, int octaves, float lacunarity, float roughness, float amplitudeStart)
{
	_ASSERTE(octaves >= 1 && octaves <= NOISE_MAX_OCTAVES);

	float amplitude = amplitudeStart;

	float value = 0.0f;
	if (fOperation == NOISE_MFO_MULTIPLY || fOperation == NOISE_MFO_MULTIPLY_ABS || fOperation == NOISE_MFO_EXP) value = 1.0f;

	for (int o = 0; o < octaves; ++o) {
		float octaveValue = coherentNoise(numDimensions, v);
		switch (fOperation) { // pick the fractal operation to perform at each octave
			case NOISE_MFO_ADD:
				value += octaveValue * amplitude;
				break;
			case NOISE_MFO_MULTIPLY:				
				value *= octaveValue * amplitude;
				break;
			case NOISE_MFO_ADD_ABS:
				value += FastMath::abs(octaveValue) * amplitude;
				break;
			case NOISE_MFO_MULTIPLY_ABS:
				value *= FastMath::abs(octaveValue) * amplitude;
				break;
			case NOISE_MFO_POW:
				value = powf(value, octaveValue) * amplitude;
				break;
			case NOISE_MFO_EXP:
				value *= expf(octaveValue) * amplitude;
				break;
		}		

		float fromSeaLevel = (value + 1.0f) * 0.5f;
		amplitude = FastMath::abs(fromSeaLevel * fromSeaLevel * octaveValue * roughness);
		for (int i = 0; i < numDimensions; ++i) v[i] *= lacunarity;
	}

	return value;
}

// TEMP
float NoiseWrapper::testNoise1(float x) { return 0; }
float NoiseWrapper::testNoise3(float x, float y, float z) { return 0; }
float NoiseWrapper::testNoise4(float x, float y, float z, float w) { return 0; }

float NoiseWrapper::testNoise2(float x, float y)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);

	x -= ix; y -= iy;
	//float x1 = x-1.0f; float y1 = y-1.0f;

	ix &= 255; iy &= 255;

    float s = FastMath::qCurve(x);
	float t = FastMath::qCurve(y);

	int a = p[ix  ]+iy;
	int b = p[ix+1]+iy;

	return	FastMath::lerp(	FastMath::lerp(	grad4[a],// grad(p[a  ],x ,y ),
											grad4[b], s),//grad(p[b  ],x1,y ), s),
							FastMath::lerp(	grad4[a+1],//grad(p[a+1],x ,y1),
											grad4[b+1], s), t); //grad(p[b+1],x1,y1), s), t) * PERLIN2_MULT;
}

float NoiseWrapper::cachedNoise1(float x) { return 0; }
float NoiseWrapper::cachedNoise3(float x, float y, float z) { return 0; }
float NoiseWrapper::cachedNoise4(float x, float y, float z, float w) { return 0; }

float NoiseWrapper::cachedNoise2(float x, float y)
{
	int ix = FastMath::f2iRoundFast(x - 0.5f);
	int iy = FastMath::f2iRoundFast(y - 0.5f);
	int ix1 = ix+1; int iy1 = iy+1;
	
	x = FastMath::qCurve(x - ix);
	y = FastMath::qCurve(y - iy);

	return	FastMath::lerp(	FastMath::lerp(	n[iy *256+ix ],
											n[iy *256+ix1], x),
							FastMath::lerp(	n[iy1*256+ix ],
											n[iy1*256+ix1], x), y);
}

// TEMP