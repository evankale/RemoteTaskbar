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

#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <windows.h>
#include <vector>
#include <unordered_map>

class WindowManager
{

public:
	WindowManager();
	~WindowManager();

	void refreshWindowTasksList();

	void addWindowTask(HWND hwnd);
	void removeWindowTask(HWND hwnd);
	void updateWindowTaskTitle(HWND hwnd);
	
	void sendWindowTasksToClient();

	void sendIconToClient(HICON hIcon);

	void performWindowOpen(HWND hwnd);
	void performWindowClose(HWND hWnd);

	bool isAltTabWindow(HWND hWnd);

	bool iconExistsInClientCache(HICON hIcon);
	void addIconToClientCache(HICON hIcon);

	void reset();

	std::unordered_map<HWND,HICON> _activeWindowTasks;
	std::unordered_map<HICON,int> _clientCachedIcons;

};

#endif