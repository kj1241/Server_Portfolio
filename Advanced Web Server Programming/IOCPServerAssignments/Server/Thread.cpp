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
				//throw "WSAGetOverlappedResult 에러"; //이렇게 무작정 throw 아무생각없이 던지면 소켓에러 일으켜서 망함
				cout << endl <<"WSAGetOverlappedResult 강제 종료" << endl; //표시만해주자
			}
			closesocket(client_sock);
			clientDisconnect();

			cout << endl << "[TCP 서버] 클라이언트 종료 : IP 주소 = " << inet_ntoa(clientAddr.sin_addr) << ", 포트 번호 = " << ntohs(clientAddr.sin_port) << endl;
			cout << "현재 접속한 클라이언트 : "<< *(Data->count) << endl;
			if (Data->ptr != nullptr)
			{
				delete Data->ptr;
				Data->ptr = nullptr;
			}
			break;
		}


		// 데이터를 받을때
		
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
				throw "recv 받기 에러";
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

		// 변화에 따른 값 모든 소켓에 알려주기
		sendPlayerPosition();
		Sleep(10);//제어권넘기기
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

	//연결없는지 확인
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

	// 해당 주소 접속을 끊었음을 확인
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
				throw "보내기 에러";
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
