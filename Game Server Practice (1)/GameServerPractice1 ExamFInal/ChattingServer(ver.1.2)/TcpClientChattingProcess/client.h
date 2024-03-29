#ifndef CLIENT_H
#define CLIENT_H

#include "stdafx.h"
#include "Thread.h"
class Client:public Thread
{
	unsigned long hSendThread;
	unsigned long hRecvThread;
	unsigned sendThreadID;
	unsigned recvThreadID;
	SOCKET clientSock;
	WSADATA wsData;
	SOCKADDR_IN serverAddr;
	char greetMessage[constant::BUFFERSIZE];

	int GetBufSize;
	char Buffer[100];

public:
	Client();
	void initSocket();
	void connet();
	void mainLoop();
	~Client();
};

#endif