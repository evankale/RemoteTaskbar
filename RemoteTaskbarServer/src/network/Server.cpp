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

#include "Network.h"

using namespace std;


static void (*IServer_OnRecv)(Server*) = NULL;
static void (*IServer_OnConnected)(Server*) = NULL;

int IServer_Recieve(Server * server);

//=======================
// Internal Server STUFF
//=======================

int IServer_CreateSocket(Server * server)
{
	if(PRINT_FUNCTION_TRACE)
		printf("<IServer_CreateSocket>\n");

    server->serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(server->serverSocket == INVALID_SOCKET)
    {
        printf("Unable to create socket() in IServer_CreateServerSocket\n");
        //WSACleanup();

		if(PRINT_FUNCTION_TRACE)
			printf("</IServer_CreateSocket>\n");
        return 0;
    }

	if(PRINT_FUNCTION_TRACE)
		printf("</IServer_CreateSocket>\n");
    return 1;
}

int IServer_Bind(Server * server)
{
	if(PRINT_FUNCTION_TRACE)
		printf("<IServer_Bind>\n");

    SOCKADDR_IN sin;
    sin.sin_family = PF_INET;
    sin.sin_port = htons(server->port);
    sin.sin_addr.s_addr = INADDR_ANY;
    int retVal = 0;

    retVal = bind(server->serverSocket, (LPSOCKADDR)&sin, sizeof(sin));
    if(retVal == SOCKET_ERROR)
    {
        printf("Unable to bind on IServer_Bind\n");
        //WSACleanup();

		if(PRINT_FUNCTION_TRACE)
			printf("</IServer_Bind>\n");
        return 0;
    }

	if(PRINT_FUNCTION_TRACE)
		printf("</IServer_Bind>\n");
    return 1;
}

int IServer_Listen(Server * server)
{
	if(PRINT_FUNCTION_TRACE)
		printf("<IServer_Listen>\n");

	int retVal=0;
    retVal = listen(server->serverSocket, 10);
    if(retVal == SOCKET_ERROR)
    {
        printf("Unable to listen() on IServer_Listen\n");
        //WSACleanup();

		if(PRINT_FUNCTION_TRACE)
			printf("</IServer_Listen>\n");
        return 0;
    }

	if(PRINT_FUNCTION_TRACE)
		printf("</IServer_Listen>\n");
    return 1;
}


void __stdcall IServer_AcceptThread(Server * server)
{

	server->clientSocket = accept(server->serverSocket, NULL, NULL);


    if(server->clientSocket == INVALID_SOCKET)
    {
        printf("Unable to accept() on IServer_AcceptThread\n");
        //WSACleanup();
    }
    else
	{    
		server->connected = 1;

		if(IServer_OnConnected)
		{
			IServer_OnConnected(server);
		}
	}
}


int IServer_Accept(Server * server)
{
	if(PRINT_FUNCTION_TRACE)
		printf("<IServer_Accept>\n");

	CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE)IServer_AcceptThread, server, 0, NULL);

	if(PRINT_FUNCTION_TRACE)
		printf("</IServer_Accept>\n");
    return 1;
}

void __stdcall IServer_RecieveThread(Server * server)
{
	while(!server->connected) Sleep(100);

	while(server->connected)
	{
		int retVal = 0;

		//printf("RECV..\n");
		retVal = recv(server->clientSocket, server->recvBuffer, BUFFER_SIZE, 0);
		//printf("\tRECVD %i\n", retVal);
		server->recvBufferLength = retVal;

		if(retVal == SOCKET_ERROR)
		{
			printf("Unable to recv() on IServer_RecieveThread\n");
			break;
		}

		else if(IServer_OnRecv)
		{
			IServer_OnRecv(server);
		}

		if(retVal==0)
		{
			break;
		}
	}
}

int IServer_Recieve(Server * server)
{
	if(PRINT_FUNCTION_TRACE)
		printf("<IServer_Recieve>\n");

	CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE)IServer_RecieveThread, server, 0, NULL);

	if(PRINT_FUNCTION_TRACE)
		printf("</IServer_Recieve>\n");
    return 1;
}


//=======================
// External Server STUFF
//=======================

Server * Server_Create()
{
	Server * server = (Server*)calloc(1,sizeof(Server));
	return server;
}

void Server_Start(Server * server, int port)
{
	if(PRINT_FUNCTION_TRACE)
		printf("<Server_Start>\n");

	server->port = port;

	int sucess = 1;

	if(!WSA_started)
		sucess = WSA_Startup();

	if(!sucess)
		printf("WSA Startup Error on Client_Connect()\n");

	server->recvBuffer = (char*)calloc(BUFFER_SIZE,sizeof(char));

	IServer_CreateSocket(server);
	IServer_Bind(server);
	IServer_Listen(server);
	IServer_Accept(server);
	IServer_Recieve(server);


	server->started = 1;

	if(PRINT_FUNCTION_TRACE)
		printf("</Server_Start>\n");
}

int Server_Send(Server * server, char * buffer, int length)
{
    int retVal = 0;

    //printf("SEND..\n");
    retVal = send(server->clientSocket, buffer, length, 0);
    //printf("\tSENT\n");

    if(retVal == SOCKET_ERROR)
    {
        printf("Unable to send() on Client_Send\n");
        //WSACleanup();
        return 0;
    }

    Sleep(SEND_DELAY);

    return 1;
}

void Server_OnRecv(void(*func)(Server*))
{
	IServer_OnRecv = func;
}

void Server_OnConnected(void(*func)(Server*))
{
	IServer_OnConnected = func;
}

void Server_Close(Server * server)
{
	if(server->clientSocket)
	{
		closesocket(server->clientSocket);
		server->clientSocket = 0;
	}
	if(server->serverSocket)
	{
		closesocket(server->serverSocket);
		server->serverSocket = 0;
	}
	server->connected = 0;
	server->started = 0;
}

void Server_Destroy(Server * server)
{
	if(server)
	{
		if(server->recvBuffer)
		{
			free(server->recvBuffer);
			server->recvBuffer = 0;
		}

		free(server);
		server = 0;
	}
}

