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

#include "network\Network.h"
#include "Global.h"

void Server_Connected(Server * s)
{
	gNetworkManager->state = NET_STATE_CONNECTED;
	gWindowControlGUI->updateConnectButton();
	gWindowControlGUI->textAreaPrint(CONNECTED_MSG);
}

void Server_Recv(Server * s)
{
	int clientRequestType = ((int*)(s->recvBuffer))[0];
	clientRequestType = ntohl(clientRequestType);

	HWND hwnd = 0;
	int ihwnd = 0;

	switch(clientRequestType)
	{
		case CLIENT_REQUEST_ACTIVE_TASK_LIST:
			gWindowManager->refreshWindowTasksList();
			gWindowManager->sendWindowTasksToClient();
		break;

		case CLIENT_SIGNAL_WINDOW_OPEN:
			ihwnd = ntohl(((int*)(s->recvBuffer))[1]);
			hwnd = (HWND)(ihwnd);
			gWindowManager->performWindowOpen(hwnd);
		break;

		case CLIENT_SIGNAL_WINDOW_CLOSE:
			ihwnd = ntohl(((int*)(s->recvBuffer))[1]);
			hwnd = (HWND)(ihwnd);
			gWindowManager->performWindowClose(hwnd);
		break;

		default:
			printf("Unsupported client request: %i\n", clientRequestType);
		break;
	}
}


NetworkManager::NetworkManager()
:_server(0)
,state(NET_STATE_DISCONNECTED)
{

}

NetworkManager::~NetworkManager()
{
	if(_server)
	{
		Server_Close(_server);

		//TODO: clean server threads before destroy
		//Server_Destroy(_server);
		_server = 0;
		state = NET_STATE_DISCONNECTED;
	}
}

void NetworkManager::startUp()
{
	if(_server)
	{
		Server_Close(_server);
		Server_Destroy(_server);
		_server = 0;
	}

	_server = Server_Create();
	Server_Start(_server, PORT);	
	state = NET_STATE_AWAIT_CLIENT;

	Server_OnConnected(Server_Connected);
	Server_OnRecv(Server_Recv);	
}

void NetworkManager::sendMessage(char * buffer, int length)
{
	Server_Send(_server,buffer,length);
}

void NetworkManager::disconnect()
{
	if(_server)
	{
		Server_Close(_server);
		Server_Destroy(_server);
		_server = 0;
		state = NET_STATE_DISCONNECTED;
	}
}