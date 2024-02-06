#include "client.h"

Client::Client()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) throw "���� �ʱ�ȭ ����";
}

void Client::initSocket()
{
	clientSock = socket(PF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET) throw "���� ����";
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
	if (connect(clientSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == -1) throw "���� ���� ����";
	
	hSendThread = _beginthreadex(NULL, 0, &sendMessage, (void*)clientSock, 0, &sendThreadID);
	hRecvThread = _beginthreadex(NULL, 0, &recvMessage, (void*)clientSock, 0, &recvThreadID);
	WaitForSingleObject((HANDLE)hSendThread, INFINITE);
	WaitForSingleObject((HANDLE)hRecvThread, INFINITE);
}

