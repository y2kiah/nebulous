/*----==== ENGINE.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	06/21/2009
	Rev.Date:	07/08/2009
----------------------------*/

#include "Engine.h"
#include "Win32.h"
#include "HighPerfTimer.h"
#include "Event/EventManager.h"
#include "Event/RegisteredEvents.h"
#include "Process/ProcessManager.h"
#include "Resource/ResCache.h"
#include "Scripting/ScriptManager_Lua.h"
#include "Render/RenderManager_D3D9.h"
#include "Render/Camera_D3D9.h"
#include "Physics/Physics.h"

// testing
#include "Math/NoiseWrapper.h"
#include "Render/Texture_D3D9.h"
#include "Render/Effect_D3D9.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/TVector4.h"
#include "Utility/Quadtree.h"
#include "Scripting/ScriptingEvents.h"
#include "Resource/ZipFile.h"
#include "Resource/ResHandle.h"
#include "Render/Texture_D3D9.h"
#include "Render/Font_TTF.h"
#include "UI/UISkin.h"
#include "Render/RenderObject_D3D9.h"

#include "Physics/FlightModel.h"
#include "Physics/Airfoil.h"
#include "EngineEvents.h"

///// STRUCTURES /////

// test process
class TestEngineProcess : public CProcess {
	private:
		float		mMillis;
		float		mTotalMillis;
		ResPtr		mTexture;
		ResPtr		mEffectFile;
		bool		mTextureError;
		bool		mEffectError;

	public:
		virtual void onUpdate(float deltaMillis) {
			mMillis += deltaMillis;
			while (mMillis >= 1000) { // every second, print to the console
				debugPrintf("%s: millisecond count %0.0f\n", name().c_str(), mTotalMillis);
				mMillis -= 1000.0f;

				// don't have the resource yet, so try to get it
				if (!mTexture.get() && !mTextureError) {
					ResHandle h;
					int retVal = h.tryLoad<Texture_D3D9>("textures/dirtnew.dds");
					if (retVal == ResLoadResult_Success) {
						mTexture = h.mResPtr;
						Texture_D3D9 *tex = static_cast<Texture_D3D9*>(mTexture.get());
					} else if (retVal == ResLoadResult_Error) {
						mTextureError = true;
						debugPrintf("ERROR loading dirtnew.dds\n");
					}
				}

				// test loading effect file
				if (!mEffectFile.get() && !mEffectError) {
					ResHandle h;
					ResLoadResult r = h.tryLoad<Effect_D3D9>("effects/phong.fxo");
					if (r == ResLoadResult_Success) {
						mEffectFile = h.mResPtr;
						Effect_D3D9 *pEff = static_cast<Effect_D3D9*>(mEffectFile.get());
						debugPrintf("Loaded fx file!\n\n");
					} else if (r == ResLoadResult_Error) {
						mEffectError = true;
						debugPrintf("ERROR loading fx file\n");
					}
				}

			}
			mTotalMillis += deltaMillis;
			if (mTotalMillis >= 4000.0f) finish(); // run for 4 seconds
		}
		virtual void onInitialize() {
			debugPrintf("%s: process initialized\n", name().c_str());
		}
		virtual void onFinish() {
			// get some info on effect file renderable
			Effect_D3D9 *pEff = static_cast<Effect_D3D9*>(mEffectFile.get());
			if (pEff) {
				debugPrintf("EFFECT FILE RENDERABLE: %i\n", pEff->isReadyForRender());
			}

			///// TEST FONT_TTF /////
/*			Font_TTF font("Courier New", Font_TTF::Wt_Normal, false, false, false, false);
			uint texSize = 4096;
			uint gap = 0;
			int pointSize = font.predictFontSize(texSize, gap);
			font.loadTrueTypeFont(pointSize);
			font.renderToBitmap(texSize, gap);
			debugPrintf("Font rendered %ix%i at %i pt gap %i\n", texSize, texSize, pointSize, gap);
*/			/////////////////////////

			///// TEST UISKIN /////
			ResHandle hUISkin;
			if (!hUISkin.load<UI::UISkin>("ui/test.xml")) {
				// handle error
			}
			UI::UISkin *pSkin = static_cast<UI::UISkin*>(hUISkin.getResPtr().get());
			///////////////////////

			///// TEST EVENT SYSTEM /////

			// test event trigger handlers
			events.registerEventType("EVENT_SOMETHING", RegEventPtr(new ScriptCallableCodeEvent<EmptyEvent>(EventDataType_Empty)));
			//events.registerEventType("EVENT_SOMETHING", RegEventPtr(new ScriptCallableCodeEvent<EmptyEvent>(EventDataType_Empty)));
			events.trigger("EVENT_SOMETHING");

			//events.trigger("EVENT_SPECIFIC");
			//EventPtr ePtr(new LuaFunctionEvent());
			//events.trigger(ePtr);
			// test event queue
			//events.raise("EVENT_SOMETHING");
			//events.notifyQueued(0);
			// get debug output
			//delete mEventMgr;
			//mEventMgr = new EventManager();

			/////////////////////////////


			debugPrintf("%s: process finished\n", name().c_str());
		}
		virtual void onTogglePause() {
			if (isPaused()) {
				debugPrintf("%s: process paused\n", name().c_str());
			} else {
				debugPrintf("%s: process unpaused\n", name().c_str());
			}
		}

