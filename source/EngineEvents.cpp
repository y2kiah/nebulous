/*----==== ENGINEEVENTS.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	06/28/2009
	Rev.Date:	06/28/2009
----------------------------------*/

#include "EngineEvents.h"
#include "Engine.h"

////////// class ChangeSettingsEvent //////////

const string ChangeSettingsEvent::sEventType("SYS_CHANGE_SETTINGS");

/*---------------------------------------------------------------------
	This is called to construct script data out of a code-defined event
---------------------------------------------------------------------*/
void ChangeSettingsEvent::buildScriptData()
{
	mScriptData.clear();
	mScriptData.push_back(AnyVarsValue("wndResX", wndResX));
	mScriptData.push_back(AnyVarsValue("wndResY", wndResY));
	mScriptData.push_back(AnyVarsValue("fsResX", fsResX));
	mScriptData.push_back(AnyVarsValue("fsResY", fsResY));
	mScriptData.push_back(AnyVarsValue("bpp", bpp));
	mScriptData.push_back(AnyVarsValue("refreshRate", refreshRate));
	mScriptData.push_back(AnyVarsValue("fullscreen", fullscreen));
	mScriptData.push_back(AnyVarsValue("vsync", vsync));
	mScriptDataBuilt = true;
}

/*---------------------------------------------------------------------
	The script-called constructor first sets all data equal to their
	current values in Engine::mSettings, so if they aren't all supplied
	by script, the setting will go unchanged.
---------------------------------------------------------------------*/
ChangeSettingsEvent::ChangeSettingsEvent(const AnyVars &eventData) :
	ScriptableEvent(eventData),
	wndResX(engine.mSettings.resX), wndResY(engine.mSettings.resY),
	fsResX(engine.mSettings.fsResX), fsResY(engine.mSettings.fsResY),
	bpp(engine.mSettings.bpp), refreshRate(engine.mSettings.refreshRate),
	fullscreen(engine.mSettings.fullscreen), vsync(engine.mSettings.vsync)
{
	for (AnyVars::const_iterator i = eventData.begin(); i != eventData.end(); ++i) {
		try {
			if (_stricmp(i->first.c_str(), "wndResX") == 0) {
				wndResX = any_cast<int>(i->second);
			} else if (_stricmp(i->first.c_str(), "wndResY") == 0) {
				wndResY = any_cast<int>(i->second);
			} else if (_stricmp(i->first.c_str(), "fsResX") == 0) {
				fsResX = any_cast<int>(i->second);
			} else if (_stricmp(i->first.c_str(), "fsResY") == 0) {
				fsResY = any_cast<int>(i->second);
			} else if (_stricmp(i->first.c_str(), "bpp") == 0) {
				bpp = any_cast<int>(i->second);
			} else if (_stricmp(i->first.c_str(), "refreshRate") == 0) {
				refreshRate = any_cast<int>(i->second);
			} else if (_stricmp(i->first.c_str(), "fullscreen") == 0) {
				fullscreen = any_cast<bool>(i->second);
			} else if (_stricmp(i->first.c_str(), "vsync") == 0) {
				vsync = any_cast<bool>(i->second);
			}
		} catch (const boost::bad_any_cast &ex) {
			// nothing happens with a bad datatype in release build, silently ignores
			debugPrintf("DeviceChangeEvent: bad_any_cast \"%s\"\n", ex.what());
		}
	}
}
