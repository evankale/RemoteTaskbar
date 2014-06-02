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

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

struct Server;

#define PORT 9999
#define NET_STATE_DISCONNECTED 0
#define NET_STATE_AWAIT_CLIENT 1
#define NET_STATE_CONNECTED 2

class NetworkManager
{

public:

	NetworkManager();
	~NetworkManager();
	void startUp();
	void sendMessage(char * buffer, int length);
	void disconnect();
	int state;

private:	
	Server * _server;
};

#endif