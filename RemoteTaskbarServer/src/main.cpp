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
#include <GdiPlus.h>

WindowControlGUI * gWindowControlGUI;
HookController * gHookController;
NetworkManager * gNetworkManager;
WindowManager * gWindowManager;

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nShowCmd)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	gNetworkManager = new NetworkManager();
	gWindowManager = new WindowManager();

	gWindowControlGUI = new WindowControlGUI(hInst,nShowCmd);
	gWindowControlGUI->createWindow();

	gHookController = new HookController(gWindowControlGUI->hWnd);
	gHookController->init();
	gHookController->hook();

	MSG msg;
	char winTitle[100];

	while(gWindowControlGUI->getMessage(&msg))
	{
		if(msg.message == WM_CBTHOOK && gNetworkManager->state == NET_STATE_CONNECTED)
		{
			if(msg.lParam == CBTHOOK_WINDOWS_CREATE)
			{
				gWindowManager->addWindowTask((HWND)msg.wParam);
			}
			else if(msg.lParam == CBTHOOK_WINDOWS_DESTROY)
			{
				gWindowManager->removeWindowTask((HWND)msg.wParam);
			}
			else if(msg.lParam == CBTHOOK_WINDOWS_RENAME)
			{
				gWindowManager->updateWindowTaskTitle((HWND)msg.wParam);
			}
		}
		else
			gWindowControlGUI->processMessage(&msg);
	}

	gHookController->unhook();


	delete gHookController;
	delete gWindowControlGUI;	

	delete gWindowManager;
	delete gNetworkManager;

	Gdiplus::GdiplusShutdown(gdiplusToken);
}
