#ifndef  SEVER_H
#define SEVER_H

#include "stdafx.h"
#include "Thread.h"


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


	int GetBufSize;
	char Buffer[100];

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