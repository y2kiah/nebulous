/*----==== NOISEWRAPPER.H ====---- */

#pragma once

#include "FastMath.h"


///// DEFINES /////

#define NOISE_MAX_OCTAVES		20

enum CoherentNoiseType : unsigned char {
	NOISE_TYPE_LINEAR = 0,
	NOISE_TYPE_COSINE,
	NOISE_TYPE_CUBIC,
	NOISE_TYPE_QUINTIC,
	NOISE_TYPE_PERLIN,
	NOISE_TYPE_SIMPLEX,
NOISE_TYPE_TEST,
NOISE_TYPE_CACHED
};

enum MultiFractalOperation : unsigned char {
	NOISE_MFO_ADD = 0,
	NOISE_MFO_MULTIPLY,
	NOISE_MFO_ADD_ABS,
	NOISE_MFO_MULTIPLY_ABS,
	NOISE_MFO_POW,
	NOISE_MFO_EXP
};


///// STRUCTURES /////

/*=============================================================================
=============================================================================*/
class NoiseWrapper {
	private:
		///// VARIABLES /////

// temptest gradient table
static float grad4[512];
static float n[65536];
// TEMP //

		// Perlin noise precomputed permutations, also used in simplex noise
		static const unsigned char		p[512];
		// 4D Simplex noise lookup table
		static const unsigned char		simplex[64][4];

		// Noise generator settings
		static int						xSeed, ySeed;	// integer noise seeds for each dimension, defaults 0,0,0
		static int						zSeed, wSeed;
		static CoherentNoiseType		cNoiseType;		// the type of coherent noise generated to feed fractal function, default NOISE_TYPE_PERLIN
		static MultiFractalOperation	fOperation;		// operator used at each octave of fBm function

		///// FUNCTIONS /////
		
		// Perlin noise gradient methods, also used in simplex noise
		inline static float		grad(int hash, float x);
		inline static float		grad(int hash, float x, float y);
		inline static float		grad(int hash, float x, float y, float z);
		inline static float		grad(int hash, float x, float y, float z, float w);

		// Constructor / Destructor
		explicit NoiseWrapper() {}	// private constructor to prevent instantiation
		~NoiseWrapper() {}

	public:
// TEMP
static void initGrad4();
// TEMP
		// Accessors
		static int		getXSeed() { return xSeed; }
		static int		getYSeed() { return ySeed; }
		static int		getZSeed() { return zSeed; }
		static int		getWSeed() { return wSeed; }
		static CoherentNoiseType		getCoherentNoiseType() { return cNoiseType; }
		static MultiFractalOperation	getMultiFractalOperation() { return fOperation; }

		// Administrative methods
		static void		setXSeed(int s) { xSeed = s; }
		static void		setYSeed(int s) { ySeed = s; }
		static void		setZSeed(int s) { zSeed = s; }
		static void		setWSeed(int s) { wSeed = s; }
		static void		setCoherentNoiseType(CoherentNoiseType t) { cNoiseType = t; }
		static void		setMultiFractalOperation(MultiFractalOperation f) { fOperation = f; }

		// Integer noise methods (discrete)
		inline static float	integerNoise1(int x);
		inline static float	integerNoise2(int x, int y);
		inline static float	integerNoise3(int x, int y, int z);
		inline static float	integerNoise4(int x, int y, int z, int w);

		// Coherent noise methods (continuous, interpolate integer noise)
		//	* these can be optimized for generating large buffers by caching integer noise results, then interpolating cached noise
		//	* optimized versions that output large noise buffers are noise generator objects
		static float	linearNoise1(float x);
		static float	linearNoise2(float x, float y);
		static float	linearNoise3(float x, float y, float z);
		static float	linearNoise4(float x, float y, float z, float w);

		static float	cosineNoise1(float x);
		static float	cosineNoise2(float x, float y);
		static float	cosineNoise3(float x, float y, float z);
		static float	cosineNoise4(float x, float y, float z, float w);

		static float	cubicNoise1(float x);
		static float	cubicNoise2(float x, float y);
		static float	cubicNoise3(float x, float y, float z);
		static float	cubicNoise4(float x, float y, float z, float w);

