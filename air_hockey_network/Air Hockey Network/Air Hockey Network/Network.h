#ifndef NETWORK_H
#define NETWORK_H

#include <WinSock2.h>
#include <list>
#include <map>
#include <memory>
#include "Vector2.h"
#include "Vector3.h"

#define MSG_CONNECTED	(0x41)	//A
#define TEXTSIZE		(80)
#define BUFFERSIZE		(500)
#define NUMCONN			(4)
#define PORTNUM			(5555)

struct Packet
{
	int ID;
	Vector3 PuckVelocity;
	Vector3 Puckposition;
	Vector3 PlayerPosition;
	char Text[TEXTSIZE];

	Packet()
	{
		ID = 1234;
	}
};

class Network
{
public:
	Network();
	~Network();

	void Initialize();
	void Update();
	void SetUpServer();
	void SendPacket(Vector3 playerPos, Vector3 paddlePos, Vector3 paddleVel);
	void ListenForConnections();
protected:

	Packet m_packet;
	fd_set m_master, m_fdTemp;

	void StartWinSock();
	sockaddr_in m_server;
	sockaddr m_client;
	WSADATA m_wsaData;
	timeval m_timeout;
	int m_numConnections;
	SOCKET * m_sock;
	bool m_connected;
	int m_socketIndex;
};


#endif // Network.h