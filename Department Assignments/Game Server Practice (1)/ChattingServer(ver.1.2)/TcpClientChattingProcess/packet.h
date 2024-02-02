#ifndef PACKET_H
#define PACKET_H
#include "stdafx.h"

struct Header
{
	BYTE packetType;
	BYTE packetSize;
};

struct Login_data : Header
{
	char id[20];
	char password[20];
	char message[40];
	bool isconnet;
	BYTE endMark;
};

struct message_data : Header
{
	char id[20];
	char message[40];
	bool flag;
	BYTE endMark;
};

enum
{
	finishPacket = 0,
	LoginPacket = 1,
	connectPacket = 2,
	chatMessage = 3,
	disConnectPacket = 4
};


#endif