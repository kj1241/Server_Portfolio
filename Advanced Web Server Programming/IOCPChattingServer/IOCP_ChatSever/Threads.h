#pragma once

class Threads
{
public:
	Threads();
	~Threads();
	unsigned __stdcall CommThread(void* pArguments); // 통신 처리 쓰레드 communication
	unsigned __stdcall InnerCommThread(void);

	static unsigned __stdcall communicationThread(void* pArguments);

	static Threads *pThreads;
protected:
	
private:
	void OnAccept(SOCKET commsock, char* AcceptBuf); //접속처리
	void AddString(HWND hDlg, UINT nID, char* msg); //문자열 더하기
	void OnRead(OVERLAPPEDSOCK* pMOV, char* buf, int nRead); //채팅 문자 받기
	void OnClose(OVERLAPPEDSOCK* pMOV);
	
	THREADDATA* threadData;

	long *threadCount;
	HWND hDlg;
	HANDLE hIOCP;
	SOCKET lstnsock;
	char* acceptBuf;
	ACCEPTOVERLAPPED AOV;
	list<OVERLAPPEDSOCK*> listOverlapped;
	CRITICAL_SECTION cs;
};