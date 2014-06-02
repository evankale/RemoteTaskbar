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

static void (*IClient_OnRecv)(Client*) = NULL;

//===========
// WSA STUFF
//===========

int WSA_started = 0;

int WSA_Startup()
{
	if(PRINT_FUNCTION_TRACE)
		printf("<WSA_Startup>\n");

	WSADATA wsaData;
	WORD version;
	int error;

	version = MAKEWORD( 2, 2 );

	error = WSAStartup( version, &wsaData );

	if ( error != 0 )
	{
		if(PRINT_FUNCTION_TRACE)
			printf("</WSA_Startup>\n");
		return 0;
	}

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
		 HIBYTE( wsaData.wVersion ) != 2 )
	{
		WSACleanup();
		if(PRINT_FUNCTION_TRACE)
			printf("</WSA_Startup>\n");
		return 0;
	}

	WSA_started = 1;

	if(PRINT_FUNCTION_TRACE)
		printf("</WSA_Startup>\n");
	return 1;
}


//=======================
// Internal Client STUFF
//=======================

int IClient_GetHost(Client * client)
{
	if(PRINT_FUNCTION_TRACE)
		printf("<IClient_GetHost>\n");

    client->hostEnt = gethostbyname(client->hostname);

    if(!client->hostEnt)
    {
        printf("Unable to collect gethostbyname() on IClient_GetHost\n");

		if(PRINT_FUNCTION_TRACE)
			printf("</IClient_GetHost>\n");
        return 0;
    }

	if(PRINT_FUNCTION_TRACE)
		printf("</IClient_GetHost>\n");
    return 1;
}


int IClient_CreateSocket(Client * client)
{
	if(PRINT_FUNCTION_TRACE)
		printf("<IClient_CreateSocket>\n");

    client->socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(client->socket == SOCKET_ERROR)
    {
        printf("Unable to create socket on IClient_CreateSocket\n");

		if(PRINT_FUNCTION_TRACE)
			printf("</IClient_CreateSocket>\n");
        return 0;
    }

	if(PRINT_FUNCTION_TRACE)
		printf("</IClient_CreateSocket>\n");
    return 1;
}

int IClient_Connect(Client * client)
{
	if(PRINT_FUNCTION_TRACE)
		printf("<IClient_Connect>\n");

	int retVal = 0;
    SOCKADDR_IN serverInfo;

    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr = *((LPIN_ADDR)*(client->hostEnt)->h_addr_list);

    serverInfo.sin_port = htons(client->port);

    retVal=connect(client->socket,(LPSOCKADDR)&serverInfo, sizeof(serverInfo));
    if(retVal==SOCKET_ERROR)
    {
        printf("Unable to connect() on IClient_Connect\n");

		if(PRINT_FUNCTION_TRACE)
			printf("</IClient_Connect>\n");
        return 0;
    }

    client->connected = 1;

	if(PRINT_FUNCTION_TRACE)
		printf("</IClient_Connect>\n");
    return 1;
}


void __stdcall IClient_RecieveThread(Client * client)
{
	while(!client->connected) Sleep(100);

	while(client->connected)
	{
		int retVal = 0;

		retVal = recv(client->socket, client->recvBuffer, BUFFER_SIZE, 0);
		client->recvBufferLength = retVal;

		if(retVal == SOCKET_ERROR)
		{
			printf("Unable to recv() on IClient_RecieveThread\n");
			continue;
		}

		else if(IClient_OnRecv)
		{
			IClient_OnRecv(client);
		}

		if(retVal==0)
		{
			break;
		}
	}
}

int IClient_Recieve(Client * client)
{
	if(PRINT_FUNCTION_TRACE)
		printf("<IClient_Recieve>\n");

	CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE)IClient_RecieveThread, client, 0, NULL);

	if(PRINT_FUNCTION_TRACE)
		printf("</IClient_Recieve>\n");
    return 1;
}


//=======================
// External Client STUFF
//=======================

Client * Client_Create()
{
	Client * client = new Client();
	memset(client,0,sizeof(Client));
	return client;
}

void Client_Connect(Client * client, char * hostname, int port)
{
	if(PRINT_FUNCTION_TRACE)
		printf("<Client_Connect>\n");

	client->hostname = hostname;
	client->port = port;

	int sucess = 1;

	if(!WSA_started)
		sucess = WSA_Startup();

	if(!sucess)
		printf("WSA Startup Error on Client_Connect()\n");

	client->recvBuffer = (char*)calloc(BUFFER_SIZE,sizeof(char));

	IClient_GetHost(client);
	IClient_CreateSocket(client);
	IClient_Connect(client);
	IClient_Recieve(client);

	Sleep(CLIENT_CONNECT_DELAY);

	if(PRINT_FUNCTION_TRACE)
		printf("</Client_Connect>\n");
}


int Client_Send(Client * client, char * buffer, int length)
{
    int retVal = 0;

    retVal = send(client->socket, buffer, length, 0);

    if(retVal == SOCKET_ERROR)
    {
        printf("Unable to send() on Client_Send\n");
        //WSACleanup();
        return 0;
    }

    Sleep(SEND_DELAY);

    return 1;
}

void Client_OnRecv(void(*func)(Client*))
{
	IClient_OnRecv = func;
}

void Client_Close(Client * client)
{
	closesocket(client->socket);
	client->connected = 0;
}


