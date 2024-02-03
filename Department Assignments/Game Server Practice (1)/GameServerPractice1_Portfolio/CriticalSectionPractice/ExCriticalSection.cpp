#include <windows.h>
#include <iostream>

#define MAXCNT 100000000
int g_count = 0;
CRITICAL_SECTION cs;

DWORD WINAPI MyThread1(LPVOID arg)
{
	for(int i=0; i<MAXCNT; ++i){
		EnterCriticalSection(&cs);
		g_count+=2;
		LeaveCriticalSection(&cs);
	}
	return 0;
}

DWORD WINAPI MyThread2(LPVOID arg)
{
	for(int i=0; i<MAXCNT; ++i){
		EnterCriticalSection(&cs);
		g_count-=2;
		LeaveCriticalSection(&cs);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	
	InitializeCriticalSection(&cs);// �Ӱ� ���� �ʱ�ȭ
	// �� ���� ������ ����
	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, MyThread1, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, MyThread2, NULL, 0, NULL);
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);	// �� ���� ������ ���� ���
	DeleteCriticalSection(&cs); // �Ӱ� ���� ����

	std::cout << "g_count= " << g_count << "\n";	
	return 0;
}