		explicit TestEngineProcess(const string &name) :
			CProcess(name, CProcess_Run_CanDelay, CProcess_Queue_Multiple),
			mMillis(0), mTotalMillis(0), mTexture(), mEffectFile(), mTextureError(false), mEffectError(false)
		{}
		virtual ~TestEngineProcess() {}
};

///// END TEMP

///// FUNCTIONS /////

void Engine::processFrame()
{
	float deltaMS = updateTimer->stop();
	updateTimer->start();
	if (!isPaused()) {
		update(deltaMS);
	}

	// if screen refresh time has passed, render the output
	if (mDeviceLost) {
		// handle device lost situation
		handleLostDevice();
	} else {
		// render the scene
		renderTimer->stop();
		if (renderTimer->millisecondsPassed() >= 16.66667f) { // should use a supplied refresh rate for this
			renderTimer->start();
			render();
		}
	}
}

void Engine::update(float deltaMillis)
{
	mEventMgr->notifyQueued(0);
	mProcMgr->updateProcesses(deltaMillis);
}

void Engine::render()
{
	if (isDeviceLost()) { return; }

	mRenderMgr->prepareSubmitList();

	// temp - use camera in scene graph, update will set the orientation, so just grab it here instead of setting it
	activeCam->setViewTranslationYawPitchRoll(Vector3f(0.0f, 0.0f, -12000000.0f), Vector3f(0.0f, 0.0f, 0.0f));

	if (!mRenderMgr->render(*activeCam)) {
		pause();
		mDeviceLost = true;
	}
	
	mRenderMgr->resetSubmitList();
}

void Engine::handleLostDevice()
{
	if (mRenderMgr->handleLostDevice()) {
		mDeviceLost = false;
		unpause();
		debugPrintf("DEVICE RESET!\n");
	}
}

void Engine::resetAfterInactive()
{
	// reset frame times so there isn't a big jump
	renderTimer->start();
	updateTimer->start();
}

