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

#ifndef WINDOW_CONTROL_GUI
#define WINDOW_CONTROL_GUI

#include <Windows.h>

#define DISCONNECTED_MSG "Disconnected.\n"
#define CONNECTED_MSG "Connected!\n"
#define CONNECTING_MSG "Awaiting client...\n"

class WindowControlGUI
{
public:
	WindowControlGUI(HINSTANCE hInst,int nShowCmd);
	~WindowControlGUI();
	void createWindow();
	BOOL getMessage(MSG * msg);
	void processMessage(MSG * msg);
	void textAreaPrint(char * str);
	void updateConnectButton();
	HWND hTextArea;
	HWND hConnectButton;
	HWND hWnd;
	HINSTANCE hInst;
	int nShowCmd;
};

#endif