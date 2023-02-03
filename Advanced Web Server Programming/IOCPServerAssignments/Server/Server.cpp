#include "Server.h"

Server::Server()
{
	for (int i = 0; i < maxPlayer*2; ++i)
	{ 
		playerPosition[i] = -1; 
	}
	nMajor = 2;
	nMinor = 2;
	wVersionRequested = MAKEWORD(nMinor, nMajor);
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet != 0) throw "소켓초기화 실패";
	if (LOBYTE(wsaData.wVersion) != nMajor || HIBYTE(wsaData.wVersion) != nMinor) throw "윈도우 소켓 사용불가";
	Data = new ThreadDATA;
}

Server::~Server()
{
	if (Data != nullptr)
	{
		delete Data;
		Data = nullptr;
	}
}

void Server::MakeIOCP()
{
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hIOCP == NULL) throw "IOCP 포트 생성 실패";
}

void Server::ConfirmSystem()
{
	GetSystemInfo(&si);
}

void Server::SetSocket()
{
	serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSock == INVALID_SOCKET) throw "대기 소켓 생성 실패";
}


void Server::SetBind()
{
	ZeroMemory(&clientAddr, sizeof(clientAddr));
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(constant::PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(serverSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) throw "bind에러";
}

void Server::SetListen()
{
	if (listen(serverSock, SOMAXCONN) == SOCKET_ERROR) throw "listen 에러";
}

bool Server::SetAccept()
{
	addrlen = sizeof(clientAddr);
	clientSock = accept(serverSock, (SOCKADDR*)&clientAddr, &addrlen);
	if (clientSock == INVALID_SOCKET) 
		return false;
	return true;
}

void Server::MainLoop()
{
	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; ++i)
	{
		hThread = CreateThread(NULL, 0, ThreadsServer, hIOCP, 0, NULL);
		if (hThread == NULL)
		{
			return;
		}
		CloseHandle(hThread);
	}

	while (1)
	{
		ZeroMemory(buffer, maxPlayer*2+1);

		if (count > maxPlayer) {
			Sleep(999);
		}
		else {
			if (!SetAccept()) //연결 실패했다면
				break;
			++count;

			//입장 데이터
			cout << "[TCP 서버] 클라이언트 접속 : IP 주소 = " << inet_ntoa(clientAddr.sin_addr) << ", 포트 번호 = " << ntohs(clientAddr.sin_port) << endl;
			cout << "현재 접속한 클라이언트 : " << count << endl;

			for (int i = 0; i < maxPlayer; ++i)
			{
				if (!playerConnect[i])
				{
					clientID = i; 
					break;
				}
			}

		
			playerPosition[clientID * 2] = 0;
			playerPosition[clientID * 2 + 1] = 0;

			memcpy(buffer, playerPosition, maxPlayer*2);

			buffer[maxPlayer*2] = clientID;

			retval = send(clientSock, buffer, maxPlayer*2+1, 0);
			if (retval == SOCKET_ERROR)
			{
				throw "전송에러";
				break;
			}

			playerConnect[clientID] = TRUE;
			playerSocket[clientID] = clientSock;
			
			//플레이어 위치 보내주기
			sendPlayerPosition();

			// 소켓과 IOCP 연결
			CreateIoCompletionPort((HANDLE)clientSock, hIOCP, clientSock, 0);

			ptr = new SOCKETDATA;
			if (ptr == NULL)
			{
				break;
			}

			// 소켓 정보 구조체 초기화
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->sock = clientSock;
			ptr->recvbytes = ptr->sendbytes = 0;
			ptr->wsabuffer.buf = ptr->buffer;
			ptr->wsabuffer.len = 512;

			// 비동기 입출력 시작 ( WSARecv 호출 )
			flag = 0;
			retval = WSARecv(clientSock, &ptr->wsabuffer, 1, &recvBytes, &flag, &ptr->overlapped, NULL);
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					throw "비동기 전달 에러";
					continue;
				}
			}

			//스레드 데이터 넘기기
			

			Data->hIOCP = hIOCP;
			Data->count = &count;
			Data->ptr = ptr;
			Data->playerConnect = playerConnect;
			Data->playerSocket = playerSocket;
			Data->playerPosition = playerPosition;

			//hThread = CreateThread(NULL, 0, ThreadsServer, Data, 0, NULL);
			Sleep(1);//제어권좀 넘겨
		}
	}
}


bool Server::sendPlayerPosition()
{
	int retval;
	for (int i = 0; i < maxPlayer; ++i) {
		if (playerConnect[i]) 
		{
			retval = send(playerSocket[i], playerPosition, maxPlayer*2, 0);
			if (retval == SOCKET_ERROR) 
				return false;
		}
	}
	return true;
}

void Server::CheckIP()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	memset(hostname, 0, sizeof(hostname));
	memset(ipaddr, 0, sizeof(ipaddr));

	int error = gethostname(hostname, sizeof(hostname));
	if (error == 0)
	{
		hostinfo = gethostbyname(hostname);
		strcpy(ipaddr, inet_ntoa(*(struct in_addr*)hostinfo->h_addr_list[0]));
		//strcpy(ipaddr, "127.0.0.1");
	}
	WSACleanup();
	cout << "이 컴퓨터의 IP 주소 : " << ipaddr << endl;
}
