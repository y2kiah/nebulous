/*----==== UISKIN.H ====----
	Author:		Jeff Kiah
	Orig.Date:	08/13/2009
	Rev.Date:	08/13/2009
--------------------------*/

#pragma once

#include "../Resource/ResHandle.h"
#include "../Math/TVector2.h"
#include "../Utility/Typedefs.h"

namespace UI {

///// STRUCTURES /////

/*=============================================================================
class SkinNode
=============================================================================*/
class SkinRect {
	public:
		///// DEFINITIONS /////
		enum MappingMode : int {
			MappingMode_Tile = 0,
			MappingMode_Stretch
		};
		///// VARIABLES /////
		Vector2f		mTL_UV;
		Vector2f		mBR_UV;
		Vector2f		mSizeScreenPx;
		Vector2i		mSizeTexturePx;
		MappingMode		mMappingMode;
};

/*=============================================================================
class FixedSkin
	A fixed proportion element cannot stretch in either x or y, so the aspect
	ratio of the element always remains the same. It may grow or shrink in size
	in either direction, like all UI elements, but the w/h ratio is fixed.
=============================================================================*/
class FixedSkin {
	public:
		SkinRect		mRect;
};

/*=============================================================================
class Stretch1Skin
	This defines the skin properties for an element		|------|------|------|
	that can stretch in only 1 direction (horizontal	|l     |m     |r     |
	or vertical). Divided into 3 sections, the left		|      |<---->|      |
	and right sides are fixed, the middle varies.		|------|------|------|
=============================================================================*/
class Stretch1Skin {
	public:
		SkinRect		mRect[3];		// order: left, mid, right -or- upper, mid, lower
};

/*=============================================================================
class Stretch2Skin
=============================================================================*/
class Stretch2Skin {
	public:
		SkinRect		mRect[9];		// order: ul, um, ur, ml, mm, mr, ll, lm, lr
};

/*=============================================================================
class UISkin
=============================================================================*/
class UISkin : public Resource {
	private:
		///// VARIABLES /////
		FixedSkin		mCheckbox[6];	// order: unchecked, checked, focus, focus+checked, disabled, disabled+checked
		FixedSkin		mRadioBtn[6];	// order: unchecked, checked, focus, focus+checked, disabled, disabled+checked
		FixedSkin		mComboBtn[3];	// order: enabled, focus, disabled
		FixedSkin		mBorderCrnr[4];	// order: ul, ur, ll, lr
		FixedSkin		mUpArrow[3];	// order: enabled, focus, disabled
		FixedSkin		mRightArrow[3];	// order: enabled, focus, disabled
		FixedSkin		mDownArrow[3];	// order: enabled, focus, disabled
		FixedSkin		mLeftArrow[3];	// order: enabled, focus, disabled
		
		Stretch1Skin	mButton[3];		// order: enabled, focus, disabled
		Stretch1Skin	mTitleBar[2];	// order: active, inactive
		Stretch1Skin	mBorderSide[4];	// order: top, right, bottom, left
		Stretch1Skin	mHorzSlider[3];	// order: enabled, focus, disabled
		Stretch1Skin	mVertSlider[3];	// order: enabled, focus, disabled
		Stretch1Skin	mSliderTrack[3];// order: enabled, focus, disabled
		Stretch1Skin	mScrollTrack[3];// order: enabled, focus, disabled

		Stretch2Skin	mTextBox[3];	// order: enabled, focus, disabled
		
		int				mBorderOpaqueWidth[4];	// order: top, right, bottom, left

	public:
		/*---------------------------------------------------------------------
			specifies the cache that will manager the resource
		---------------------------------------------------------------------*/
		static const ResCacheType	sCacheType = ResCache_Material;

		///// FUNCTIONS /////

		/*---------------------------------------------------------------------
			onLoad is called automatically by the resource caching system when
			a resource is first loaded from disk and added to the cache.
		---------------------------------------------------------------------*/
		virtual bool	onLoad(const BufferPtr &dataPtr, bool async);

		// Constructors / destructor
		/*---------------------------------------------------------------------
			constructor with this signature is required for the resource system
		---------------------------------------------------------------------*/
		explicit UISkin(const string &name, uint sizeB, const ResCachePtr &resCachePtr) :
			Resource(name, sizeB, resCachePtr)
		{}
		explicit UISkin() :
			Resource()
		{}
		virtual ~UISkin() {}
};

} // namespace UI