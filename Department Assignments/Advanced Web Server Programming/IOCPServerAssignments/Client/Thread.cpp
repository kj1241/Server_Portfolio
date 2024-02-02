#include "stdafx.h"
#include "Thread.h"

Thread* Thread::pThreads = nullptr;

Thread::Thread()
{
	pThreads = this;
}

Thread::~Thread()
{
}

void Thread::draw()
{
	while (true)
	{
		for (int i = 0; i < maxPlayer; ++i) {
			if (playerPosition[i * 2] != -1) 
			{
				board[playerPosition[i * 2]][playerPosition[i * 2 + 1]] = 2;
			}
		}
		system("cls"); //콘솔 부르고
		cout << "조작 = ←→↑↓" << endl << "종료 = ESC" << endl << endl;

		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j) {
				if (board[i][j] == 0) 
				{ 
					cout << "▨";
				}
				else if (board[i][j] == 1) 
				{ 
					cout << "▧";
				}
				else if (board[i][j] == 2) 
				{ 
					cout << "●";
				}
				else {}
			}
			cout << endl;
		}

		//움직이고 나면
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j) {
				if ((i + j) % 2 == 0) 
				{ 
					board[i][j] = 1; 
				}
				else 
				{ 
					board[i][j] = 0; 
				}
			}
		}

		Sleep(100);
	}

}

void Thread::sendData(const SOCKET& s, char ID)
{
	char Buffer[512];
	int retval = 0, temp;
	int cmd = -1;

	ZeroMemory(Buffer, 512);

	temp = 3;
	Buffer[0] = 1;
	Buffer[1] = 0;
	Buffer[2] = ID;

	while (1)
	{
		Buffer[temp] = getch();//키보드 내려받기
		if (Buffer[temp] == 27) //esc 키누르면
		{
			exit(1);
		}

		retval = send(s, Buffer, temp + 1, 0); //키보드 키 전송
		if (retval == SOCKET_ERROR)
		{
			throw((char*)"전송 에러");
			exit(1);
		}

	}
}

void Thread::recvData(const SOCKET& s)
{
	char Buffer[512];
	int retval;

	ZeroMemory(Buffer, 512);

	while (1)
	{
		retval = recvn(s, Buffer, maxPlayer*2, 0);
		if (retval == SOCKET_ERROR) 
		{
			throw((char*)"받기 에러");
			exit(1);
		}

		memcpy(playerPosition, Buffer, maxPlayer*2);
		Sleep(50);
	}
}

unsigned __stdcall Thread::drawServer()
{
	pThreads->draw();
	return 0;
}

unsigned __stdcall Thread::sendDataSever(const SOCKET& s, char ID)
{
	pThreads->sendData(s, ID);
	return 0;
}

unsigned __stdcall Thread::recvDataSever(const SOCKET& s)
{
	pThreads->recvData(s);
	return 0;
}


int Thread::recvn(SOCKET s, char* Buffer, int len, int flags)
{
	int received;
	char* ptr = Buffer;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags); //정보 받기
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;  //에러면 나가고
		else if (received == 0)
			break; //값이 없으면 끝내면됨
		left -= received;
		ptr += received;
	}

	return (len - left);

}