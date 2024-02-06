#include "Sever.h"

Sever::Sever()
{
	addrlen = 0;
	hMutex = CreateMutex(NULL, FALSE, NULL);
	ZeroMemory(greetMessage, sizeof(greetMessage));
	if (&hMutex == NULL) throw "���ý� ����� ����";
	nMajor = 2;
	nMinor = 2;
	wVersionRequested = MAKEWORD(nMinor, nMajor);
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet != 0) throw "�����ʱ�ȭ ����";
	if (LOBYTE(wsaData.wVersion) != nMajor || HIBYTE(wsaData.wVersion) != nMinor) throw "������ ���� ���Ұ�";

}

void Sever::SetSocket()
{
	serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSock == INVALID_SOCKET) throw "��� ���� ���� ����";
}

void Sever::SetBind()
{
	ZeroMemory(&clientAddr, sizeof(clientAddr));
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(constant::PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(serverSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) throw "bind����";
}

void Sever::SetListen()
{
	if (listen(serverSock, SOMAXCONN) == SOCKET_ERROR) throw "listen ����";
}

void Sever::SetAccept()
{
	addrlen = sizeof(clientAddr);
	clientSock = accept(serverSock, (SOCKADDR*)&clientAddr, &addrlen);
	if (clientSock == INVALID_SOCKET) throw "accpet ����";
}





void Sever::MainLoop()
{
	while (1)
	{
		SetAccept();
		WaitForSingleObject(hMutex, INFINITE);
		++clientNumber;
		cout << "���ο� Ŭ���̾�Ʈ ����" << endl;
		allClientSocket[clientNumber - 1] = clientSock;
		ReleaseMutex(hMutex);

		send(clientSock, greetMessage, sizeof(greetMessage), 0);
		_beginthreadex(NULL, 0, &broadcastAllClient, (void *)clientSock, 0, &threadID);
		
	}
}


	

Sever::~Sever()
{
	closesocket(serverSock);
	WSACleanup();
	CloseHandle(hMutex);
}
