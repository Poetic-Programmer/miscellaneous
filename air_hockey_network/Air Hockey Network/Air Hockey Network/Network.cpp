#include "Network.h"
#include <stdio.h>
#include <conio.h>
#include <iostream>
using namespace std;

Network::Network()
{
	m_numConnections = 0;
	m_sock = new SOCKET[2];
	m_sock[0] = INVALID_SOCKET;
	m_sock[1] = INVALID_SOCKET;
	m_connected = false;
	//ZeroMemory(&m_wsaData, sizeof(WSADATA)); 
	printf("CONTRUCTOR");
}

Network::~Network()
{
	for(int i = 0; i < 2; ++i)
		closesocket (m_sock[i]);
	WSACleanup ();
}



void Network::Initialize()
{
	ZeroMemory(&m_wsaData, sizeof(WSADATA)); 
	StartWinSock();
	SetUpServer();

	FD_ZERO(&m_master);			// Clear the sets
	FD_ZERO(&m_fdTemp);
	FD_SET(m_sock[0], &m_master);		// Put listening socket in the Master set

	m_timeout.tv_sec=60;			// or until some activity
	m_timeout.tv_usec=500000;
}

void Network::StartWinSock()
{
	int error = WSAStartup (0x0202,&m_wsaData);
	if (error)
	{
		printf("Error:  You need WinSock 2.2!\n");
		return;
	}
	else
	{
		printf("Succesfully Started Winsock\n");
	}
	if (m_wsaData.wVersion!=0x0202)
	{
		printf("Error:  Wrong WinSock version!\n");
		WSACleanup ();
		return;
	}

}

void Network::SetUpServer()
{
	if ((m_sock[0] = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf("Error:  Failed to create socket");
		return;
	}

	m_server.sin_family = AF_INET;
	m_server.sin_port = htons (PORTNUM);
	m_server.sin_addr.s_addr = htonl (INADDR_ANY);

	u_short PortNo;
	PortNo = (m_server.sin_port >> 8)|(m_server.sin_port << 8);
	printf("My PORT address is: ");
	printf("%d\n", (int)PortNo);
}

void Network::ListenForConnections()
{
	char Buffer[BUFFERSIZE];
	int sa_size = sizeof(sockaddr);
	printf("Listening for  a connections...\n");
		
	// Bind the listening socket
	if (bind(m_sock[0], (LPSOCKADDR)&m_server, sizeof(m_server)) == SOCKET_ERROR)
	{
		printf("Error:  Unable to bind socket!\n");
		WSACleanup ();
		return;
	}

	// Listen for connections
	if (listen(m_sock[0], 1) == SOCKET_ERROR)
	{
		printf("Error:  Unable to listen!\n");
		WSACleanup ();
		return;
	}

	// Get working copy of set
	m_fdTemp = m_master;	
		
	// Look for socket activity
	select(0, &m_fdTemp, NULL, NULL, &m_timeout); 
		
	// Process any connection attempts
	if(FD_ISSET(m_sock[0], &m_fdTemp))
	{
		int Sock_Num, Accepted;
		Accepted = 0;

		// find a free socket for client
		for(int j=1; j<2; j++)
		{
			if(m_sock[j] == INVALID_SOCKET) 
			{
				Sock_Num = j;
				m_sock[Sock_Num] = accept (m_sock[0],&m_client,&sa_size);
				if (m_sock[Sock_Num]==INVALID_SOCKET)
				{
					printf("Error:  Unable to accept connection!\n");
					WSACleanup ();
					return;
				}
					
				printf("Accepted Client on socket %d\n", j);
				Accepted = 1;
				break;
			}
		}
		float velocity = 3;

		if (Accepted)
		{
			printf("Client has connected!\n");
			sprintf_s (m_packet.Text, TEXTSIZE, "%c",MSG_CONNECTED); //A
			printf ("The Following has been sent -> %c\n", m_packet.Text[0]);
			memcpy(Buffer, &m_packet, sizeof(Packet));
			send (m_sock[Sock_Num], Buffer, BUFFERSIZE, 0);

			FD_SET(m_sock[Sock_Num], &m_master);
			m_numConnections++;

			m_socketIndex = Sock_Num;
			m_connected = true;
		}
		else
		{
			printf("Can't accept any more connections\n");
			// Accept them immediately close the connection
			SOCKET s_tmp = accept (m_sock[0],&m_client,&sa_size);
			closesocket (s_tmp);
		}

	}
			
	// Process all the connection sockets
	for(int i=1;i<2;i++)
	{
		// Does the socket need servicing
		if(FD_ISSET(m_sock[i], &m_fdTemp))
		{	
			int bytes = recv (m_sock[i],Buffer,BUFFERSIZE,0);

			// There has been an error on the socket
			if(bytes == SOCKET_ERROR)
			{
				int SocketError = WSAGetLastError();
				printf("Socket servicing client %d has error:%d, closing the connection\n", i, SocketError);
				FD_CLR(m_sock[i], &m_master);	// remove from master set
				closesocket (m_sock[i]);		// close the connection
				m_sock[i] = 0;				// reset to 0 for reuse
				m_numConnections--;			// decrement number of current connections
			}
				
			// We have received data so process it
			if(bytes > 0)
			{
				memcpy(&m_packet, Buffer, sizeof(Packet));
				printf("Received from client %d -> %s\n", i, m_packet.Text);
				printf("Packet ID = %d\n", m_packet.ID);
				send (m_sock[i],Buffer,BUFFERSIZE,0);
			}
				
			// The client has closed the connection if bytes == 0
			if(bytes == 0)
			{
				printf("The connection to client %d has been closed\n", i);
				FD_CLR(m_sock[i], &m_master);	// remove from master set
				closesocket (m_sock[i]);		// close the connection
				m_sock[i] = 0;				// reset to 0 for reuse
				m_numConnections--;			// decrement number of current connections
			}

		}
	}

	printf("Reached the end\n");
	// Simple way out!
	if(_kbhit())
	{
		_getch();
		return;
	}
}

void Network::SendPacket(Vector3 playerPos, Vector3 paddlePos, Vector3 paddleVel)
{
	static int f = 0;
	f++;

	m_packet.PlayerPosition = playerPos;
	m_packet.Puckposition = paddlePos;
	m_packet.PuckVelocity = paddleVel;

	char Buffer[BUFFERSIZE];
	if(m_numConnections > 0 && f > 1000)
	{
		printf("Client has connected!\n");
		sprintf_s (m_packet.Text, TEXTSIZE, "%c",MSG_CONNECTED); //A
		printf ("The Following has been sent -> %c\n", m_packet.Text[0]);
		memcpy(Buffer, &m_packet, sizeof(Packet));
		send (m_sock[m_socketIndex], Buffer, BUFFERSIZE, 0);
		f = 0;
	}
}