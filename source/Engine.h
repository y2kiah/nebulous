/*----==== ENGINE.H ====----
	Author:		Jeff Kiah
	Orig.Date:	06/21/2009
	Rev.Date:	03/03/2010
--------------------------*/

#pragma once

#include <bitset>
#include "Utility/Singleton.h"
#include "Utility/Typedefs.h"
#include "EngineEvents.h"
#include "Settings.h"
#include "Render/RenderManager_D3D9.h"
#include "HighPerfTimer.h"
#include "Event/EventListener.h"

using std::bitset;

///// DEFINES /////

#define engine		Engine::instance()	// used to access instance globally

///// STRUCTURES /////

//class FastMath;
class EventManager;
class ProcessManager;
class ResCacheManager;
class ScriptManager_Lua;
class EngineEventListener;
class PhysicsScene;

class Camera_D3D9; // TEMP
class Mesh_D3D9; // TEMP

/*=============================================================================
class Engine
=============================================================================*/
class Engine : public Singleton<Engine> {
	friend class Win32;					// allows access to mSettings
	friend class EngineEventListener;	// allows access to mSettings
	friend class ChangeSettingsEvent;	// allows access to mSettings
	private:
		///// VARIABLES /////
		HighPerfTimer *			renderTimer;
		HighPerfTimer *			updateTimer;
		Settings				mSettings;
		EventManager *			mEventMgr;
		ProcessManager *		mProcMgr;
		ResCacheManager *		mResCacheMgr;
		ScriptManager_Lua *		mLuaMgr;
		EngineEventListener *	mEventListener;
		RenderManager_D3D9 *	mRenderMgr;
		PhysicsScene *			mPhysicsScene;
		
		int		mPausedCount; // pause() increments, unpause() decrements, always >= 0, unpaused when 0
		bool	mDeviceLost;
		bool	mExit;

		Camera_D3D9 *	activeCam; // TEMP
		Mesh_D3D9 *		testMesh; // TEMP

		// initialization flags
		enum EngInitFlags {
			INIT_ENGINE = 0,
			INIT_RNDR_DEVICE,
			INIT_MAX
		};
		bitset<INIT_MAX>	mInitFlags;

	public:
		///// FUNCTIONS /////
		// Accessors
		bool				isPaused() const		{ return (mPausedCount > 0); }
		bool				isDeviceLost() const	{ return mDeviceLost; }
		bool				isExiting() const		{ return mExit; }

		// Mutators
		void	exit()		{ mExit = true; }
		int		pause()		{ return ++mPausedCount; }
		int		unpause()	{ return (mPausedCount > 0 ? --mPausedCount : 0); }
		void	processFrame();
		void	update(float deltaMillis);
		void	render();
		void	handleLostDevice();
		void	resetAfterInactive();

		bool	initEngine();
		bool	initRenderDevice();
		void	deInitEngine();

		// Constructor / Destructor

		explicit Engine();
		~Engine();
};

/*=============================================================================
class EngineEventListener
=============================================================================*/
class EngineEventListener : public EventListener {
	private:
		/*---------------------------------------------------------------------
			Handles the change setting event (usually triggered from script) by
			changing the Engine::mSettings members. This event will propagate
			to any other listeners that may need to take action like the
			render or sound managers.
		---------------------------------------------------------------------*/
		bool handleChangeSettingsEvent(const EventPtr &ePtr);

	public:
		explicit EngineEventListener();
		virtual ~EngineEventListener() {}
};