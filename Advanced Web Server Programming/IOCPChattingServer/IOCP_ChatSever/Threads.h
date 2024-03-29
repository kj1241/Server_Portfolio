#pragma once

class Threads
{
public:
	Threads();
	~Threads();
	unsigned __stdcall CommThread(void* pArguments); // ��� ó�� ������ communication
	unsigned __stdcall InnerCommThread(void);

	static unsigned __stdcall communicationThread(void* pArguments);

	static Threads *pThreads;
protected:
	
private:
	void OnAccept(SOCKET commsock, char* AcceptBuf); //����ó��
	void AddString(HWND hDlg, UINT nID, char* msg); //���ڿ� ���ϱ�
	void OnRead(OVERLAPPEDSOCK* pMOV, char* buf, int nRead); //ä�� ���� �ޱ�
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