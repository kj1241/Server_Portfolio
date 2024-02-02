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

		GetBufSize = recv(clientSock, Buffer, sizeof(Buffer), 0);
		Buffer[GetBufSize] = '\0';
		recvLoginPacket = (Login_data*)Buffer;

		memcpy(Session::UserId[clientNumber - 1], recvLoginPacket->id, sizeof(recvLoginPacket->id));
		memcpy(Session::Password[clientNumber - 1], recvLoginPacket->password, sizeof(recvLoginPacket->password));
		//memcpy(recvLoginPacket->message, NULL, sizeof(recvLoginPacket->message));

		cout << "접속ID :" << Session::UserId[clientNumber - 1] << endl;
		cout << "접속Password :" << Session::Password[clientNumber - 1] << endl;

		send(clientSock, (char*)recvLoginPacket, sizeof(Login_data), 0);

		GetBufSize = recv(clientSock, Buffer, sizeof(Buffer), 0);
		Buffer[GetBufSize] = '\0';
		recvLoginPacket = (Login_data*)Buffer;

		recvLoginPacket->isconnet = true;

		//	if (recvLoginPacket->isconnet == true)
		//	{
		cout << "ID: " << recvLoginPacket->id << endl;
		cout << "접속허가완료: " << recvLoginPacket->message << endl;
		cout << endl;

		memcpy(recvLoginPacket->message, "연결", sizeof(recvLoginPacket->message));

		for (int i = 0; i < clientNumber; ++i)
		{
			if (allClientSocket[i] != clientSock) //자기자신이아니라면
			{
				sendMessagePacket->packetType = connectPacket;
				sendMessagePacket->packetSize = sizeof(message_data);
				memcpy(sendMessagePacket->id, recvLoginPacket->id, sizeof(sendMessagePacket->id));
				memcpy(sendMessagePacket->message, "연결되었습니다.", sizeof(sendMessagePacket->message));
				sendMessagePacket->flag = 1;
				sendMessagePacket->endMark = '\0';
				send(allClientSocket[i], (char*)sendMessagePacket, sizeof(message_data), 0);
			}
			else //자기자신이라면
			{
				sendLoginPacket->packetType = connectPacket;
				sendLoginPacket->packetSize = sizeof(message_data);
				memcpy(sendLoginPacket->id, recvLoginPacket->id, sizeof(sendMessagePacket->id));
				memcpy(sendLoginPacket->message, "연결되었습니다.", sizeof(sendMessagePacket->message));
				sendLoginPacket->isconnet = true;
				sendLoginPacket->endMark = '\0';
				send(clientSock, (char*)sendLoginPacket, sizeof(Login_data), 0);
			}
		}

		_beginthreadex(NULL, 0, &broadcastAllClient, (void*)clientSock, 0, &threadID);
		//	}
		//	else// (recvLoginPacket->isconnet == false)
		//	{
		//		--clientNumber;
			//	return;
		//	}
	}
}


	

Sever::~Sever()
{

	closesocket(serverSock);
	WSACleanup();
	CloseHandle(hMutex);
}
