#include "Sever.h"

Sever::Sever()
{
	addrlen = 0;
	hMutex = CreateMutex(NULL, FALSE, NULL);
	ZeroMemory(greetMessage, sizeof(greetMessage));
	if (&hMutex == NULL) throw "뮤택스 만들기 실패";
	nMajor = 2;
	nMinor = 2;
	wVersionRequested = MAKEWORD(nMinor, nMajor);
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet != 0) throw "소켓초기화 실패";
	if (LOBYTE(wsaData.wVersion) != nMajor || HIBYTE(wsaData.wVersion) != nMinor) throw "윈도우 소켓 사용불가";

}

void Sever::SetSocket()
{
	serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSock == INVALID_SOCKET) throw "대기 소켓 생성 실패";
}

void Sever::SetBind()
{
	ZeroMemory(&clientAddr, sizeof(clientAddr));
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(constant::PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(serverSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) throw "bind에러";
}

void Sever::SetListen()
{
	if (listen(serverSock, SOMAXCONN) == SOCKET_ERROR) throw "listen 에러";
}

void Sever::SetAccept()
{
	addrlen = sizeof(clientAddr);
	clientSock = accept(serverSock, (SOCKADDR*)&clientAddr, &addrlen);
	if (clientSock == INVALID_SOCKET) throw "accpet 에러";
}





void Sever::MainLoop()
{
	while (1)
	{
		SetAccept();
		WaitForSingleObject(hMutex, INFINITE);
		++clientNumber;
		cout << "새로운 클라이언트 연결" << endl;
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
