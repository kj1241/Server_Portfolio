#include "Client.h"

Client::Client()
{
	for (int i = 0; i < maxPlayer*2; ++i)
	{
		playerPosition[i] = -1;
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) throw "소켓 초기화 실패";
}

Client::~Client()
{
	closesocket(clientSock);
	WSACleanup();
}

void Client::initSocket()
{
	clientSock = socket(PF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET) throw "소켓 에러";
}

void Client::connet()
{
	cout << "Example = 127.0.0.1" << endl << "서버 IP를 입력하세요 : ";
	cin >> serverIP;

	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(constant::PORT);
	serverAddr.sin_addr.s_addr = inet_addr(serverIP);
	retval = connect(clientSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR)  throw "연결 에러()";
}

void Client::mainLoop()
{
	retval = recvn(clientSock, Buffer, maxPlayer*2+1, 0);
	if (retval == SOCKET_ERROR) throw "데이터 받기 실패";
	memcpy(playerPosition, Buffer, maxPlayer*2);
	
	ID = Buffer[maxPlayer*2];

	//thread.h 연습하기로 했으니
	std::thread Draw = std::thread(drawServer);
	std::thread Send = std::thread(sendDataSever, clientSock, ID);
	std::thread Recv = std::thread(recvDataSever, clientSock);

	Draw.join();
	Send.join();
	Recv.join();
}



