/*----==== SCRIPTINGEVENTS.H ====----
	Author: Jeffrey Kiah
	Orig.Date: 05/06/2009
	Rev.Date:  05/06/2009
-----------------------------------*/

#pragma once

#include <string>
#include "../Event/Event.h"

using std::string;

/*=============================================================================
class LuaFunctionEvent
=============================================================================*/
class LuaFunctionEvent : public Event {
	private:
		string		mFuncName;
		LuaObject	mParamTable;
		LuaObject	mReturnObj;

	public:
		///// VARIABLES /////
		static const string		sEventType; // made public so listeners can register via this variable
											// and not the actual string value itself
		///// FUNCTIONS /////
		// Accessors
		const string &		funcName() const	{ return mFuncName; }
		const LuaObject &	paramTable() const	{ return mParamTable; }
		const LuaObject &	returnObj() const	{ return mReturnObj; }

		virtual const string & type() const { return sEventType; }

		// Mutators
		virtual void	serialize(ostream &out) const {}
		virtual void	deserialize(istream &in) {}

		void	setReturnObj(const LuaObject &returnObj) { mReturnObj = returnObj; }

		// Constructor / destructor
		explicit LuaFunctionEvent(const string &funcName) :
			mFuncName(funcName)
		{}
		virtual ~LuaFunctionEvent() {}
};