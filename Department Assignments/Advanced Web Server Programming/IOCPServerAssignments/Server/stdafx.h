#pragma once
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
#include <string.h>
#include <string>
#include <conio.h>
#include <thread>

using namespace std;

enum constant
{
	PORT = 9000,
	BUFFERSIZE = 255,
	NAMESIZE = 10
};
#define maxPlayer 10

struct SOCKETDATA
{
	OVERLAPPED overlapped;
	SOCKET sock;
	char buffer[512 + 1];
	int recvbytes;
	int sendbytes;
	WSABUF wsabuffer;
};

struct ThreadDATA
{
	HANDLE hIOCP;
	SOCKETDATA* ptr;
	unsigned int* count;
	bool* playerConnect;
	SOCKET* playerSocket;
	CHAR* playerPosition;
};