/*---------------------------------------------------------------------
	initEngine handles all initialization done before window creation
	(debug console will already be present).
---------------------------------------------------------------------*/
bool Engine::initEngine()
{
	// create Timers
	renderTimer = new HighPerfTimer();
	renderTimer->start();
	updateTimer = new HighPerfTimer();
	updateTimer->start();
	// create Event System
	mEventMgr = new EventManager();
	// create Engine Event Listener
	mEventListener = new EngineEventListener();
	// create ProcessManager
	mProcMgr = new ProcessManager();
	// create Resource Cache Manager
	// TEMP these hard-coded values should come from real-time memory queries
	mResCacheMgr = new ResCacheManager(2048, 512);
	// create Physics Scene
	mPhysicsScene = new PhysicsScene();
	// create Lua Scripting System
	mLuaMgr = new ScriptManager_Lua();

	// register engine events
	mEventMgr->registerEventType(ChangeSettingsEvent::sEventType,
		RegEventPtr(new ScriptCallableCodeEvent<ChangeSettingsEvent>(EventDataType_NotEmpty)));

	// set the init flag
	mInitFlags[INIT_ENGINE] = true;
	// init.lua configures the startup settings
	return mLuaMgr->init("data/script/init.lua");
}

bool Engine::initRenderDevice()
{
	// create RenderManager
	mRenderMgr = new RenderManager_D3D9();
	// TEMP, camera should be in scene graph, and set up from script
	activeCam  = new Camera_D3D9(Vector3f(0.0f, 0.0f, 0.0f),
								 Vector3f(0.0f, 0.0f, 0.0f),
								 60.0f * DEGTORADf,
								 (float)mSettings.resXSet() / (float)mSettings.resYSet(),
								 0.1f, 100000.0f, 1.0f);

	// initialize Direct3D
	if (!mRenderMgr->initRenderDevice(	mSettings.bpp,
										mSettings.resXSet(),
										mSettings.resYSet(),
										mSettings.refreshRate,
										mSettings.fullscreen,
										mSettings.vsync))
	{
		// handle error message
		return false;
	}

///// TEST CPROCESS /////
	CProcessPtr teProcPtr(new TestEngineProcess("TestEngineProcess"));
//	CProcessPtr ttProcPtr(new TestThreadProcess("TestThreadProcess"));
//	teProcPtr->setNextProcess(ttProcPtr);
	mProcMgr->attach(teProcPtr);

/////////////////////////

///// TEST RESOURCE MANAGER /////
	ResSourcePtr srcPtr(new ZipFile(L"data/textures.zip"));
	if (srcPtr->open()) {
		mResCacheMgr->registerSource("textures", srcPtr);
	} else {
		debugPrintf("ERROR OPENING ZIP\n");
	}

	ResSourcePtr srcPtr2(new ZipFile(L"data/effects.zip"));
	if (srcPtr2->open()) {
		mResCacheMgr->registerSource("effects", srcPtr2);
	} else {
		debugPrintf("ERROR OPENING ZIP 2\n");
	}

	ResSourcePtr srcPtr3(new ZipFile(L"data/ui.zip"));
	if (srcPtr3->open()) {
		mResCacheMgr->registerSource("ui", srcPtr3);
	} else {
		debugPrintf("ERROR OPENING ZIP 3\n");
	}

/////////////////////////////////

///// TEST Mesh /////
	testMesh = new Mesh_D3D9;
	testMesh->loadFromXFile("data/model/palmtree.x");
///////////////////////

///// TEST NOISEWRAPPER /////
/*	HighPerfTimer *codeTimer = new HighPerfTimer();
	NoiseWrapper::setCoherentNoiseType(NOISE_TYPE_TEST);	
NoiseWrapper::initGrad4();
	const int texSizeX = 256;
	const int texSizeY = 256;
	const float length = 16.0f;
	const int numChannels = 3;

	uchar *buffer = new uchar[texSizeY*texSizeX*numChannels];
	
	const float xStep = length / static_cast<float>(texSizeX);
	const float yStep = length / static_cast<float>(texSizeY);

	codeTimer->start();
	float cx = 0, cy = 0;
	for (int y = 0; y < texSizeY; ++y) {
		for (int x = 0; x < texSizeX; ++x) {
			NoiseWrapper::setMultiFractalOperation(NOISE_MFO_ADD);
			//float nResult = NoiseWrapper::multiFractal(2, Vector2f(cx,cy).v, 1, 2.0f, 0.3f, 0.6f);
			float nResult = NoiseWrapper::fBm(2, Vector2f(cx,cy).v, 8, 2.0f, 0.5f, 1.0f);
			nResult += 1.0f; nResult *= 0.5f; // normalize to [0,1] range
			nResult *= 0.7f; nResult += 0.2f;
//			NoiseWrapper::setMultiFractalOperation(NOISE_MFO_ADD_ABS);
//			float nResult2 = NoiseWrapper::fBm(2, Vector2f(cx,cy).v, 18, 2.0f, 0.5f, 0.6f);
//			nResult2 *= nResult2;

//			nResult *= nResult2;		
			
			int iResult = math.f2iRound(nResult * 255);  // convert to int
			uchar ucResult = (iResult > 255) ? 255 : ((iResult < 0) ? 0 : iResult); // convert to uchar 

			int index = (y*texSizeX*numChannels) + (x*numChannels);
			for (int c = 0; c < numChannels; ++c) buffer[index+c] = ucResult;

			cx += xStep;
		}
		cx = 0;
		cy += yStep;
	}	
	codeTimer->stop();
	debugPrintf("ms to generate = %Lf\n", (double)codeTimer->secondsPassed() * 1000.0);
	
	ofstream fOut;
	fOut.open("time.txt", std::ios::out | std::ios::trunc);
	fOut << (double)codeTimer->secondsPassed() * 1000.0;
	fOut.close();

	Texture_D3D9 *d3dTexture = new Texture_D3D9();
	d3dTexture->createTexture(buffer, numChannels, texSizeX, texSizeY, TEX_FMT_UCHAR, TEX_USAGE_DEFAULT,
		TEX_RPT_WRAP, TEX_FILTER_DEFAULT, false, false, L"fBm");

	if (d3dTexture->isInitialized()) d3dTexture->saveTextureToFile("cached_quintic_8_oct.jpg");

	delete d3dTexture;
*/	
	// convert to single channel
/*	uchar *buffer2 = new uchar[texSizeY*texSizeX];
	int op = 0;
	for (int p = 0; p < texSizeY*texSizeX; ++p) {
		buffer2[p] = buffer[op];
		op += numChannels;
	}
	ofstream fOut;
	fOut.open("map_02.raw",std::ios::out | std::ios::trunc | std::ios::binary);
	fOut.write((const char *)buffer2, sizeof(uchar) * texSizeY * texSizeX);
	fOut.close();
	
	delete [] buffer2;*/
//	delete [] buffer;
//	delete codeTimer;

/////////////////////////////

//// TEST QUADTREE TRAVERSAL ////
/*	HighPerfTimer *codeTimer = new HighPerfTimer();
	Quadtree32 qt(4);

	codeTimer->start();
	qt.traverse(0, 4, 0, 0);
	
	codeTimer->stop();
	
	ulong numNodes = 0;
	for (int l = 0; l <= 4; ++l) numNodes += ((1<<l)*(1<<l));
	debugPrintf("sec.=%Lf   numNodes=%lu   ", (double)codeTimer->secondsPassed(), numNodes);
	debugPrintf("sec./node= %0.14Lf\n", (double)codeTimer->secondsPassed() / (double)numNodes);

	delete codeTimer;*/
/////////////////////////////////

//// TEST FLIGHT MODEL ////
//	float span = 20; //36.0833f;
//	float Croot = 8.92f;// /*5.645f;*/ 4.822f; //0.1f;
//	float Ctip = 3.92f;// /*4.0f;*/ 4.822f; //0.1f;
/*	float mach = 0.0f;
	float leSweep = 50.0f * DEGTORADf;
	float S = FlightModel::calcPlanformArea(Croot, Ctip, span);
	float AR = FlightModel::calcAspectRatio(span, S);
	float B = FlightModel::calcCompressibilityCorrection(mach);
	float sweep = atanf(FlightModel::calcTanHalfChordSweep(leSweep, Croot, Ctip, span));
	float slope = FlightModel::calcCLalpha(span, Croot, Ctip, leSweep, mach, TWO_PIf);
	debugPrintf("S = %0.5f\n", S);
	debugPrintf("AR = %0.5f\n", AR);
	debugPrintf("B = %0.5f\n", B);
	debugPrintf("c/2 sweep = %0.5f\n", sweep * RADTODEGf);
	debugPrintf("slope = %0.5f\n", slope / RADTODEGf);
*/
//	for (float m = 0.0f; m < 3.05f; m += 0.1f) {
//		slope = FlightModel::calcCLalpha(span, Croot, Ctip, leSweep, m, TWO_PIf);
//		debugPrintf("mach = %0.1f   slope = %0.5f\n", m, slope/* / RADTODEGf*/);
//	}

//// TEST AIRFOIL ////
/*	Airfoil af;
	af.mCLIntercept = 0.25f;
	af.mCLSlope = TWO_PIf;
	for (float a = 0.0f; a < 20.05f; a += 0.1f) {
		float cl = af.flowSeparationCurve(a * DEGTORADf);
	//	debugPrintf("alpha = %0.1f   cl = %0.5f\n", a, cl);
	}
*/	

/////////////////////////////////
	// set the init flag
	mInitFlags[INIT_RNDR_DEVICE] = true;
	return true;
}

