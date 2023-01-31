#ifndef THREAD_H
#define THREAD_H


#pragma comment(lib,"ws2_32")
#include <iostream>
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <stdio.h>
#include <process.h>
#include <cstdio>
using namespace std;

class Thread
{
protected:
	enum constant
	{
		PORT = 8080,
		BUFFERSIZE = 255,
		NAMESIZE = 10
	};
	int RemoveEndl(char* buf, int size);

public:
	void sendMessages(void *arg);
	void recvMessages(void *arg);
	

	static unsigned __stdcall sendMessage(void* arg);
	static unsigned __stdcall recvMessage(void* arg);
	



};



#endif