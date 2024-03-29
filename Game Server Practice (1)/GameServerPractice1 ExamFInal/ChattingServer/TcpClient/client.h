#ifndef CLIENT_H
#define CLIENT_H

#pragma comment(lib,"ws2_32")
#include <iostream>
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <stdio.h>
#include <process.h>
#include <cstdio>
#include <iostream>
#include <process.h>
#include <cstdio>
#include "Thread.h"
using namespace std;

class Client:public Thread
{
	
	
	unsigned long hSendThread;
	unsigned long hRecvThread;
	unsigned sendThreadID;
	unsigned recvThreadID;
	SOCKET clientSock;
	WSADATA wsData;
	SOCKADDR_IN serverAddr;

public :
	Client();
	void initSocket();
	void connet();
	void mainLoop();

};


#endif // !CLIENT_H
