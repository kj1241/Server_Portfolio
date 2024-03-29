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
		system("cls"); //�ܼ� �θ���
		cout << "���� = �����" << endl << "���� = ESC" << endl << endl;

		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j) {
				if (board[i][j] == 0) 
				{ 
					cout << "��";
				}
				else if (board[i][j] == 1) 
				{ 
					cout << "��";
				}
				else if (board[i][j] == 2) 
				{ 
					cout << "��";
				}
				else {}
			}
			cout << endl;
		}

		//�����̰� ����
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
		Buffer[temp] = getch();//Ű���� �����ޱ�
		if (Buffer[temp] == 27) //esc Ű������
		{
			exit(1);
		}

		retval = send(s, Buffer, temp + 1, 0); //Ű���� Ű ����
		if (retval == SOCKET_ERROR)
		{
			throw((char*)"���� ����");
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
			throw((char*)"�ޱ� ����");
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
		received = recv(s, ptr, left, flags); //���� �ޱ�
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;  //������ ������
		else if (received == 0)
			break; //���� ������ �������
		left -= received;
		ptr += received;
	}

	return (len - left);

}