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

#define PADDING 10
#define BUTTON_W 150
#define BUTTON_H 25
#define TEXTAREA_W 200
#define TEXTAREA_H 100

#define CONNECT_BTN_MSG "Connect"
#define DISCONNECT_BTN_MSG "Disconnect"
#define CONNECTING_BTN_MSG "Connecting..."

#define IDC_CONNECT_BUTTON	101			// Button identifier
#define IDC_ABOUT_BUTTON	102			// Button identifier
#define IDC_EXIT_BUTTON		103			// Button identifier
#define IDC_MAIN_EDIT		104			// Edit box identifier

LRESULT CALLBACK WindowControlGUIProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		{
			// Create an edit box
			gWindowControlGUI->hTextArea=CreateWindowEx(WS_EX_CLIENTEDGE,
				"EDIT",
				"",
				WS_CHILD|WS_VISIBLE|
				ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_READONLY,
				PADDING + BUTTON_W + PADDING,
				PADDING,
				TEXTAREA_W,
				TEXTAREA_H,
				hWnd,
				(HMENU)IDC_MAIN_EDIT,
				gWindowControlGUI->hInst,
				NULL);

			HGDIOBJ hfDefault=GetStockObject(DEFAULT_GUI_FONT);

			SendMessage(gWindowControlGUI->hTextArea,
				WM_SETFONT,
				(WPARAM)hfDefault,
				MAKELPARAM(FALSE,0));

			gWindowControlGUI->textAreaPrint(DISCONNECTED_MSG);

			// Create a connect button
			gWindowControlGUI->hConnectButton=CreateWindowEx(NULL,
				"BUTTON",
				CONNECT_BTN_MSG,
				WS_TABSTOP|WS_VISIBLE|
				WS_CHILD|BS_DEFPUSHBUTTON,
				PADDING,
				PADDING,
				BUTTON_W,
				BUTTON_H,
				hWnd,
				(HMENU)IDC_CONNECT_BUTTON,
				gWindowControlGUI->hInst,
				NULL);

			SendMessage(gWindowControlGUI->hConnectButton,
				WM_SETFONT,
				(WPARAM)hfDefault,
				MAKELPARAM(FALSE,0));

			// Create a about button
			HWND hWndButton=CreateWindowEx(NULL,
				"BUTTON",
				"About",
				WS_TABSTOP|WS_VISIBLE|
				WS_CHILD|BS_DEFPUSHBUTTON,
				PADDING,
				PADDING * 2 + BUTTON_H,
				BUTTON_W,
				BUTTON_H,
				hWnd,
				(HMENU)IDC_ABOUT_BUTTON,
				gWindowControlGUI->hInst,
				NULL);

			SendMessage(hWndButton,
				WM_SETFONT,
				(WPARAM)hfDefault,
				MAKELPARAM(FALSE,0));

			// Create a about button
			hWndButton=CreateWindowEx(NULL,
				"BUTTON",
				"Exit",
				WS_TABSTOP|WS_VISIBLE|
				WS_CHILD|BS_DEFPUSHBUTTON,
				PADDING,
				PADDING * 3 + BUTTON_H * 2,
				BUTTON_W,
				BUTTON_H,
				hWnd,
				(HMENU)IDC_EXIT_BUTTON,
				gWindowControlGUI->hInst,
				NULL);

			SendMessage(hWndButton,
				WM_SETFONT,
				(WPARAM)hfDefault,
				MAKELPARAM(FALSE,0));
		}
		break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
            {
				case IDC_CONNECT_BUTTON:
				{
					if(gNetworkManager->state == NET_STATE_DISCONNECTED)
					{
						gNetworkManager->startUp();
						gWindowManager->reset();
						gWindowControlGUI->textAreaPrint(CONNECTING_MSG);
						gWindowControlGUI->updateConnectButton();
					}
					else if(gNetworkManager->state == NET_STATE_CONNECTED)
					{
						gNetworkManager->disconnect();
						gWindowControlGUI->textAreaPrint(DISCONNECTED_MSG);
						gWindowControlGUI->updateConnectButton();
					}

					break;
				}

				case IDC_ABOUT_BUTTON:
				{
					MessageBox(hWnd,
						"Remote Taskbar Server v0.1\nEvanKale91@gmail.com\nwww.iseedeadpixel.com",
						"About",
						MB_ICONINFORMATION);
					break;
				}

				case IDC_EXIT_BUTTON:
				{
					PostQuitMessage(0);
				}
				break;
			}
			break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		break;
	}

	return DefWindowProc(hWnd,msg,wParam,lParam);
}

