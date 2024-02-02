#ifndef THREAD_H
#define THREAD_H

#include "stdafx.h"

class Threads
{
	char playerID{ -1 };
	char recvBuffer[512];
	int retval;

	DWORD recvBytes;
	DWORD flag = 0;

	DWORD cbTransferred;
	SOCKET client_sock;
	SOCKADDR_IN clientAddr;

	void clientDisconnect();

	void control(const char buf[]);
	//ThreadDATA* Data;

protected:
	void sendPlayerPosition();
	static ThreadDATA* Data;

public:
	Threads();
	~Threads();
	static Threads* pThreads;
	DWORD WINAPI ThreadsFunction(LPVOID arg);
	static DWORD WINAPI ThreadsServer(LPVOID arg); //������ ���ϱ����� ������ ������ֱ�
};

#endif



