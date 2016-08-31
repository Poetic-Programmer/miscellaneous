#include "Network.h"
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <queue>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "Game.h"
using namespace std;


Network::Network()
{
	m_numConnections = 0;
	m_sock = INVALID_SOCKET;
	m_connected = false;
}

Network::~Network()
{
	closesocket(m_sock);
	WSACleanup();
	cout << endl;
}

void Network::Initialize()
{
	ZeroMemory(&m_wsaData, sizeof(WSADATA)); 
	StartWinSock();

	FD_ZERO(&Master);			    // Clear the sets
	FD_ZERO(&fd_read);
	FD_SET(m_sock, &Master);		// Put listening socket in the Master set

	m_timeout.tv_sec=5;			    // or until some activity
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

void Network::SetUpClientConnection()
{
	char Buffer[BUFFERSIZE];
	char ServerIP[20];

	printf("Input the server IP address (return for default): ");
	gets_s(ServerIP);

	m_sock = socket (AF_INET,SOCK_STREAM,0);
	m_client.sin_family = AF_INET;
	m_client.sin_port = htons (SERVERPORT);

	if(strlen(ServerIP) == 0)	m_client.sin_addr.s_addr = inet_addr (SERVERIP);
	else m_client.sin_addr.s_addr = inet_addr (ServerIP);

	printf("The IP address being connected to is: ");
	printf("%d.", (int)m_client.sin_addr.S_un.S_un_b.s_b1);
	printf("%d.", (int)m_client.sin_addr.S_un.S_un_b.s_b2);
	printf("%d.", (int)m_client.sin_addr.S_un.S_un_b.s_b3);
	printf("%d\n", (int)m_client.sin_addr.S_un.S_un_b.s_b4);

	//notice the different byte order for network
	u_short PortNo;
	PortNo = (m_client.sin_port >> 8)|(m_client.sin_port << 8);

	printf("The PORT being connected to is: ");
	printf("%d\n\n", (int)PortNo);

	int size = sizeof(sockaddr);
	if (connect(m_sock,(LPSOCKADDR)&m_client,size) == SOCKET_ERROR)
	{
		printf("Error:  Unable to connect to server!\n");
	    return;
	}
		
	printf("Connected to server\n");

	//wait for a connection message from server
	recv (m_sock,Buffer, BUFFERSIZE,0);
	memcpy(&m_packet, Buffer, sizeof(Packet));

	//gets_s(m_packet.Text);
	//m_packet.ID = 12345;
	if(m_packet.Text[0] == MSG_CONNECTED)
	{
		printf("A valid connection message has been recieved - ie -> %f\n", m_packet.Text[0]);
		m_connected = true;
	}
	else
	{
		printf("An invalid connection message has been recieved - ie -> %f\n", m_packet.Text[0]);
	    printf("exiting\n");
		closesocket(m_sock);
		return;
	}

	FD_ZERO(&Master);			// Clear the sets
	FD_ZERO(&fd_read);
	FD_SET(m_sock, &Master);

	m_timeout.tv_sec=0;			// or until some activity
	m_timeout.tv_usec=1000;
}

bool Network::RecievePacket()
{
	// Set simple counter to control packet flow
	static int f = 0;
	f++;
	fd_read = Master;

	// Look for socket activity
	select(0, &fd_read, NULL, NULL, &m_timeout);

	if(FD_ISSET(m_sock, &fd_read))
	{
		if(f > 1000)
		{
			char Buffer[BUFFERSIZE];
			recv (m_sock, Buffer, BUFFERSIZE,0);
			memcpy(&m_packet, Buffer, sizeof(Packet));
			f = 0;
			cout << "Player Position: <" << m_packet.PlayerPosition.x << ", " << m_packet.PlayerPosition.y << ", " << m_packet.PlayerPosition.z << ">" << endl;

			// Check if connection dropped
			if(m_packet.Text[0] != MSG_CONNECTED)
			{
				m_connected = false;
				return false;
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}