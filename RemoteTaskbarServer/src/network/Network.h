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

#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <iostream>

#define PRINT_FUNCTION_TRACE 1
#define BUFFER_SIZE 256

#define SEND_DELAY 100
#define CLIENT_CONNECT_DELAY 100

extern int WSA_started;

struct Client
{
	char * hostname;
	int port;
	LPHOSTENT hostEnt;
	SOCKET socket;
	bool connected;
	char * recvBuffer;
	int recvBufferLength;
};

struct Server
{
	int port;
	SOCKET serverSocket;
	SOCKET clientSocket;
	bool connected;
	bool started;
	char * recvBuffer;
	int recvBufferLength;
};

Server * Server_Create();
void Server_Start(Server * server, int port);
void Server_OnRecv(void(*func)(Server*));
void Server_OnConnected(void(*func)(Server*));
int Server_Send(Server * server, char * buffer, int length);
void Server_Close(Server * server);
void Server_Destroy(Server * server);

Client * Client_Create();
void Client_Connect(Client * client, char * server, int port);
void Client_OnRecv(void(*func)(Client*));
int Client_Send(Client * client, char * buffer, int length);
void Client_Close(Client * client);

int WSA_Startup();

char * Packet_LoadFromFile(char * filename, int * packetLength_out);

#endif