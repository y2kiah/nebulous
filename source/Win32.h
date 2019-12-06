/*----==== WIN32.H ====----
	Author:		Jeff Kiah
	Orig.Date:	12/01/2007
	Rev.Date:	07/09/2009
-------------------------*/

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN	// defined in project settings
#endif

#include <Windows.h>
#include <bitset>
#include "Utility/Singleton.h"

using std::bitset;

///// DECLARATIONS /////

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

///// STRUCTURES /////

/*=============================================================================
class Win32
=============================================================================*/
class Win32 : public Singleton<Win32> {
	private:
		bool	mActive;		// true if this is the active window
		bool	mAppHasMouse;	// true if app has mouse control, false if Windows cursor visible

		// initialization flags
		enum Win32InitFlags {
			INIT_WINDOW = 0,	DEINIT_WINDOW,
			INIT_WNDCLASS,		DEINIT_WNDCLASS,
			INIT_MAX
		};
		bitset<INIT_MAX>	mInitFlags;

	public:
		///// VARIABLES /////

		HINSTANCE	hInst;		
		HWND		hWnd;
		HANDLE		hMutex;

		const TCHAR	*appName;
		const TCHAR	*className;

		///// FUNCTIONS /////

		// misc functions
		bool	isOnlyInstance();
		bool	isActive() const	{ return mActive; }
		void	setActive(bool active = true);	// set and unset app active status
		void	takeCursor();	// takes control of the mouse cursor from Windows
		void	giveCursor();	// gives mouse control back to Windows
		void	showErrorBox(const TCHAR *message);

		// main window
		bool	initWindow();
		void	killWindow();
		bool	checkFullScreenSettings(DISPLAY_DEVICE &dispDevice, DEVMODE &dmScreenSettings);
		bool	getWindowSize(RECT &outRect, DWORD dwStyle, DWORD dwExStyle) const;
		bool	setFullScreen();
		bool	toggleFullScreen();
		bool	initInputDevices();
		bool	getSystemInformation();
		LRESULT wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		// debug console
		bool	initConsole();
		static BOOL WINAPI	ctrlHandler(DWORD dwCtrlType);

		// static functions
		//static void	showErrorBox(const TCHAR *message);

		// Constructor / Destructor
		explicit Win32(const TCHAR *name, HINSTANCE hInstance);
		~Win32();
};