/*----==== IMAGEPROCESSING.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	07/17/2009
	Rev.Date:	07/17/2009
-------------------------------------*/

#include "ImageProcessing.h"
#include "../Math/FastMath.h"

////////// class ImageBuffer //////////

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
bool ImageBuffer::getRGBA(uint x, uint y, Color &outColor) const
{
	if (x >= mWidth || y >= mHeight) { return false; }
	uchar *pixel = mpBits + (y * mPitch) + (x * mBpp);
	switch (mImgType) {
		case Img_Type_A:
			outColor.setRGBA(0,0,0,pixel[0]); break;
		case Img_Type_RGB:
			outColor.setRGBA(pixel[0],pixel[1],pixel[2],0); break;
		case Img_Type_RGBA:
			outColor.setRGBA(pixel[0],pixel[1],pixel[2],pixel[3]); break;
		case Img_Type_ARGB:
			outColor.setRGBA(pixel[1],pixel[2],pixel[3],pixel[0]); break;
		case Img_Type_ABGR:
			outColor.setRGBA(pixel[3],pixel[2],pixel[1],pixel[0]); break;
		default:
			return false;
	}
	return true;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
void ImageBuffer::setRGBA(uint x, uint y, const Color &c)
{
	if (x >= mWidth || y >= mHeight) { return; }
	uchar *pixel = mpBits + (y * mPitch) + (x * mBpp);
	switch (mImgType) {
		case Img_Type_A:
			pixel[0] = c.a; break;
		case Img_Type_RGB:
			pixel[0] = c.r; pixel[1] = c.g; pixel[2] = c.b; break;
		case Img_Type_RGBA:
			pixel[0] = c.r; pixel[1] = c.g; pixel[2] = c.b; pixel[3] = c.a; break;
		case Img_Type_ARGB:
			pixel[1] = c.r; pixel[2] = c.g; pixel[3] = c.b; pixel[0] = c.a; break;
		case Img_Type_ABGR:
			pixel[3] = c.r; pixel[2] = c.g; pixel[1] = c.b; pixel[0] = c.a; break;
	}
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
bool ImageBuffer::allocateBits(uint width, uint height, ImageType imgType)
{
	if (mOwnImageData) { return false; } // if data has already been allocated, don't do it again
	mWidth = width;
	mHeight = height;
	mImgType = imgType;
	switch (mImgType) {
		case Img_Type_A:	mBpp = 1; break;
		case Img_Type_RGB:	mBpp = 3; break;
		case Img_Type_RGBA:
		case Img_Type_ARGB:
		case Img_Type_ABGR:	mBpp = 4; break;
		default: mBpp = 4; _ASSERTE(false && "New image type that isn't handled here!");
	}
	mPitch = mWidth * mBpp;
	mpBits = new uchar[mPitch * mHeight];
	mOwnImageData = true;
	return true;
}

////////// class SDFFilter //////////

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
float SDFFilter::findSignedDistance(int pointX, int pointY, const ImageBuffer &inImage,
									int scanWidth, int scanHeight)
{
	Color c;
	inImage.getRGBA(pointX, pointY, c);
	bool baseIsSolid = c.r > 0;
	
	float closestDistance = FLT_MAX;
	bool closestValid = false;

	int startY = FastMath::maxVal(pointY - (scanHeight / 2), 0);
	int endY =   FastMath::minVal(startY + scanHeight, (int)inImage.height());

	int startX = FastMath::maxVal(pointX - (scanWidth / 2), 0);
	int endX =   FastMath::minVal(startX + scanWidth, (int)inImage.width());
	
	float pointXf = (float)pointX;
	float pointYf = (float)pointY;

	for (int y = startY; y < endY; ++y) {
		for (int x = startX; x < endX; ++x) {
			inImage.getRGBA(x, y, c);
			
			if (baseIsSolid) {
				if (c.r == 0) {
					Vector2f v1(pointXf, pointYf);
					Vector2f v2((float)x, (float)y);
					float dist = v1.distSquared(v2);
					if (dist < closestDistance) {
						closestDistance = dist;
						closestValid = true;
					}
				}
			} else {
				if (c.r > 0) {
					Vector2f v1(pointXf, pointYf);
					Vector2f v2((float)x, (float)y);
					float dist = v1.distSquared(v2);
					if (dist < closestDistance) {
						closestDistance = dist;
						closestValid = true;
					}
				}
			}
		}
	}
	
	if (baseIsSolid) {
		if (closestValid) {
			return sqrtf(closestDistance);
		} else {
			return FLT_MAX;
		}
	} else {
		if (closestValid) {
			return -sqrtf(closestDistance);
		} else {
			return -FLT_MAX;
		}
	}
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
ImageBufferPtr SDFFilter::process(const ImageBuffer &inImage)
{
	debugPrintf("SignedDistanceFieldFilter processing...\n");
	
	int scanSize = 32;	// controls the size of the affect. Larger numbers will allow larger
						// outline/shadow regions at the expense of precision and longer preprocessing times
	int outWidth = inImage.width() / 8;
	int outHeight = inImage.height() / 8;
	
	const int outImageBufferSize = outWidth * outHeight;
	float *distances = new float[outImageBufferSize];
	
	int blockWidth = inImage.width() / outWidth;
	int blockHeight = inImage.height() / outHeight;
	
	debugPrintf("   block size is %i,%i\n", blockWidth, blockHeight);
	
	for (int y = 0; y < outHeight; ++y) {
		debugPrintf("   loop 1: processing row %i...\n", y);
		for (int x = 0; x < outWidth; ++x) {
			const int idx = y * outWidth + x;
			distances[idx] = findSignedDistance((x * blockWidth) + (blockWidth / 2),
												(y * blockHeight) + (blockHeight / 2),
												inImage,
												scanSize, scanSize);
		}
	}
	
	float max = 0;
	for (int y = 0; y < outHeight; ++y) {
		debugPrintf("   loop 2: processing row %i...\n", y);
		for (int x = 0; x < outWidth; ++x) {
			const int idx = y * outWidth + x;
			const float d = distances[idx];
			if (d != FLT_MAX && d > max) { max = d; }
		}
	}
	float min = 0;
	for (int y = 0; y < outHeight; ++y) {
		debugPrintf("   loop 3: processing row %i...\n", y);
		for (int x = 0; x < outWidth; ++x) {
			const int idx = y * outWidth + x;
			const float d = distances[idx];
			if (d != -FLT_MAX && d < min) { min = d; }
		}
	}
	
	float range = max - min;
	float scale = FastMath::maxVal(FastMath::abs(min), FastMath::abs(max));
	float invScale = 1.0f / scale;
	
	debugPrintf("   Max:%f, Min:%f, Range:%f\n", max, min, range);
	
	for (int y = 0; y < outHeight; ++y) {
		debugPrintf("   loop 4: processing row %i...\n", y);
		for (int x = 0; x < outWidth; ++x) {
			const int idx = y * outWidth + x;
			float d = distances[idx];
			
			if (d == FLT_MAX) {
				d = 1.0f;
			} else if (d == -FLT_MAX) {
				d = 0.0f;
			} else {
				d *= invScale;
				d *= 0.5f;
				d += 0.5f;
			}
			
			distances[idx] = d;
		}
	}

	// create the output image
	ImageBufferPtr outImagePtr(new ImageBuffer());
	ImageBuffer &outImage = *(static_cast<ImageBuffer*>(outImagePtr.get()));
	outImage.allocateBits(outWidth, outHeight, ImageBuffer::Img_Type_RGB); // output as alpha

	for (int y = 0; y < outHeight; ++y) {
		for (int x = 0; x < outWidth; ++x) {
			const int idx = y * outWidth + x;
			float d = distances[idx];
			d = FastMath::maxVal(FastMath::minVal(d, 1.0f), 0.0f);
			// output as greyscale (for now)
			uchar uc = (uchar)(d * 255);
			outImage.setRGBA(x, y, Color(uc, uc, uc, 255));
		}
	}
	delete [] distances;
	
	return outImagePtr;
}