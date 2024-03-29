#include "Client.h"

Client::Client()
{
	for (int i = 0; i < maxPlayer*2; ++i)
	{
		playerPosition[i] = -1;
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) throw "���� �ʱ�ȭ ����";
}

Client::~Client()
{
	closesocket(clientSock);
	WSACleanup();
}

void Client::initSocket()
{
	clientSock = socket(PF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET) throw "���� ����";
}

void Client::connet()
{
	cout << "Example = 127.0.0.1" << endl << "���� IP�� �Է��ϼ��� : ";
	cin >> serverIP;

	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(constant::PORT);
	serverAddr.sin_addr.s_addr = inet_addr(serverIP);
	retval = connect(clientSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR)  throw "���� ����()";
}

void Client::mainLoop()
{
	retval = recvn(clientSock, Buffer, maxPlayer*2+1, 0);
	if (retval == SOCKET_ERROR) throw "������ �ޱ� ����";
	memcpy(playerPosition, Buffer, maxPlayer*2);
	
	ID = Buffer[maxPlayer*2];

	//thread.h �����ϱ�� ������
	std::thread Draw = std::thread(drawServer);
	std::thread Send = std::thread(sendDataSever, clientSock, ID);
	std::thread Recv = std::thread(recvDataSever, clientSock);

	Draw.join();
	Send.join();
	Recv.join();
}



