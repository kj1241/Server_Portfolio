#ifndef CLIENT_H
#define CLIENT_H

#include "stdafx.h"
#include "Thread.h"
class Client:public Thread
{
	SOCKET clientSock;
	WSADATA wsData;
	SOCKADDR_IN serverAddr;

	int GetBufSize;
	char Buffer[512];
	char serverIP[32];
	int retval;
	unsigned char ID={0};
	
public:
	Client();
	~Client();
	void initSocket();
	void connet();
	void mainLoop();
};

#endif