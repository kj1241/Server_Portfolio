#ifndef SERVER_H
#define SERVER_H

#include "stdafx.h"
#include "Thread.h"

class Server :public Threads
{

	DWORD flag, recvBytes;
	WSADATA wsaData;
	BYTE nMajor, nMinor;
	WORD wVersionRequested;
	int nRet;
	int addrlen;
	int retval;
	SOCKET serverSock;
	SOCKET clientSock;
	SOCKADDR_IN serverAddr;
	SOCKADDR_IN clientAddr;


	SYSTEM_INFO si;
	HANDLE hIOCP;
	
	char buffer[maxPlayer*2+1] = { 0 };
	unsigned char clientID{ 0 };
	
	PHOSTENT hostinfo;
	char hostname[50];
	char ipaddr[50];

	bool sendPlayerPosition();
	HANDLE hThread;

	SOCKETDATA* ptr;
	unsigned int count=0; //접속인원 4명까지
	bool playerConnect[maxPlayer] = {0};
	SOCKET playerSocket[maxPlayer] = {0};
	CHAR playerPosition[maxPlayer*2] = {0};


public:
	Server();
	~Server();
	void CheckIP();
	void MainLoop();
	void MakeIOCP();
	void ConfirmSystem(); //시스템 정보확인
	void SetListen();
	void SetBind();
	void SetSocket();
	bool SetAccept();
};

#endif // ! SEVER_H
