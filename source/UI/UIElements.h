/*----==== UIELEMENTS.H ====----
	Author:		Jeff Kiah
	Orig.Date:	08/12/2009
	Rev.Date:	08/13/2009
------------------------------*/

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <boost/any.hpp>
#include "../Utility/Typedefs.h"

using std::string;	using std::vector;
using boost::any;	using std::shared_ptr;

namespace UI {

///// STRUCTURES /////

/*=============================================================================
class Element
=============================================================================*/
class Element {
	public:
		///// DEFINITIONS /////
		typedef shared_ptr<Element>		ElementPtr;
		typedef vector<ElementPtr>		ElementList;

	private:
	protected:
		///// VARIABLES /////
		string		mID;
		uint		mWidth, mHeight;
		bool		mVisible;
		bool		mHasFocus;
		ElementList	mElements;
		ElementPtr	mParent;

	public:
		///// FUNCTIONS /////
		void		show() {}
		void		hid() {}
		void		activate() {}
		void		blur() {}

		// Constructor / destructor
		explicit Element() {}
		~Element() {}
};

/*=============================================================================
class FormElement
=============================================================================*/
class FormElement : public Element {
	private:
	protected:
		///// VARIABLES /////
		string		mName;
		bool		mDisabled;
		any			value;

	public:
		///// FUNCTIONS /////

		// Constructor / destructor
		explicit FormElement() :
			Element()
		{}
		~FormElement() {}
};

/*=============================================================================
class Label
=============================================================================*/
class Label : public Element {
	private:
		///// VARIABLES /////
		string		mText;
		//Font		mFont;
		//Color		mColor;
		//FontEffect	mFontEffect;
		//Color		mFontEffectColor;
		float		mFontEffectOpacity;
		bool		mFontBorder;
		//Color		mFontBorderColor;
		float		mFontBorderWidth;

	public:
		///// FUNCTIONS /////

		// Constructor / destructor
		explicit Label() :
			Element()
		{}
		~Label() {}
};

} // namespace UI