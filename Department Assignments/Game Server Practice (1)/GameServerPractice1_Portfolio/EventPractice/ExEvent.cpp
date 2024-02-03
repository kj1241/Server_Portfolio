#include <windows.h>
#include <iostream>
#include <IOMANIP>

#define BUFSIZE 10

HANDLE hReadEvent;
HANDLE hWriteEvent;
int buf[BUFSIZE];

DWORD WINAPI WriteThread(LPVOID arg)
{
	DWORD retval;

	for(int k=0; k<500; ++k){
		retval = WaitForSingleObject(hReadEvent, INFINITE);
		if(retval != WAIT_OBJECT_0) break;

		for(int i=0; i<BUFSIZE; i++)
			buf[i] = k+1; // ���� ���ۿ� ������ ����
		SetEvent(hWriteEvent); // ���� �Ϸ� �˸�
	}

	return 0;
}

DWORD WINAPI ReadThread(LPVOID arg)
{
	DWORD retval;

	while(1){
		retval = WaitForSingleObject(hWriteEvent, INFINITE); 	// ���� �Ϸ� ���
		if(retval != WAIT_OBJECT_0) break;

		// ���� ������ ���
		std::cout << "Thread " << std::setw(4)<< GetCurrentThreadId() << ": ";
		for (int i = 0; i < BUFSIZE; i++)
			std::cout << std::setw(3) << buf[i];
	
		std::cout << "\n";
		
		ZeroMemory(buf, sizeof(buf));// ���� �ʱ�ȭ
		SetEvent(hReadEvent);// �б� �Ϸ� �˸�
	}

	return 0;
}

int main(int argc, char *argv[])
{
	// �� ���� �ڵ� ���� �̺�Ʈ ����(���� ���ȣ, ��ȣ ����)
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(hWriteEvent == NULL) return 1;
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if(hReadEvent == NULL) return 1;
	
	// �� ���� ������ ����
	HANDLE hThread[3];
	hThread[0] = CreateThread(NULL, 0, WriteThread, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	
	
	WaitForMultipleObjects(3, hThread, TRUE, INFINITE); // �� ���� ������ ���� ���

	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);
	return 0;
}