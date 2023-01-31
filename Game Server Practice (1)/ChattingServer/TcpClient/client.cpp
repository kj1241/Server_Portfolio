#include "client.h"

Client::Client()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) throw "소켓 초기화 실패";
}

void Client::initSocket()
{
	clientSock = socket(PF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET) throw "소켓 에러";
}

void Client::connet()
{
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(constant::PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
}

void Client::mainLoop()
{
	if (connect(clientSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == -1) throw "접속 연결 실패";
	
	hSendThread = _beginthreadex(NULL, 0, &sendMessage, (void*)clientSock, 0, &sendThreadID);
	hRecvThread = _beginthreadex(NULL, 0, &recvMessage, (void*)clientSock, 0, &recvThreadID);
	WaitForSingleObject((HANDLE)hSendThread, INFINITE);
	WaitForSingleObject((HANDLE)hRecvThread, INFINITE);
}

