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
	if (connect(clientSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == -1) throw "���ӽ���";
	int mSock = (int)clientSock;
	char finish[2];
	char greedmessage[20];

	cout << "ü�����α׷� ver1.2(�����Ͻ÷��� /bye ������)" << endl;
	cout << "�г����� ��������" << endl;
	cin >> sendLoginPacket->id;

	cout << "��й�ȣ�� ��������" << endl;
	cin >> sendLoginPacket->password;

	sendLoginPacket->isconnet = true;
	sendLoginPacket->packetType = LoginPacket;
	sendLoginPacket->packetSize = sizeof(Login_data);

	send(mSock, (char*)sendLoginPacket, sizeof(Login_data), 0);

	GetBufSize = recv(mSock, Buffer, sizeof(Buffer), 0);
	Buffer[GetBufSize] = '\0';
	recvLoginPacket = (Login_data*)Buffer;


	memcpy(recvLoginPacket->message, "���� �Ǿ����ϴ�.", sizeof(recvLoginPacket->message));
	send(clientSock, (char*)recvLoginPacket, sizeof(Login_data), 0);

	GetBufSize = recv(mSock, Buffer, sizeof(Buffer), 0);
	Buffer[GetBufSize] = '\0';


	hSendThread = _beginthreadex(NULL, 0, &sendMessage, (void*)clientSock, 0, &sendThreadID);
	hRecvThread = _beginthreadex(NULL, 0, &recvMessage, (void*)clientSock, 0, &recvThreadID);
	WaitForSingleObject((HANDLE)hSendThread, INFINITE);
	WaitForSingleObject((HANDLE)hRecvThread, INFINITE);

}

Client::~Client()
{
	closesocket(clientSock);
	WSACleanup();
	//CloseHandle(hMutex);

}