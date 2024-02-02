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
	void draw(); //보드판 그리기
	void sendData(const SOCKET& s, char ID);//데이타 보내기
	void recvData(const SOCKET& s);//데이터 서버로부터 받기

	static Thread* pThreads;

	//쓰레드 만들기위해서 껍데기 
	static unsigned __stdcall drawServer();
	static unsigned __stdcall sendDataSever(const SOCKET& s, char ID);
	static unsigned __stdcall recvDataSever(const SOCKET& s);



};
#endif