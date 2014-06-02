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

#include "Global.h"
#include "Shlwapi.h"

#define DLL_NAME "HookDLL.dll"

#define INSTALL_FUNCTION "installEventHook"
#define UNINSTALL_FUNCTION "uninstallEventHook"

UINT WM_CBTHOOK;

HookController::HookController(HWND msgReturnHwnd)
: msgReturnHwnd(msgReturnHwnd)
, hookDllHandle(0)
{
}

HookController::~HookController()
{
	unhook();

	if(hookDllHandle)
	{
		FreeLibrary(hookDllHandle);
		hookDllHandle = 0;
	}
}

BOOL HookController::init()
{
	WM_CBTHOOK = RegisterWindowMessage(WM_CBTHOOK_MSG);

	char exePath[MAX_PATH];
	GetModuleFileName(NULL,exePath,MAX_PATH);
	PathRemoveFileSpec(exePath);

	char dllPath[MAX_PATH];
	sprintf(dllPath, "%s\\%s", exePath, DLL_NAME);

	// Load library in which we'll be hooking our functions.
	hookDllHandle = LoadLibrary(dllPath);

	if(hookDllHandle == NULL)
	{
		return false;
	}
 
	// Get the address of the function inside the DLL.
	installHookFunction = (installHook)GetProcAddress(hookDllHandle, INSTALL_FUNCTION);

	if(installHookFunction == NULL)
	{
		return false;
	}

	uninstallHookFunction = (uninstallHook)GetProcAddress(hookDllHandle, UNINSTALL_FUNCTION);

	if(uninstallHookFunction == NULL)
	{
		return false;
	}

	return true;
}

BOOL HookController::hook()
{
	// Hook the function.
	return installHookFunction(msgReturnHwnd);
}

BOOL HookController::unhook()
{
	// Unhook the function.	
	return uninstallHookFunction(msgReturnHwnd);
}
