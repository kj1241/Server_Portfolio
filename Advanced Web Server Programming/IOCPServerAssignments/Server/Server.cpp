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
	if (nRet != 0) throw "�����ʱ�ȭ ����";
	if (LOBYTE(wsaData.wVersion) != nMajor || HIBYTE(wsaData.wVersion) != nMinor) throw "������ ���� ���Ұ�";
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
	if (hIOCP == NULL) throw "IOCP ��Ʈ ���� ����";
}

void Server::ConfirmSystem()
{
	GetSystemInfo(&si);
}

void Server::SetSocket()
{
	serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSock == INVALID_SOCKET) throw "��� ���� ���� ����";
}


void Server::SetBind()
{
	ZeroMemory(&clientAddr, sizeof(clientAddr));
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(constant::PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(serverSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) throw "bind����";
}

void Server::SetListen()
{
	if (listen(serverSock, SOMAXCONN) == SOCKET_ERROR) throw "listen ����";
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
			if (!SetAccept()) //���� �����ߴٸ�
				break;
			++count;

			//���� ������
			cout << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientAddr.sin_addr) << ", ��Ʈ ��ȣ = " << ntohs(clientAddr.sin_port) << endl;
			cout << "���� ������ Ŭ���̾�Ʈ : " << count << endl;

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
				throw "���ۿ���";
				break;
			}

			playerConnect[clientID] = TRUE;
			playerSocket[clientID] = clientSock;
			
			//�÷��̾� ��ġ �����ֱ�
			sendPlayerPosition();

			// ���ϰ� IOCP ����
			CreateIoCompletionPort((HANDLE)clientSock, hIOCP, clientSock, 0);

			ptr = new SOCKETDATA;
			if (ptr == NULL)
			{
				break;
			}

			// ���� ���� ����ü �ʱ�ȭ
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->sock = clientSock;
			ptr->recvbytes = ptr->sendbytes = 0;
			ptr->wsabuffer.buf = ptr->buffer;
			ptr->wsabuffer.len = 512;

			// �񵿱� ����� ���� ( WSARecv ȣ�� )
			flag = 0;
			retval = WSARecv(clientSock, &ptr->wsabuffer, 1, &recvBytes, &flag, &ptr->overlapped, NULL);
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					throw "�񵿱� ���� ����";
					continue;
				}
			}

			//������ ������ �ѱ��
			

			Data->hIOCP = hIOCP;
			Data->count = &count;
			Data->ptr = ptr;
			Data->playerConnect = playerConnect;
			Data->playerSocket = playerSocket;
			Data->playerPosition = playerPosition;

			//hThread = CreateThread(NULL, 0, ThreadsServer, Data, 0, NULL);
			Sleep(1);//������� �Ѱ�
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
	cout << "�� ��ǻ���� IP �ּ� : " << ipaddr << endl;
}
