/*----==== FONT_TTF.H ====----
	Author:		Jeff Kiah
	Orig.Date:	07/09/2009
	Rev.Date:	07/13/2009
----------------------------*/

#pragma once

#include "../Win32.h"
#include "../Utility/Typedefs.h"
#include <bitset>
#include <string>

using std::bitset;
using std::string;

///// STRUCTURES /////

class Font_TTF {
	public:
		///// DEFINITIONS /////
		enum : int {
			FIRST_CHAR = 32,
			LAST_CHAR = 126,
			NUM_CHARS = LAST_CHAR-FIRST_CHAR+1
		};
		enum FontWeight : int {
			Wt_DontCare = 0,
			Wt_Thin = 100,
			Wt_Normal = 400,
			Wt_Bold = 700,
			Wt_Heavy = 900
		};
		struct CharMetrics {
			uint width;			// the total width of the character [ = b + max(abcA,0) + max(abcC,0) ]
			uint preAdvance;	// the horizontal distance to move the cursor before rendering [ = min(abcA,0) ]
			uint postAdvance;	// the horizontal distance to move the cursor after rendering [ = abcB + abcC ]
			uint posX, posY;	// the upper-left x,y coordinate of the character on the bitmap
		};

	private:
		///// VARIABLES /////
		// initialization flags
		enum FontInitFlags {
			INIT_DC = 0,
			INIT_FONT,
			INIT_BITMAP,
			INIT_MAX
		};
		bitset<INIT_MAX>	mInitFlags;

	protected:
		HFONT			mhFont;
		HFONT			mhFontOld;
		HBITMAP			mhBitmap;
		HBITMAP			mhBitmapOld;
		HDC				mhFontDC;
		uint			mCharHeight;
		uint			mPointSize;
		CharMetrics		mCharMetrics[NUM_CHARS];
		TEXTMETRIC		mTextMetrics;
		DWORD *			mpBits;
		string			mFontName;
		FontWeight		mFontWeight;
		bool			mItalic, mUnderline, mStrikeout;
		bool			mAntialiased;

	public:
		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Picks the smallest power of 2 size for a square texture that will
			fit all ANSI characters of the font. Returns 0 if no texture size
			in the range 32-4096 will fit the font.
		---------------------------------------------------------------------*/
		uint	predictTextureSize(uint pointSize, uint gap) const;

		/*---------------------------------------------------------------------
			Picks the largest point size that will fit onto a square pwr 2
			texture of the specified size. Returns 0 if no font size 8 or
			greater will fit on the texture.
		---------------------------------------------------------------------*/
		uint	predictFontSize(uint textureSize, uint gap) const;

		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		bool	loadTrueTypeFont(uint pointSize);
		
		/*---------------------------------------------------------------------
			Renders the glyphs to a bitmap in memory. If pointSize is 0, the
			bitmap size will be set by textureSize (which must be pwr 2) and
			the largest font size that fits within the texture will be
			determined automatically. If pointSize is > 0, textureSize will be
			ignored and the smallest pwr 2 size that fits the font size will be
			determined automatically. The specified number of pixels in gap
			will border each glyph.
		---------------------------------------------------------------------*/
		bool	renderToBitmap(uint textureSize, uint gap);
		
		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		void	destroyFont();

		// Constructor / destructor
		explicit Font_TTF(const string &fontName, FontWeight weight,
						  bool italic = false, bool underline = false, bool strikeout = false,
						  bool antialiased = true);
		~Font_TTF();
};