		static float	quinticNoise1(float x);
		static float	quinticNoise2(float x, float y);
		static float	quinticNoise3(float x, float y, float z);
		static float	quinticNoise4(float x, float y, float z, float w);
		
		static float	perlinNoise1(float x);	// fast gradient noise as implemented by Perlin, "improved" version
		static float	perlinNoise2(float x, float y);
		static float	perlinNoise3(float x, float y, float z);
		static float	perlinNoise4(float x, float y, float z, float w);

		static float	simplexNoise1(float x);
		static float	simplexNoise2(float x, float y);
		static float	simplexNoise3(float x, float y, float z);
		static float	simplexNoise4(float x, float y, float z, float w);

static float	testNoise1(float x);
static float	testNoise2(float x, float y);
static float	testNoise3(float x, float y, float z);
static float	testNoise4(float x, float y, float z, float w);

static float	cachedNoise1(float x);
static float	cachedNoise2(float x, float y);
static float	cachedNoise3(float x, float y, float z);
static float	cachedNoise4(float x, float y, float z, float w);

		// Wrapper for coherent noise functions
		static float	coherentNoise(int numDimensions, float *v);

		// Fractal methods (coherent noise combined in octaves)
		static float	fBm(int numDimensions, float *v, int octaves, float lacunarity, float persistence, float amplitudeStart);
		static float	multiFractal(int numDimensions, float *v, int octaves, float lacunarity, float roughness, float amplitudeStart);
};


///// INLINE FUNCTIONS /////

inline float NoiseWrapper::grad(int hash, float x)
{
	hash &= 15;
	float grad = 1.0f + (hash & 7);	// gradient value 1.0, 2.0, ..., 8.0
	if (hash & 8) return -grad * x;	// and a random sign for the gradient
	else return grad * x;			// multiply the gradient with the distance
}

inline float NoiseWrapper::grad(int hash, float x, float y)
{
	hash &= 7;						// convert low 3 bits of hash code
	float s = (hash < 4) ? x : y;	// into 8 simple gradient directions
	float t = (hash < 4) ? y : x;	// and compute the dot product with (x, y)
	return ((hash & 1) ? -s : s) + ((hash & 2) ? -2.0f * t : 2.0f * t);
}

inline float NoiseWrapper::grad(int hash, float x, float y, float z)
{
	hash &= 15;						// convert low 4 bits of hash code into 12 simple
	float s = (hash < 8) ? x : y;	// gradient directions and compute dot product
	float t = (hash < 4) ? y : ((hash == 12 || hash == 14) ? x : z);	// fix repeats at hash = 12 to 15
	return ((hash & 1) ? -s : s) + ((hash & 2) ? -t : t);
}

inline float NoiseWrapper::grad(int hash, float x, float y, float z, float w)
{
	hash &= 31;						// Convert low 5 bits of hash code into 32 simple
	float s = (hash < 24) ? x : y;	// gradient directions, and compute dot product.
	float t = (hash < 16) ? y : z;
	float r = (hash < 8) ? z : w;
	return ((hash & 1) ? -s : s) + ((hash & 2) ? -t : t) + ((hash & 4) ? -r : r);
}

inline float NoiseWrapper::integerNoise1(int x)
{
	x += xSeed;
	x = (x << 13) ^ x;
	return (1.0f - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) * 0.000000000931322574615478515625f);
}

inline float NoiseWrapper::integerNoise2(int x, int y)
{
	x += xSeed; y += ySeed;
	x += y * 47;
	x = (x << 13) ^ x;
	return (1.0f - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) * 0.000000000931322574615478515625f);
}

inline float NoiseWrapper::integerNoise3(int x, int y, int z)
{
	x += xSeed; y += ySeed; z += zSeed;
	x += (y * 47) + (z * 59);
	x = (x << 13) ^ x;
	return (1.0f - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) * 0.000000000931322574615478515625f);
}

inline float NoiseWrapper::integerNoise4(int x, int y, int z, int w)
{
	x += xSeed; y += ySeed; z += zSeed; w += wSeed;
	x += (y * 47) + (z * 59) + (w * 131);
	x = (x << 13) ^ x;
	return (1.0f - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) * 0.000000000931322574615478515625f);
}
