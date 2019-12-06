/*----==== WIN32.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	12/01/2007
	Rev.Date:	07/09/2009
---------------------------*/

#include "Win32.h"
#include "hidusage.h"
#include <shlobj.h>
#include <sstream>
#include <tchar.h>
#include "Engine.h"
#include "Event/EventManager.h"

///// DEFINITIONS /////

#define CONSOLE_X	1700	// default location of the debug console window
#define CONSOLE_Y	20

///// FUNCTIONS /////

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return Win32::instance().wndProc(hwnd, msg, wParam, lParam);
}

////////// class Win32 //////////

bool Win32::isOnlyInstance()
{
	// use a random GUID for the named mutex of this app
	hMutex = CreateMutex(NULL, TRUE, TEXT("18E205E0-59CC-11DE-AA00-82C055D89593"));
	if (GetLastError() != ERROR_SUCCESS) {
		HWND hwnd = FindWindow(className, appName);
		if (hwnd) {
			ShowWindow(hwnd, SW_SHOWNORMAL);
			SetFocus(hwnd);
			SetForegroundWindow(hwnd);
			SetActiveWindow(hwnd);
			return false;
		}
	}
	return true;
}

void Win32::showErrorBox(const TCHAR *message)
{
	std::wostringstream oss;
	oss << message << TEXT(" (error ") << GetLastError() << TEXT(")");
	MessageBox((hWnd?hWnd:NULL), oss.str().c_str(), TEXT("Error"), MB_OK | MB_ICONERROR | MB_TOPMOST);
}

void Win32::setActive(bool active)
{
	mActive = active;
	if (active) engine.resetAfterInactive();
}

void Win32::takeCursor()
{
	if (mAppHasMouse) return;
	while (ShowCursor(FALSE) >= 0) {} // guarantee that the cursor will hide
	// clip the cursor to the window's client area
	POINT ulPoint = {0,0};
	ClientToScreen(hWnd, &ulPoint);
	int resX = engine.mSettings.resXSet();
	int resY = engine.mSettings.resYSet();
	// clip it to the window
	RECT clipRect = {ulPoint.x, ulPoint.y, ulPoint.x + resX, ulPoint.y + resY };
	ClipCursor(&clipRect);
	mAppHasMouse = true;
	debugPrintf("TAKE CURSOR!\n"); // TEMP
}

void Win32::giveCursor()
{
	if (!mAppHasMouse) return;
	ClipCursor(NULL);	// unclip the cursor
	while (ShowCursor(TRUE) < 0) {} // guarantee that the cursor will show
	mAppHasMouse = false;
	debugPrintf("GIVE CURSOR!\n"); // TEMP
}

