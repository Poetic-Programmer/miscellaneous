#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <conio.h>
#include <iostream>
//#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define MSG_CONNECTED	(0x41)	//A
#define TEXTSIZE		(80)
#define BUFFERSIZE		(100)
#define NUMCONN			(4)

class Server
{
public:
	Server();
	~Server();

private:
};

#endif // Server.h