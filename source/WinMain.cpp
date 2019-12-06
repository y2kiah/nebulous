/*----==== WINMAIN.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	07/21/2007
	Rev.Date:	06/25/2009
-----------------------------*/

#include "Win32.h"
#include <crtdbg.h>
#include <cstdlib>
#include <boost/scoped_ptr.hpp>

#include "Engine.h"
#include "Settings.h"
#include "HighPerfTimer.h"

using boost::scoped_ptr;

///// FUNCTIONS /////

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(0);

	Win32 win32Inst(TEXT("Nebulous Engine"), hInstance);
	if (!win32Inst.isOnlyInstance()) { return 0; }	// allow only a single instance

	#ifdef _DEBUG
		// Set up debug heap checking	
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif
	#ifdef DEBUG_CONSOLE
		// Open a console and redirect stdout to it
		win32Inst.initConsole();
	#endif
	
	if (!win32Inst.getSystemInformation()) { return 0; }	// some requirement not met

	// init Timer
	if (!HighPerfTimer::initHighPerfTimer()) {
		win32Inst.showErrorBox(TEXT("Failed to initialize high-performance counter"));
		return 0;
	}

	scoped_ptr<Engine> engineInst(new Engine);

	if (!engineInst->initEngine()) {
		win32Inst.showErrorBox(TEXT("Failed to initialize engine"));
		return 0;
	}

	if (!win32Inst.initWindow()) { return 0; }

	if (!win32Inst.initInputDevices()) { return 0; }

	if (!engineInst->initRenderDevice()) {
		win32Inst.showErrorBox(TEXT("Failed to initialize render device"));
		return 0;
	}

	MSG msg;
	SecureZeroMemory(&msg, sizeof(msg));

	for (;;) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
		} else if (win32Inst.isActive()) {
			if (engine.isExiting()) { PostMessage(win32Inst.hWnd, WM_CLOSE, 0, 0); }

			engine.processFrame();

		} else {
			WaitMessage();	// avoid 100% CPU when inactive
		}		
	}
	
	// Check for corruption in the heap, generate debug report	
	_ASSERTE(_CrtCheckMemory());

	return static_cast<int>(msg.wParam);
}