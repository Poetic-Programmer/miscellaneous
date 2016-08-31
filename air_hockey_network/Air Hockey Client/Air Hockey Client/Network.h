#ifndef NETWORK_H
#define NETWORK_H

#include <WinSock2.h>
#include <list>
#include <map>
#include <memory>
#include "Vector2.h"
#include "Vector3.h"

#define SERVERIP "127.0.0.1"

//the port of the server to connect to
#define SERVERPORT 5555
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
	void SetUpClientConnection();
	bool RecievePacket();

	SOCKET GetSocket() {return m_sock;}
	bool IsConnected() {return m_connected;}
	Packet GetPacket() {return m_packet;}
protected:
	void StartWinSock();

protected:
	Packet m_packet;
	fd_set Master, fd_read;

	sockaddr_in m_server;
	sockaddr_in m_client;
	WSADATA     m_wsaData;
	timeval     m_timeout;
	int         m_numConnections;
	SOCKET      m_sock;
	bool        m_connected;
	int         m_socketIndex;
};


#endif // Network.h