WindowControlGUI::WindowControlGUI(HINSTANCE hInst,int nShowCmd)
: hInst(hInst)
, nShowCmd(nShowCmd)
{
}

void WindowControlGUI::createWindow()
{
	WNDCLASSEX wClass;
	ZeroMemory(&wClass,sizeof(WNDCLASSEX));
	wClass.cbClsExtra		= NULL;
	wClass.cbSize			= sizeof(WNDCLASSEX);
	wClass.cbWndExtra		= NULL;
	wClass.hbrBackground	= (HBRUSH)COLOR_WINDOW;
	wClass.hCursor			= LoadCursor(NULL,IDC_ARROW);
	wClass.hIcon			= NULL;
	wClass.hIconSm			= NULL;
	wClass.hInstance		= hInst;
	wClass.lpfnWndProc		= (WNDPROC)WindowControlGUIProc;
	wClass.lpszClassName	= "WindowControlGUIWindowClass";
	wClass.lpszMenuName		= NULL;
	wClass.style			= CS_HREDRAW|CS_VREDRAW;

	if(!RegisterClassEx(&wClass))
	{
		int nResult=GetLastError();
		MessageBox(NULL,
			"Window class creation failed\r\n",
			"Window Class Failed",
			MB_ICONERROR);
	}

	hWnd=CreateWindowEx(NULL,
			"WindowControlGUIWindowClass",
			"Remote Taskbar Server",
			WS_OVERLAPPEDWINDOW,
			100,		//x
			100,		//y
			PADDING * 4 + BUTTON_W + TEXTAREA_W,	//w
			PADDING * 5 + TEXTAREA_H,				//h
			NULL,		//parent
			NULL,		//menu
			hInst,		//instance
			NULL);		//param

	if(!hWnd)
	{
		int nResult=GetLastError();

		MessageBox(NULL,
			"Window creation failed\r\n",
			"Window Creation Failed",
			MB_ICONERROR);
	}

    ShowWindow(hWnd,nShowCmd);
}

WindowControlGUI::~WindowControlGUI()
{

}

BOOL WindowControlGUI::getMessage(MSG * msg)
{
	return GetMessage(msg,0,0,0);
}

void WindowControlGUI::processMessage(MSG * msg)
{
	TranslateMessage(msg);
	DispatchMessage(msg);
}

void WindowControlGUI::textAreaPrint(char * str)
{
	int length = GetWindowTextLength(hTextArea);
	SendMessage(hTextArea, EM_SETSEL, WPARAM(length), LPARAM(length));
	SendMessage(hTextArea, EM_REPLACESEL, WPARAM(TRUE), LPARAM(str));
}

void WindowControlGUI::updateConnectButton()
{
	if(gNetworkManager->state == NET_STATE_DISCONNECTED)
	{
		SendMessage(hConnectButton,WM_SETTEXT,0,(LPARAM)CONNECT_BTN_MSG);
	}
	else if(gNetworkManager->state == NET_STATE_AWAIT_CLIENT)
	{
		SendMessage(hConnectButton,WM_SETTEXT,0,(LPARAM)CONNECTING_BTN_MSG);
		SendMessage(hConnectButton,WM_ENABLE,0,(LPARAM)FALSE);
	}
	else if(gNetworkManager->state == NET_STATE_CONNECTED)
	{
		SendMessage(hConnectButton,WM_SETTEXT,0,(LPARAM)DISCONNECT_BTN_MSG);
		SendMessage(hConnectButton,WM_ENABLE,0,(LPARAM)TRUE);
	}
}