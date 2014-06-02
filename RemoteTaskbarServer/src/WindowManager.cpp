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

#include "util\IconUtil.h"


#define MAX_WINDOW_TITLE_LENGTH 255


bool CALLBACK enumWindowsProc(HWND hWnd, LPARAM lParam)
{
	if (gWindowManager->isAltTabWindow(hWnd))
	{
		gWindowManager->_activeWindowTasks.insert(std::pair<HWND,HICON>(hWnd,(HICON)NULL));
    }

    return TRUE;
}

WindowManager::WindowManager()
{
}

WindowManager::~WindowManager()
{
}

void WindowManager::reset()
{
	_activeWindowTasks.clear();
	_clientCachedIcons.clear();
}

bool WindowManager::iconExistsInClientCache(HICON hIcon)
{
	std::unordered_map<HICON,int>::iterator it = _clientCachedIcons.find(hIcon);
	bool found = (it != _clientCachedIcons.end());
	return found;
}

void WindowManager::addIconToClientCache(HICON hIcon)
{
	_clientCachedIcons.insert(std::pair<HICON,int>(hIcon,1));
}

void WindowManager::refreshWindowTasksList()
{
	_activeWindowTasks.clear();
	EnumWindows((WNDENUMPROC)(enumWindowsProc), 0);

	for(std::unordered_map<HWND,HICON>::iterator it = _activeWindowTasks.begin();
		it != _activeWindowTasks.end(); ++it)
	{
		std::pair<const HWND,HICON> & task = *it;

		HICON hIcon = IconUtil::GetHICON(task.first);

		task.second = hIcon;

		if(!iconExistsInClientCache(hIcon))
		{
			sendIconToClient(hIcon);
			addIconToClientCache(hIcon);
		}

		/*#define PNG_FILESIZE 8000

		unsigned char pngBuffer[PNG_FILESIZE];
		unsigned int pngSize = 0;
		IconUtil::HIconToPNG(pngBuffer,PNG_FILESIZE,hIcon, &pngSize);

		
		char title[20];
		GetWindowText(_activeWindowTasks[i], title, 19);
		printf("%s\t%i\n",title,hwnd);

		char filename[256];
		sprintf(filename,"icon%i.png",hwnd);

		FILE * f = fopen(filename,"wb");
		fwrite(pngBuffer,pngSize,1,f);
		fclose(f);*/
	}

}


void WindowManager::sendIconToClient(HICON hIcon)
{
	#define PNG_MAX_FILESIZE 8000

	/*
	struct SendBuffer
	{
		int responseType;
		HICON hicon;
		int pngSize;		
		char png[pngSize];
	};
	*/

	char buffer[sizeof(int)*3+PNG_MAX_FILESIZE];

	unsigned int pngSize = 0;
	IconUtil::HIconToPNG((unsigned char*)(buffer+(sizeof(int)*3)),PNG_MAX_FILESIZE,hIcon,&pngSize);

	char * currBufferPtr = buffer;

	*((int*)currBufferPtr) = htonl(SERVER_RESPONSE_SEND_ICON);
	currBufferPtr += sizeof(int);

	*((int*)currBufferPtr) = htonl((int)hIcon);
	currBufferPtr += sizeof(int);

	*((int*)currBufferPtr) = htonl(pngSize);
	currBufferPtr += sizeof(int);

	gNetworkManager->sendMessage(buffer,sizeof(int)*3+pngSize);
}


void WindowManager::sendWindowTasksToClient()
{
    char windowTitle[MAX_WINDOW_TITLE_LENGTH];
	int numActiveWindowTasks = _activeWindowTasks.size();

	/*
	struct SendBuffer
	{
		int responseType;
		int numActiveWindowTasks;
		struct Task
		{
			int hwnd;
			int hicon;
			int titleLength;
			char title[titleLeng];
		} tasks[numActiveWindowTasks];
	};
	*/

	int bufferSize = sizeof(int) + sizeof(int) + numActiveWindowTasks*(sizeof(int)+sizeof(int)+sizeof(int)+MAX_WINDOW_TITLE_LENGTH);
	char *buffer = new char[bufferSize];

	char * currBufferPtr = buffer;

	*((int*)currBufferPtr) = htonl(SERVER_RESPONSE_ACTIVE_TASK_LIST);
	currBufferPtr += sizeof(int);

	*((int*)currBufferPtr) = htonl(numActiveWindowTasks);
	currBufferPtr += sizeof(int);

	for(std::unordered_map<HWND,HICON>::iterator it = _activeWindowTasks.begin();
		it != _activeWindowTasks.end(); ++it)
	{
		std::pair<const HWND,HICON> & task = *it;

		memset(windowTitle,0,MAX_WINDOW_TITLE_LENGTH*sizeof(char));
		
		//NOTE:
		//returned "titleLength" does not include \0.
		//paramaeter "MAX_WINDOW_TITLE_LENGTH-1" is max copied length, including \0.
		int titleLength = GetWindowText(task.first, windowTitle, MAX_WINDOW_TITLE_LENGTH-1);

		*((int*)currBufferPtr) = htonl((int)task.first);
		currBufferPtr += sizeof(int);

		*((int*)currBufferPtr) = htonl((int)task.second);
		currBufferPtr += sizeof(int);

		*((int*)currBufferPtr) = htonl(titleLength+1);
		currBufferPtr += sizeof(int);

		strcpy(currBufferPtr, windowTitle);
		currBufferPtr += titleLength+1;
	}

	gNetworkManager->sendMessage(buffer,currBufferPtr-buffer);

	delete[] buffer;

}



