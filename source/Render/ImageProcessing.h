/*----==== IMAGEPROCESSING.H ====----
	Author:		Jeff Kiah
	Orig.Date:	07/17/2009
	Rev.Date:	07/17/2009
-----------------------------------*/

#pragma once

#include <memory>
#include "../Utility/Typedefs.h"
#include "../Math/TVector2.h"

using std::shared_ptr;

///// DEFINITIONS /////
class ImageBuffer;
typedef shared_ptr<ImageBuffer>	ImageBufferPtr;

///// STRUCTURES /////

/*=============================================================================
=============================================================================*/
class Color {
	public:
		///// VARIABLES /////
		uchar r, g, b, a;

		///// FUNCTIONS /////
		void setRGBA(uchar red, uchar green, uchar blue, uchar alpha) {
			r = red; g = green; b = blue; a = alpha;
		}
		///// OPERATORS /////
		void operator= (const Color &c) {
			r = c.r; g = c.g; b = c.b; a = c.a;
		}
		// Constructor / destructor
		Color() : r(0), g(0), b(0), a(0) {}
		Color(uchar red, uchar green, uchar blue, uchar alpha) : r(red), g(green), b(blue), a(alpha) {}
		Color(const Color &c) : r(c.r), g(c.g), b(c.b), a(c.a) {}
};

/*=============================================================================
class ImageBuffer
=============================================================================*/
class ImageBuffer {
	public:
		///// DEFINITIONS /////
		enum ImageType : int {
			Img_Type_A = 0,
			Img_Type_RGB,
			Img_Type_RGBA,
			Img_Type_ARGB,
			Img_Type_ABGR
		};

	private:
		///// VARIABLES /////
		uchar *		mpBits;			// pointer to pixel data
		uint		mWidth;			// width in pixels
		uint		mHeight;		// height in pixels
		uint		mBpp;			// Bytes per pixel (RGBA would be 4)
		uint		mPitch;			// this would normally be (width * Bpp), but could be another value
		ImageType	mImgType;		// format of the pixels stored in pBits
		bool		mOwnImageData;	// true if data has been allocated for bits, false if they
									// just point to data owned somewhere else
	public:
		///// FUNCTIONS /////
		// Accessors
		const uchar *pBits() const	{ return mpBits; }
		uint		width() const	{ return mWidth; }
		uint		height() const	{ return mHeight; }
		uint		Bpp() const		{ return mBpp; }
		uint		pitch() const	{ return mPitch; }
		ImageType	imgType() const { return mImgType; }
		bool		hasOwnImageData() const { return mOwnImageData; }

		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		bool getRGBA(uint x, uint y, Color &outColor) const;

		// Mutators
		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		void setRGBA(uint x, uint y, const Color &c);

		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		bool allocateBits(uint width, uint height, ImageType imgType);

		// Constructor / destructor
		explicit ImageBuffer() :
			mWidth(0), mHeight(0), mBpp(0), mPitch(0),
			mImgType(Img_Type_A), mpBits(0), mOwnImageData(false)
		{}
		explicit ImageBuffer(uint width, uint height, uint Bpp, uint pitch,
							 ImageType imgType, uchar *pBits) :
			mWidth(width), mHeight(height), mBpp(Bpp), mPitch(pitch),
			mImgType(imgType), mpBits(pBits), mOwnImageData(false)
		{}
		~ImageBuffer() {
			if (mOwnImageData && mpBits) { delete [] mpBits; }
		}
};

/*=============================================================================
class SDFFilter
	An image filter to create a low resolution signed distance field from a
	high resolution input image. The input image is treated as binary data
	(in/out) where any value > 0 is in, and 0 is out.
=============================================================================*/
class SDFFilter {
	private:
		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		static float findSignedDistance(int pointX, int pointY, const ImageBuffer &inImage,
										int scanWidth, int scanHeight);

	public:
		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		static ImageBufferPtr process(const ImageBuffer &inImage);
};