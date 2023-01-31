#ifndef  THREAD_H
#define THREAD_H

#pragma comment(lib,"ws2_32")
#include <iostream>
#include <WinSock2.h>
#include <tchar.h>
#include <cstdio>
#include <process.h>

using namespace std;

class Threads
{
protected:
	enum constant
	{
		PORT = 8080,
		BUFFERSIZE = 255,
		NAMESIZE = 10
	};
	unsigned threadID;
	HANDLE hMutex;
    static int allClientSocket[100];
    static int clientNumber;

public :
	Threads();

	void threadsFunction(void *arg);
	static unsigned __stdcall broadcastAllClient(void *arg);
	
	~Threads();

};







#endif

