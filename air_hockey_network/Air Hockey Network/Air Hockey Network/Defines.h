#ifndef _DEFINES_H_
#define _DEFINES_H_


// the ip address of the server to connect to
//#define SERVERIP "169.254.200.70"
#define SERVERIP "127.0.0.1"

//the port of the server to connect to
#define SERVERPORT 5555

#define MSG_CONNECTED	(0x41)	//A
#define TEXTSIZE		(80)
#define BUFFERSIZE		(100)

struct MyPacket_t
{
	int ID;
	char Text[TEXTSIZE];

	float xPosition;
	MyPacket_t()
	{
		ID = 1234;
	}
};



#endif