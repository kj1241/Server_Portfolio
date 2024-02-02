#include "stdafx.h"
#include "Thread.h"

Threads *Threads::pThreads = nullptr;
ThreadDATA* Threads::Data = nullptr;

Threads::Threads()
{
	pThreads = this;
}

Threads::~Threads()
{
	
}

DWORD __stdcall Threads::ThreadsFunction(LPVOID arg)
{
	ZeroMemory(recvBuffer, 512);
	//Data = (ThreadDATA*)arg;
	HANDLE hIOCP = (HANDLE*)arg;
	while (1)
	{
		// WSARecv
		retval = GetQueuedCompletionStatus(hIOCP, &cbTransferred, (LPDWORD)&client_sock, (LPOVERLAPPED*)&Data->ptr, INFINITE);

		int addrlen = sizeof(clientAddr);
		getpeername(Data->ptr->sock, (SOCKADDR*)&clientAddr, &addrlen);

		if (retval == 0 || cbTransferred == 0) 
		{
			if (retval == 0) 
			{
				DWORD temp1, temp2;
				WSAGetOverlappedResult(Data->ptr->sock, &Data->ptr->overlapped, &temp1, FALSE, &temp2);
				//throw "WSAGetOverlappedResult ����"; //�̷��� ������ throw �ƹ��������� ������ ���Ͽ��� �����Ѽ� ����
				cout << endl <<"WSAGetOverlappedResult ���� ����" << endl; //ǥ�ø�������
			}
			closesocket(client_sock);
			clientDisconnect();

			cout << endl << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientAddr.sin_addr) << ", ��Ʈ ��ȣ = " << ntohs(clientAddr.sin_port) << endl;
			cout << "���� ������ Ŭ���̾�Ʈ : "<< *(Data->count) << endl;
			if (Data->ptr != nullptr)
			{
				delete Data->ptr;
				Data->ptr = nullptr;
			}
			break;
		}


		// �����͸� ������
		
		Data->ptr->recvbytes = 0;
		ZeroMemory(&Data->ptr->overlapped, sizeof(Data->ptr->overlapped));
		Data->ptr->wsabuffer.buf = Data->ptr->buffer;
		Data->ptr->wsabuffer.len = 512;
		DWORD recvBytes;
		DWORD flag=0;

		retval = WSARecv(Data->ptr->sock, &Data->ptr->wsabuffer, 1, &recvBytes, &flag, &Data->ptr->overlapped, NULL);
		if (retval == SOCKET_ERROR) 
		{
			if (WSAGetLastError() != WSA_IO_PENDING) 
			{ 
				throw "recv �ޱ� ����";
			}
			continue;
		}
		else 
		{
			memcpy(recvBuffer, Data->ptr->wsabuffer.buf, Data->ptr->wsabuffer.len);
			playerID = recvBuffer[2];

			switch (recvBuffer[1])
			{
			case -1:
				break;
			case 0:
				control(recvBuffer);
				break;
			default:
				break;
			}
		}

		// ��ȭ�� ���� �� ��� ���Ͽ� �˷��ֱ�
		sendPlayerPosition();
		Sleep(10);//����ǳѱ��
	}
	return 0;
}

DWORD __stdcall Threads::ThreadsServer(LPVOID arg)
{
	return pThreads->ThreadsFunction(arg);
}

void Threads::clientDisconnect()
{
	--(*Data->count);
	char key;

	//��������� Ȯ��
	for (int i = 0; i < maxPlayer; ++i) {
		if (Data->playerConnect[i]) {
			int retval = send(Data->playerSocket[i], Data->playerPosition, maxPlayer*2, 0);
			if (retval == SOCKET_ERROR) 
			{ 
				key = i; 
				break; 
			}
		}
	}

	// �ش� �ּ� ������ �������� Ȯ��
	Data->playerConnect[key] = FALSE;

	for (int i = 0; i < maxPlayer; ++i) {
		if (!Data->playerConnect[i]) {
			Data->playerPosition[i * 2] = -1;
			Data->playerPosition[i * 2 + 1] = -1;
		}
	}

	sendPlayerPosition();
}

void Threads::sendPlayerPosition() {
	int retval;
	for (int i = 0; i < maxPlayer; ++i) {
		if (Data->playerConnect[i]) {
			retval = send(Data->playerSocket[i], Data->playerPosition, maxPlayer*2, 0);
			if (retval == SOCKET_ERROR) 
			{ 
				throw "������ ����";
				exit(1); 
			}
		}
	}
}

void Threads::control(const char buf[])
{
	int key = buf[2];
	switch (buf[3])
	{
	case 72: //up
		if (Data->playerPosition[key * 2] > 0)
		{ 
			--Data->playerPosition[key * 2];
		}
		break;
	case 80: //down
		if (Data->playerPosition[key * 2] < 7)
		{ 
			++Data->playerPosition[key * 2];
		}
		break;
	case 75: //left
		if (Data->playerPosition[key * 2 + 1] > 0)
		{ 
			--Data->playerPosition[key * 2 + 1];
		}
		break;
	case 77://right
		if (Data->playerPosition[key * 2 + 1] < 7)
		{ 
			++Data->playerPosition[key * 2 + 1];
		}
		break;
	case 27: //esc
		break;
	default:
		break;
	}

}