void Engine::deInitEngine()
{
	if (mInitFlags[INIT_RNDR_DEVICE]) {
		delete activeCam;// TEMP
		delete testMesh; // TEMP
		// shutdown all processes - do this early incase any processes hold Resources
		mProcMgr->clear();
		delete mRenderMgr;
	}
	if (mInitFlags[INIT_ENGINE]) {
		delete mLuaMgr;
		delete mPhysicsScene;
		delete mResCacheMgr;
		delete mProcMgr;
		delete mEventListener;
		delete mEventMgr;
		delete updateTimer;
		delete renderTimer;
	}
}

Engine::Engine() :
	Singleton<Engine>(*this),
	mPausedCount(0),
	mDeviceLost(false),
	mExit(false),
	mInitFlags(0)
{}

Engine::~Engine()
{
	deInitEngine();
}

////////// class EngineEventListener //////////

/*---------------------------------------------------------------------
	Handles the change setting event (usually triggered from script) by
	changing the Engine::mSettings members. This event will propagate
	to any other listeners that may need to take action like the
	render or sound managers.
---------------------------------------------------------------------*/
bool EngineEventListener::handleChangeSettingsEvent(const EventPtr &ePtr) {
	ChangeSettingsEvent &e = *(static_cast<ChangeSettingsEvent*>(ePtr.get()));
	engine.mSettings.resX = e.wndResX;
	engine.mSettings.resY = e.wndResY;
	engine.mSettings.fsResX = e.fsResX;
	engine.mSettings.fsResY = e.fsResY;
	engine.mSettings.bpp = e.bpp;
	engine.mSettings.refreshRate = e.refreshRate;
	engine.mSettings.fullscreen = e.fullscreen;
	engine.mSettings.vsync = e.vsync;
	debugPrintf("%s: %i %i %i %i %i %i %i %i\n", name().c_str(), e.wndResX, e.wndResY, e.fsResX,
		e.fsResY, e.bpp, e.refreshRate, e.fullscreen, e.vsync);
	return false; // allow event to propagate
}

EngineEventListener::EngineEventListener() :
	EventListener("EngineEventListener")
{
	IEventHandlerPtr p(new EventHandler<EngineEventListener>(this, &EngineEventListener::handleChangeSettingsEvent));
	registerEventHandler(ChangeSettingsEvent::sEventType, p, 1);
}
