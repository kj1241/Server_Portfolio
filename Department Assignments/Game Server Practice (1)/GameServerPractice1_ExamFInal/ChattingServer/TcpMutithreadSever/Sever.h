#ifndef  SEVER_H
#define SEVER_H

#pragma comment(lib,"ws2_32")
#include <iostream>
#include <WinSock2.h>
#include <tchar.h>
#include <cstdio>
#include <process.h>
#include "Thread.h"
using namespace std;

class Sever:public Threads
{
private:

	HANDLE hThread;
	unsigned threadID;
	int addrlen;
	HANDLE hMutex;
	char greetMessage[constant::BUFFERSIZE];

	WSADATA wsaData;
	BYTE nMajor, nMinor;
	WORD wVersionRequested;
	int nRet;
	SOCKET serverSock;
	SOCKET clientSock;
	SOCKADDR_IN serverAddr;
	SOCKADDR_IN clientAddr;

public :
	Sever();
	void SetSocket();
	void SetBind();
	void SetListen();
	void SetAccept();

	void MainLoop();
	~Sever();
};

#endif // ! SEVER_H