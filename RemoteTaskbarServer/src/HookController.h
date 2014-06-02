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

#ifndef HOOK_CONTROLLER_H
#define HOOK_CONTROLLER_H

#include <Windows.h>

#define WM_CBTHOOK_MSG "WM_CBTHOOK_MSG"
#define CBTHOOK_WINDOWS_CREATE 1
#define CBTHOOK_WINDOWS_DESTROY 2
#define CBTHOOK_WINDOWS_RENAME 3

extern UINT WM_CBTHOOK;

typedef BOOL (*installHook)(HWND msgDestination);
typedef BOOL (*uninstallHook)(HWND msgDestination);


class HookController
{
public:
	HookController(HWND msgReturnHwnd);
	~HookController();
	BOOL init();
	BOOL hook();
	BOOL unhook();
private:
	HWND msgReturnHwnd;
	installHook installHookFunction;
	uninstallHook uninstallHookFunction;
	HMODULE hookDllHandle;
};


#endif