bool Win32::initWindow()
{
	WNDCLASSEX	wc;
	DWORD		dwExStyle;
	DWORD		dwStyle;
	
	// create WindowClass
    wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInst;
	wc.hIcon			= LoadIcon(0, IDI_APPLICATION);
	wc.hIconSm			= LoadIcon(0, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(0, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= className;    

	if (!RegisterClassEx(&wc)) {
		showErrorBox(TEXT("Failed to register the window class"));
		return false;
	}
	mInitFlags[INIT_WNDCLASS] = true; // set the init flag

	// set windowed mode settings
	dwExStyle = WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW;
	dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	// find the window RECT size based on desired client area resolution
	RECT wRect = {0,0,engine.mSettings.resXSet(),engine.mSettings.resYSet()};
	if (!getWindowSize(wRect, dwStyle, dwExStyle)) {
		showErrorBox(TEXT("Window sizing error"));
		return false;
	}
	int wWidth = wRect.right;
	int wHeight = wRect.bottom;

	// create new window
	hWnd = CreateWindowEx(	dwExStyle,
							className,
							appName,
							dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							wWidth,
							wHeight,
							0,
							0,
							hInst,
							0);
	if (!hWnd) {
		showErrorBox(TEXT("Window creation error"));
		return false;
	}
	mInitFlags[INIT_WINDOW] = true; // set the init flag

	takeCursor();
	ShowWindow(hWnd, SW_SHOWNORMAL);
	SetFocus(hWnd);
	
	return true;
}

void Win32::killWindow()
{
	giveCursor();
	
	if (hWnd && mInitFlags[INIT_WINDOW] && !mInitFlags[DEINIT_WINDOW]) {
		if (!DestroyWindow(hWnd)) {
			showErrorBox(TEXT("Could not release hWnd"));
		}
		hWnd = 0;
		mInitFlags[DEINIT_WINDOW] = true; // set the deinit flag
	}

	if (hInst && mInitFlags[INIT_WNDCLASS] && !mInitFlags[DEINIT_WNDCLASS]) {
		if (!UnregisterClass(className, hInst)) {
			showErrorBox(TEXT("Could not unregister class"));
		}
		hInst = 0;
		mInitFlags[DEINIT_WNDCLASS] = true; // set the deinit flag
	}
}

bool Win32::getWindowSize(RECT &outRect, DWORD dwStyle, DWORD dwExStyle) const
{
	// find the window RECT size based on desired client area resolution
	if (!AdjustWindowRectEx(&outRect, dwStyle, FALSE, dwExStyle)) {
		return false;
	}
	// pass back the size in right, bottom
	outRect.right = outRect.right - outRect.left;
	outRect.bottom = outRect.bottom - outRect.top;
	outRect.left = outRect.top = 0;
	debugPrintf("Window width=%i  height=%i\n", outRect.right, outRect.bottom);
	return true;
}

bool Win32::checkFullScreenSettings(DISPLAY_DEVICE &dispDevice, DEVMODE &dmScreenSettings)
{
	dispDevice.cb = sizeof(DISPLAY_DEVICE);
	TCHAR *deviceNamePtr = 0;

	dmScreenSettings.dmSize	= sizeof(DEVMODE);
	dmScreenSettings.dmDriverExtra = 0; // if primary display driver cannot be found, null is used
		
	// Choose the primary display driver
	int d = 0;
	for (;;) {			
		if (EnumDisplayDevices(0, d, &dispDevice, 0) == 0) break;

		if ((dispDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
			(dispDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)) {
			deviceNamePtr = dispDevice.DeviceName;
			debugPrintf("Display driver \"%S\" chosen\n", dispDevice.DeviceString);
			break;
		}
		++d;
	}

	// Choose the closest matching display settings (the first to match
	// on all 4 criteria will exit early)
	int mostMatches = 0, c = 0;
	for (;;) {
		DEVMODE dmEnum;
		dmEnum.dmSize = sizeof(DEVMODE);
		dmEnum.dmDriverExtra = 0;

		if (EnumDisplaySettings(deviceNamePtr, c, &dmEnum) == 0) break;

		int matches = 0;
		if (dmEnum.dmBitsPerPel == engine.mSettings.bpp)    { ++matches; }
		if (dmEnum.dmPelsWidth  == engine.mSettings.fsResX) { ++matches; }
		if (dmEnum.dmPelsHeight == engine.mSettings.fsResY) { ++matches; }
		if (dmEnum.dmDisplayFrequency == engine.mSettings.refreshRate) { ++matches; }

		if (matches > mostMatches) {
			mostMatches = matches;
			memcpy_s(&dmScreenSettings, sizeof(DEVMODE), &dmEnum, sizeof(DEVMODE));
		}
		if (matches == 4) break;

		++c;
	}		

	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFLAGS | DM_DISPLAYFREQUENCY;

	LONG check = ChangeDisplaySettings(&dmScreenSettings, CDS_TEST);
		
	switch (check) {
		case DISP_CHANGE_SUCCESSFUL:
			return true;
			break;
		case DISP_CHANGE_FAILED:
			showErrorBox(TEXT("Failed to set fullscreen mode"));
			break;
		case DISP_CHANGE_BADMODE:
			showErrorBox(TEXT("Fullscreen video mode not supported"));
			break;
		case DISP_CHANGE_RESTART:
			showErrorBox(TEXT("Must restart to get fullscreen video mode"));
			break;
		default:
			showErrorBox(TEXT("Failed to set fullscreen mode"));
	}
	return false;
}

bool Win32::setFullScreen()
{
	DISPLAY_DEVICE dispDevice;
	DEVMODE dmScreenSettings;

	bool ok = checkFullScreenSettings(dispDevice, dmScreenSettings);
	if (ok) { // fullscreen mode supported
		// Since DirectX will set fullscreen when initializing the device, we don't actually
		// set it here, only perform the check so we know if the window can initially be set
		// up for fullscreen mode. Fullscreen settings will be altered to the closest match.
		
		//ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		engine.mSettings.set(	engine.mSettings.resX,
								engine.mSettings.resY,
								dmScreenSettings.dmPelsWidth,
								dmScreenSettings.dmPelsHeight,
								dmScreenSettings.dmBitsPerPel,
								dmScreenSettings.dmDisplayFrequency,
								true, true,
								engine.mSettings.vsync);
	}
	return ok;
}

bool Win32::toggleFullScreen()
{
	int width = 0, height = 0;
	int posX = engine.mSettings.wndPosX, posY = engine.mSettings.wndPosY;
	HWND hWndInsertAfter = HWND_NOTOPMOST;
	// get the new desired fullscreen settings
	if (engine.mSettings.toggleFullscreen()) {
		// check that fullscreen settings are ok
		if (setFullScreen()) {
			// fullscreen mode settings
			DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;
			DWORD dwStyle = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
			hWndInsertAfter = HWND_TOPMOST;
			SetWindowLongPtr(hWnd, GWL_STYLE, dwStyle);
			SetWindowLongPtr(hWnd, GWL_EXSTYLE, dwExStyle);
			width = engine.mSettings.resXSet();
			height = engine.mSettings.resYSet();
			posX = posY = 0;
		} else {
			debugPrintf("Could not toggle fullscreen!");
			return false;
		}
	// get the desired windowed settings
	} else {
		// windowed mode settings
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW;
		DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX |
						WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
		SetWindowLongPtr(hWnd, GWL_STYLE, dwStyle);
		SetWindowLongPtr(hWnd, GWL_EXSTYLE, dwExStyle);
		engine.mSettings.fullscreenSet = false;
		RECT wRect = {0,0,engine.mSettings.resXSet(),engine.mSettings.resYSet()};
		getWindowSize(wRect, dwStyle, dwExStyle);
		width = wRect.right;
		height = wRect.bottom;
	}
	giveCursor();
	// commit the changes to the window
	if (!SetWindowPos(hWnd, hWndInsertAfter,
					  posX, posY,
					  width, height,
					  SWP_FRAMECHANGED | SWP_SHOWWINDOW))
	{
		debugPrintf("SetWindowPos failed!\n");
		return false;
	}
	if (!engine.mSettings.fullscreenSet) {
		ShowWindow(hWnd, SW_SHOWNORMAL);
	}
	// change the display mode accordingly
	if (!engine.mRenderMgr->makeDisplayChanges(	engine.mSettings.bpp,
												width,
												height,
												engine.mSettings.refreshRate,
												engine.mSettings.fullscreenSet,
												engine.mSettings.vsync))
	{
		return false;
	}
	takeCursor();

	return true;
}

bool Win32::initInputDevices()
{
	// find number of mouse buttons
	int mouseNumBtns = GetSystemMetrics(SM_CMOUSEBUTTONS); // set some persistent value
	if (mouseNumBtns == 0) { // check for mouse existence
		showErrorBox(TEXT("No mouse detected, closing!"));
		return false;
	}
	// find if mouse has a vertical scroll wheel
	bool mouseHasWheel = (GetSystemMetrics(SM_MOUSEWHEELPRESENT) != 0); // set some persistent value

	// register the mouse for HID input
	RAWINPUTDEVICE rid[1];
	rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
	rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
	rid[0].dwFlags = 0; // or RIDEV_INPUTSINK;
	rid[0].hwndTarget = hWnd;
	BOOL result = RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
	if (!result) {
		showErrorBox(TEXT("Failed to register mouse for raw input"));
		return false;
	}

	// get keyboard type information
	int kbType = GetKeyboardType(0); // type (1-7), set some persistent value
	if (!kbType) {
		showErrorBox(TEXT("No keyboard detected, closing!"));
		return false;
	}
	int kbNumFuncKeys = GetKeyboardType(2); // number of function keys, set some persistent value

	debugPrintf("Keyboard type:%i   func.keys:%i\n", kbType, kbNumFuncKeys);
	debugPrintf("Mouse detected: btns:%i   wheel:%i\n", mouseNumBtns, mouseHasWheel);
	return true;
}

bool Win32::getSystemInformation()
{
	// test the Windows OS version (XP or greater to pass)
	DWORD version = GetVersion();
	DWORD majorVersion = LOBYTE(LOWORD(version));
	DWORD minorVersion = HIBYTE(LOWORD(version));
	DWORD build = 0; if (version < 0x80000000) { build = HIWORD(version); }
	debugPrintf("Windows version %d.%d.%d\n", majorVersion, minorVersion, build);
	if (majorVersion < 5 || (majorVersion == 5 && minorVersion < 1)) {
		MessageBox((hWnd?hWnd:NULL), TEXT("This application is not supported on this version of Windows.\nThe minimum required version is 5.1 (Windows XP)."),
			TEXT("Error"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		return false;
	}

	// Get the application data path
	TCHAR appDataPath[MAX_PATH]; // this should be persistent

	// This is for Windows XP - for Vista and 7 this wraps a
	// call to SHGetKnownFolderPath(FOLDERID_ProgramData, ...)
	//if (version < 6) {
	if (SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataPath) != S_OK) {
		MessageBox((hWnd?hWnd:NULL), TEXT("The application data path could not be retrieved!"),
			TEXT("Error"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		return false;
	}
	// For Vista/7 we use SHGetKnownFolderPath instead
	//} else {
	//	PWSTR *ppszPath;
	//	SHGetKnownFolderPath(FOLDERID_ProgramData, KF_FLAG_NO_ALIAS, NULL, ppszPath);
	//	// memcpy ppszPath into appDataPath	
	//	CoTaskMemFree(ppszPath);
	//}
	debugTPrintf("Save path: \"%s\"\n", appDataPath);

	return true;
}

LRESULT Win32::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_CLOSE:
			killWindow(); // calls DestroyWindow
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		// handle deactivate and non mouse-click activate
		case WM_ACTIVATE: {
			WORD activeFlag = LOWORD(wParam);
			WORD minimized = HIWORD(wParam);
			if (activeFlag == WA_INACTIVE || minimized) { // lost focus
				setActive(false);
				giveCursor();

				debugPrintf("LOST FOCUS!\n");

			// gained focus not by mouse click, was not active and not minimized
			} else if (activeFlag == WA_ACTIVE && !isActive() && !minimized) {
				setActive(true);
				takeCursor();

				debugPrintf("GOT FOCUS!\n");
			}
			break;
		}
		// handle mouse-click activate
		case WM_MOUSEACTIVATE: {
			debugPrintf("GOT FOCUS FROM MOUSE\n");
			setActive(true);
			// if the mouse caused activation, if click was in the client area
			// hide/clip the cursor, otherwise leave OS cursor visible
			if (LOWORD(lParam) == HTCLIENT) {
				takeCursor();
				return MA_ACTIVATEANDEAT;
			}
			// if user clicks the title bar or border or buttons, don't take the cursor
			return MA_ACTIVATE;
			break;
		}

		case WM_PAINT:
			// This only gets called when the window is invalidated somehow, like resizing or
			// dragging another window across it. In any case, the app should be inactive at that
			// time so this check is just for good measure, so it's clear that you aren't really
			// calling render twice in one game loop (render is called during app idle processing).
			if (!isActive()) { if (Engine::exists()) engine.render(); }
			return DefWindowProc(hwnd, msg, wParam, lParam);
			break;

		case WM_DISPLAYCHANGE:
			debugPrintf("Display mode changed: resX=%i  resY=%i  bpp=%u\n", LOWORD(lParam), HIWORD(lParam), wParam);
			break;

		case WM_MOVE:
			if (!engine.mSettings.fullscreenSet) {
				engine.mSettings.wndPosX = LOWORD(lParam);	// horizontal position
				engine.mSettings.wndPosY = HIWORD(lParam);	// vertical position
			}
			break;

		case WM_SIZE:
			//SIZE_MINIMIZED
				//Pause rendering
			//SIZE_RESTORED
				//If a SIZE_MINIMIZED previously occurred then un-pause rendering
				//If between a WM_ENTERSIZEMOVE and WM_EXITSIZEMOVE then ignore
				//Otherwise, check for the host window changing size. If the window has changed size reset the device
			//SIZE_MAXIMISED
				//Un-pause rendering
			break;

		case WM_ENTERSIZEMOVE:
			// Pause rendering until the user finishes resizing the target window
			setActive(false);
			debugPrintf("WM_ENTERSIZEMOVE\n");
			break;

		case WM_EXITSIZEMOVE:
			// Un-pause rendering
			// Check for the host window changing size; if the window has changed then reset the device
			setActive(true);
			debugPrintf("WM_EXITSIZEMOVE\n");
			break;

		// Handle Mouse Input
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			break;
		case WM_RBUTTONDOWN:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_MBUTTONDOWN:
			break;
		case WM_MBUTTONUP:
			break;
		case WM_XBUTTONDOWN: {
			int fwButton = GET_XBUTTON_WPARAM(wParam);
			debugPrintf("WM_XBUTTONDOWN %i\n", fwButton);
			break;
		}
		case WM_XBUTTONUP: {
			int fwButton = GET_XBUTTON_WPARAM(wParam);
			debugPrintf("WM_XBUTTONUP %i\n", fwButton);
			break;
		}
		case WM_MOUSEWHEEL: {
			int fwKeys = GET_KEYSTATE_WPARAM(wParam);
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			debugPrintf("WM_MOUSEWHEEL %i\n", zDelta);
			break;
		}
		case WM_MOUSEHWHEEL: {
			int fwKeys = GET_KEYSTATE_WPARAM(wParam);
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			debugPrintf("WM_MOUSEHWHEEL %i\n", zDelta);
			break;
		}

		// WM_MOUSEMOVE is used for cursor control because pointer ballistics apply.
		// It is not used for high-precision first-person camera or similar control.
		case WM_MOUSEMOVE: {
			int posX = LOWORD(lParam);
			int posY = HIWORD(lParam);
			break;
		}

		// WM_INPUT is used for high-precision mouse movements such as camera control,
		// it is not used for cursor control since pointer ballistics do not apply. It
		// can also captures Joystick and other controller raw inputs, but DirectInput
		// may be used for those.
		case WM_INPUT: {
			UINT dwSize = 40;		// This hard-coded size is specific to mouse.
			static BYTE lpb[40];	// Consider using variable size like below.

			/* or...
			UINT dwSize;
			// get the required size of the data buffer
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, 
							sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[dwSize];
			if (!lpb) return 0;
			*/
			
			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize,
								sizeof(RAWINPUTHEADER)) != dwSize)
			{
				debugPrintf("WM_INPUT: GetRawInputData doesn't return the expected size!\n");
			}

			RAWINPUT &raw = *((RAWINPUT*)lpb);

			if (raw.header.dwType == RIM_TYPEMOUSE) {
				// mouse snooper
				/*debugPrintf("Mouse: usFlags=%04x ulButtons=%04x usButtonFlags=%04x usButtonData=%04x ulRawButtons=%04x lLastX=%04x lLastY=%04x ulExtraInformation=%04x\n",
							raw.data.mouse.usFlags,
							raw.data.mouse.ulButtons,
							raw.data.mouse.usButtonFlags,
							raw.data.mouse.usButtonData,
							raw.data.mouse.ulRawButtons,
							raw.data.mouse.lLastX,
							raw.data.mouse.lLastY,
							raw.data.mouse.ulExtraInformation);*/
			}
			break;
		}

		// Handle Keyboard Input
		// Alt key handling
		case WM_SYSKEYDOWN:
			switch (wParam) {
				// handle fullscreen toggle Alt-Enter
				case VK_RETURN:
					//DWORD dwMask = (1 << 29);
                    //if ((lParam & dwMask) != 0) { // Alt is down also
					toggleFullScreen();
					//}
					break;
				// if the alt key is pressed down don't send a WM_SYSCOMMAND message
				case VK_MENU:
					break;
				default:
					return DefWindowProc(hwnd, msg, wParam, lParam);
			}
			break;
		case WM_SYSKEYUP:
			switch (wParam) {
				// if the alt key is upped, in windowed mode toggle the cursor
				case VK_MENU:
					if (!engine.mSettings.fullscreenSet) {
						if (mAppHasMouse && isActive()) {
							giveCursor();
						} else {
							takeCursor();
						}
					}
					break;
				default:
					return DefWindowProc(hwnd, msg, wParam, lParam);
			}
			break;

		// handle other keys
		case WM_CHAR:
		case WM_KEYUP:
		case WM_KEYDOWN: {
			// keyboard snooper
			uint extended	= ((HIWORD(lParam) & KF_EXTENDED) != 0);
			uint altDown	= ((HIWORD(lParam) & KF_ALTDOWN) != 0);
			uint repeat		= ((HIWORD(lParam) & KF_REPEAT) != 0);
			uint up			= ((HIWORD(lParam) & KF_UP) != 0);
			uint repeatCnt	= LOWORD(lParam);
			uint vsc		= LOBYTE(HIWORD(lParam));
			uint vk			= MapVirtualKey(vsc, MAPVK_VSC_TO_VK_EX);
			
			#ifdef DEBUG_CONSOLE
				if		(msg == WM_KEYDOWN)	{ debugPrintf("WM_KEYDOWN "); }
				else if (msg == WM_CHAR)	{ debugPrintf("WM_CHAR    "); }
				else if (msg == WM_KEYUP)	{ debugPrintf("WM_KEYUP   "); }
			#endif
			
			debugPrintf("code=%3u '%c' vk=%3u oem=%2u ext'd=%u altDn=%u rpt=%u rptCnt=%u rel'd=%u\n",
						wParam, (wParam>31&&wParam<128)?wParam:' ', vk, vsc, extended, altDown, repeat, repeatCnt, up);
			
			switch (wParam) {
				case VK_ESCAPE:
					//PostMessage(hWnd,WM_CLOSE,0,0);
					engine.exit();
					break;
			}
			break;
		}

		case WM_SYSCOMMAND:
			switch (wParam & 0xFFF0) {
				// these are ignored and not handled
				case SC_SCREENSAVE:
					// Microsoft Windows Vista and later: If password protection is enabled by policy,
					// the screen saver is started regardless of what an application does with the
					// SC_SCREENSAVE notification even if fails to pass it to DefWindowProc.
				case SC_MONITORPOWER:
				case SC_HSCROLL:
				case SC_VSCROLL:
				case SC_KEYMENU:
				case SC_HOTKEY:
				case SC_CONTEXTHELP:
					break;
				// all others use the default handler
				default:
					return DefWindowProc(hwnd, msg, wParam, lParam);
			}
			break;

		// ignore these messages (return 0)
		case WM_CREATE:
		case WM_APPCOMMAND:
		case WM_ERASEBKGND:
			break;

		// all other messages use default handler
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

bool Win32::initConsole()
{
	BOOL result = AllocConsole();
	if (result) {
		result = SetConsoleCtrlHandler((PHANDLER_ROUTINE)Win32::ctrlHandler, TRUE);
		if (result) {
			FILE *stream;
			freopen_s(&stream, "CONOUT$", "w", stdout);
			printf("Debugging console (Press CTRL-C to close)\n-----------------------------------------\n\n");
		} else {
			showErrorBox(TEXT("Could not set console handler"));
		}
	} else {
		showErrorBox(TEXT("Could not create debug console window"));
	}
	bool success = (result != 0);
	if (success) {
		HWND hWnd_c = GetConsoleWindow();
		SetWindowPos(hWnd_c, 0, CONSOLE_X, CONSOLE_Y,
					 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	return success;
}

BOOL WINAPI Win32::ctrlHandler(DWORD dwCtrlType) 
{ 
	switch (dwCtrlType) { 
		case CTRL_C_EVENT:
			FreeConsole();
			return TRUE;
 
		case CTRL_BREAK_EVENT:
			FreeConsole();
			return TRUE;

		case CTRL_CLOSE_EVENT:
			PostMessage(Win32::instance().hWnd, WM_CLOSE, 0, 0);
			return TRUE;
  
		case CTRL_LOGOFF_EVENT:
			PostMessage(Win32::instance().hWnd, WM_CLOSE, 0, 0);
			return TRUE;
 
		case CTRL_SHUTDOWN_EVENT:
			PostMessage(Win32::instance().hWnd, WM_CLOSE, 0, 0);
			return TRUE;
 
		default:
			return FALSE;
	}
}

// Constructor / destructor
Win32::Win32(const TCHAR *name, HINSTANCE hInstance) :
	Singleton<Win32>(*this),
	mActive(true),
	mAppHasMouse(false),
	mInitFlags(0),
	hInst(hInstance),
	hWnd(0),
	hMutex(0),
	appName(name),
	className(TEXT("WNDCLASS1"))
{}

Win32::~Win32()
{
	killWindow();
	if (hMutex) ReleaseMutex(hMutex);
}