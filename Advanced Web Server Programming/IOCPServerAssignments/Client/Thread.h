#ifndef THREAD_H
#define THREAD_H

#include "stdafx.h"
class Thread
{
protected:
	CHAR playerPosition[maxPlayer*2] = { 0 };
	char board[8][8]=
	{
		{ 1, 0, 1, 0, 1, 0, 1, 0 },
		{ 0, 1, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 0, 1, 0 },
		{ 0, 1, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 0, 1, 0 },
		{ 0, 1, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 0, 1, 0 },
		{ 0, 1, 0, 1, 0, 1, 0, 1 }
	};

	int recvn(SOCKET s, char* Buffer, int len, int flags);

public:
	Thread();
	~Thread();
	void draw(); //������ �׸���
	void sendData(const SOCKET& s, char ID);//����Ÿ ������
	void recvData(const SOCKET& s);//������ �����κ��� �ޱ�

	static Thread* pThreads;

	//������ ��������ؼ� ������ 
	static unsigned __stdcall drawServer();
	static unsigned __stdcall sendDataSever(const SOCKET& s, char ID);
	static unsigned __stdcall recvDataSever(const SOCKET& s);



};
#endif