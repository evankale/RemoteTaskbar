/*
* Copyright (c) 2014 Evan Kale
* Email: EvanKale91@gmail.com
* Website: www.ISeeDeadPixel.com
* 
* This file is part of RemoteTaskbar.
*
* RemoteTaskbar is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <windows.h>

#define WM_CBTHOOK_MSG "WM_CBTHOOK_MSG"
#define CBTHOOK_WINDOWS_CREATE 1
#define CBTHOOK_WINDOWS_DESTROY 2
#define CBTHOOK_WINDOWS_RENAME 3

#pragma data_seg(".SHARE")
HWND msgRedirectDestination = NULL;
HHOOK hookHandle = NULL;
HWINEVENTHOOK hookEventHandle = NULL;
#pragma data_seg()
#pragma comment(linker, "/section:.SHARE,rws")

HMODULE dllModuleHandle;
UINT WM_CBTHOOK;


bool IsAltTabWindow(HWND hWnd, BOOL visibilityCheck)
{
	if(hWnd == 0)
		return false;

	//is the window visible?
	if (!visibilityCheck
		|| (visibilityCheck && IsWindowVisible(hWnd)))
	{
		//window has no parent/owner?
		if(GetParent(hWnd)==NULL && GetWindow(hWnd, GW_OWNER) == NULL)
		{			
			//window has title?
			char title[4];
			memset(title,0,4);
			int titleLength = GetWindowText(hWnd,title,3);
			if(titleLength > 0)
			{
				//window is not a tool window?
				if(!(GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW))
					return true;
			}
		}
	}
	return false;
}


extern "C" LRESULT __declspec(dllexport) __stdcall hookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// this is required by the specification of hook handlers
	if(nCode < 0)
	{
		CallNextHookEx(hookHandle, nCode, wParam, lParam);
		return 0;
	}

	BOOL sucess = 0;	

	switch(nCode)
	{

	case HCBT_CREATEWND:
		sucess = PostMessage(msgRedirectDestination,WM_CBTHOOK,0,CBTHOOK_WINDOWS_CREATE);
		break;

	case HCBT_DESTROYWND:
		sucess = PostMessage(msgRedirectDestination,WM_CBTHOOK,0,CBTHOOK_WINDOWS_DESTROY);
		break;

	default:
		break;

	}	

	LRESULT RetVal = CallNextHookEx(hookHandle, nCode, wParam, lParam);	
	return  RetVal;
}

extern "C" void __declspec(dllexport) CALLBACK hookEventProc(	HWINEVENTHOOK hWinEventHook,
																DWORD event,
																HWND hwnd,
																LONG idObject,
																LONG idChild,
																DWORD dwEventThread,
																DWORD dwmsEventTime)
{
	BOOL sucess = 0;

	switch(event)
	{
		case EVENT_OBJECT_DESTROY:
		case EVENT_OBJECT_HIDE:
			//if(IsAltTabWindow(hwnd,false))
				sucess = PostMessage(msgRedirectDestination,WM_CBTHOOK,(WPARAM)hwnd,CBTHOOK_WINDOWS_DESTROY);
		break;

		case EVENT_OBJECT_SHOW:
			if(IsAltTabWindow(hwnd,false))
				sucess = PostMessage(msgRedirectDestination,WM_CBTHOOK,(WPARAM)hwnd,CBTHOOK_WINDOWS_CREATE);
		break;

		case EVENT_OBJECT_NAMECHANGE:
			if(IsAltTabWindow(hwnd,false))
				sucess = PostMessage(msgRedirectDestination,WM_CBTHOOK,(WPARAM)hwnd,CBTHOOK_WINDOWS_RENAME);
		break;
	}
}

extern "C" BOOL __declspec(dllexport) __stdcall installHook(HWND msgDestination)
{
	if(msgRedirectDestination != NULL)
		return FALSE;

	hookHandle = SetWindowsHookEx(WH_CBT, (HOOKPROC)hookProc, dllModuleHandle, 0);

	if(hookHandle != NULL)
	{
		msgRedirectDestination = msgDestination;
		return TRUE;
	}

   return FALSE;
}

extern "C" BOOL __declspec(dllexport) __stdcall installEventHook(HWND msgDestination)
{
	if(msgRedirectDestination != NULL)
		return FALSE;

	hookEventHandle = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_NAMECHANGE, dllModuleHandle, hookEventProc, 0, 0, 0);

	if(hookEventHandle != NULL)
	{
		msgRedirectDestination = msgDestination;
		return TRUE;
	}

   return FALSE;
}

extern "C" BOOL __declspec(dllexport) uninstallHook(HWND msgDestination)
{
	if(msgDestination != msgRedirectDestination)
		return FALSE;

	BOOL unhooked = UnhookWindowsHookEx(hookHandle);

	if(unhooked)
		msgRedirectDestination = NULL;
	return unhooked;
} 

extern "C" BOOL __declspec(dllexport) uninstallEventHook(HWND msgDestination)
{
	if(msgDestination != msgRedirectDestination)
		return FALSE;

	BOOL unhooked = UnhookWinEvent(hookEventHandle);

	if(unhooked)
		msgRedirectDestination = NULL;
	return unhooked;
} 

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved)
{
	switch(reason)
	{
		case DLL_PROCESS_ATTACH:
			dllModuleHandle = hModule;
			WM_CBTHOOK = RegisterWindowMessage(WM_CBTHOOK_MSG);
		break;

		case DLL_PROCESS_DETACH:
			//Dont unhook here, do it in the exe
			//if(msgRedirectDestination != NULL)
			//	uninstallHook(msgRedirectDestination);
		break;
	}

	return TRUE;
}
