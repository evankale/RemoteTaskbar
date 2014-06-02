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

#ifndef GLOBAL_H
#define GLOBAL_H

#include "WindowControlGUI.h"
#include "HookController.h"
#include "NetworkManager.h"
#include "WindowManager.h"

//signals from the client to server
#define CLIENT_REQUEST_ACTIVE_TASK_LIST 1001
#define CLIENT_SIGNAL_WINDOW_OPEN 1002
#define CLIENT_SIGNAL_WINDOW_CLOSE 1003

//signals from the server to client
#define SERVER_RESPONSE_ACTIVE_TASK_LIST 2001
#define SERVER_RESPONSE_SEND_ICON 2002
#define SERVER_RESPONSE_ADD_TASK 2003
#define SERVER_RESPONSE_REMOVE_TASK 2004
#define SERVER_RESPONSE_UPDATE_TASK 2005

extern WindowControlGUI * gWindowControlGUI;
extern HookController * gHookController;

extern NetworkManager * gNetworkManager;
extern WindowManager * gWindowManager;

#endif GLOBAL_H