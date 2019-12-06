/*----==== ENGINEEVENTS.H ====----
	Author:		Jeff Kiah
	Orig.Date:	06/25/2009
	Rev.Date:	06/28/2009
--------------------------------*/

#pragma once

#include <string>
#include "Event/Event.h"

using std::string;

///// STRUCTURES /////

/*=============================================================================
class ChangeSettingsEvent
	Triggered from script or code when settings have changed. Event data
	includes the new desired settings. If the desired settings fail to be set,
	the event should be consumed to prevent further processing.
=============================================================================*/
class ChangeSettingsEvent : public ScriptableEvent {
	public:
		///// VARIABLES /////
		// Static
		static const string sEventType;

		// Member
		int		wndResX, wndResY;
		int		fsResX, fsResY;
		int		bpp;
		int		refreshRate;
		bool	fullscreen;
		bool	vsync;

		///// FUNCTIONS /////
		virtual const string & type() const { return sEventType; }
		virtual void serialize(ostream &out) const {}
		virtual void deserialize(istream &in) {}

		/*---------------------------------------------------------------------
			This is called to construct script data out of a code-defined event
		---------------------------------------------------------------------*/
		virtual void buildScriptData();

		// Constructors
		explicit ChangeSettingsEvent(int _wndResX, int _wndResY, int _fsResX, int _fsResY,
									 int _bpp, int _refreshRate, bool _fullscreen, bool _vsync) :
			ScriptableEvent(),
			wndResX(_wndResX), wndResY(_wndResY), fsResX(_fsResX), fsResY(_fsResY),
			bpp(_bpp), refreshRate(_refreshRate), fullscreen(_fullscreen), vsync(_vsync)
		{}
		/*---------------------------------------------------------------------
			The script-called constructor first sets all data equal to their
			current values in Engine::mSettings, so if they aren't all supplied
			by script, the setting will go unchanged.
		---------------------------------------------------------------------*/
		explicit ChangeSettingsEvent(const AnyVars &eventData);

		// Destructor
		virtual ~ChangeSettingsEvent() {}
};