bool WindowManager::isAltTabWindow(HWND hWnd)
{
	//is the window visible?
	if (IsWindowVisible(hWnd))
	{
		//window has no parent/owner?
		if(GetParent(hWnd)==NULL && GetWindow(hWnd, GW_OWNER) == NULL)
		{			
			//window is not a tool window?
			if(!(GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW))
				return true;
		}
	}
	return false;
}


void WindowManager::performWindowOpen(HWND hWnd)
{
	SwitchToThisWindow(hWnd, true);
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	//SendMessage(hWnd,WM_SETFOCUS,0,0);
}

void WindowManager::performWindowClose(HWND hWnd)
{
	SendMessage(hWnd,WM_CLOSE,0,0);
}

void WindowManager::addWindowTask(HWND hwnd)
{
	std::unordered_map<HWND,HICON>::iterator it = _activeWindowTasks.find(hwnd);

	//add if not found
	if(it == _activeWindowTasks.end())
	{		
		HICON hIcon = IconUtil::GetHICON(hwnd);

		_activeWindowTasks.insert(std::pair<HWND,HICON>(hwnd,hIcon));

		if(!iconExistsInClientCache(hIcon))
		{
			sendIconToClient(hIcon);
			addIconToClientCache(hIcon);
		}

		/*
		struct SendBuffer
		{
			int responseType;
			int hwnd;
			int hicon;
			int titleLength;
			char title[titleLeng];
		};
		*/

		char buffer[sizeof(int) + (sizeof(int)+sizeof(int)+sizeof(int)+MAX_WINDOW_TITLE_LENGTH)];

		char * currBufferPtr = buffer;

		*((int*)currBufferPtr) = htonl(SERVER_RESPONSE_ADD_TASK);
		currBufferPtr += sizeof(int);

		*((int*)currBufferPtr) = htonl((int)hwnd);
		currBufferPtr += sizeof(int);

		*((int*)currBufferPtr) = htonl((int)hIcon);
		currBufferPtr += sizeof(int);

		char windowTitle[MAX_WINDOW_TITLE_LENGTH];
		memset(windowTitle,0,MAX_WINDOW_TITLE_LENGTH*sizeof(char));
		
		//NOTE:
		//returned "titleLength" does not include \0.
		//paramaeter "MAX_WINDOW_TITLE_LENGTH-1" is max copied length, including \0.
		int titleLength = GetWindowText(hwnd, windowTitle, MAX_WINDOW_TITLE_LENGTH-1);

		*((int*)currBufferPtr) = htonl(titleLength+1);
		currBufferPtr += sizeof(int);

		strcpy(currBufferPtr, windowTitle);
		currBufferPtr += titleLength+1;

		gNetworkManager->sendMessage(buffer,currBufferPtr-buffer);

	}
}

void WindowManager::removeWindowTask(HWND hwnd)
{
	std::unordered_map<HWND,HICON>::iterator it = _activeWindowTasks.find(hwnd);

	//remove if found
	if(it != _activeWindowTasks.end())
	{	
		_activeWindowTasks.erase(it);

		//TODO:
		//send remove task to client(hwnd)

		/*
		struct SendBuffer
		{
			int responseType;
			int hwnd;
		};
		*/

		char buffer[sizeof(int) + sizeof(int)];

		char * currBufferPtr = buffer;

		*((int*)currBufferPtr) = htonl(SERVER_RESPONSE_REMOVE_TASK);
		currBufferPtr += sizeof(int);

		*((int*)currBufferPtr) = htonl((int)hwnd);
		currBufferPtr += sizeof(int);

		gNetworkManager->sendMessage(buffer,currBufferPtr-buffer);
	}
}

void WindowManager::updateWindowTaskTitle(HWND hwnd)
{
	std::unordered_map<HWND,HICON>::iterator it = _activeWindowTasks.find(hwnd);

	//update if found
	if(it != _activeWindowTasks.end())
	{	
		/*
		struct SendBuffer
		{
			int responseType;
			int hwnd;
			int titleLength;
			char title[titleLeng];
		};
		*/

		char buffer[sizeof(int) + (sizeof(int)+sizeof(int)+MAX_WINDOW_TITLE_LENGTH)];

		char * currBufferPtr = buffer;

		*((int*)currBufferPtr) = htonl(SERVER_RESPONSE_UPDATE_TASK);
		currBufferPtr += sizeof(int);

		*((int*)currBufferPtr) = htonl((int)hwnd);
		currBufferPtr += sizeof(int);

		char windowTitle[MAX_WINDOW_TITLE_LENGTH];
		memset(windowTitle,0,MAX_WINDOW_TITLE_LENGTH*sizeof(char));
		
		//NOTE:
		//returned "titleLength" does not include \0.
		//paramaeter "MAX_WINDOW_TITLE_LENGTH-1" is max copied length, including \0.
		int titleLength = GetWindowText(hwnd, windowTitle, MAX_WINDOW_TITLE_LENGTH-1);

		*((int*)currBufferPtr) = htonl(titleLength+1);
		currBufferPtr += sizeof(int);

		strcpy(currBufferPtr, windowTitle);
		currBufferPtr += titleLength+1;

		gNetworkManager->sendMessage(buffer,currBufferPtr-buffer);

	}
}
