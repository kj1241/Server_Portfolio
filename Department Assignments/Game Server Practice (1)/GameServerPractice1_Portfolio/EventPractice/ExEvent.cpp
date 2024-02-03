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
			buf[i] = k+1; // 공유 버퍼에 데이터 저장
		SetEvent(hWriteEvent); // 쓰기 완료 알림
	}

	return 0;
}

DWORD WINAPI ReadThread(LPVOID arg)
{
	DWORD retval;

	while(1){
		retval = WaitForSingleObject(hWriteEvent, INFINITE); 	// 쓰기 완료 대기
		if(retval != WAIT_OBJECT_0) break;

		// 읽은 데이터 출력
		std::cout << "Thread " << std::setw(4)<< GetCurrentThreadId() << ": ";
		for (int i = 0; i < BUFSIZE; i++)
			std::cout << std::setw(3) << buf[i];
	
		std::cout << "\n";
		
		ZeroMemory(buf, sizeof(buf));// 버퍼 초기화
		SetEvent(hReadEvent);// 읽기 완료 알림
	}

	return 0;
}

int main(int argc, char *argv[])
{
	// 두 개의 자동 리셋 이벤트 생성(각각 비신호, 신호 상태)
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(hWriteEvent == NULL) return 1;
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if(hReadEvent == NULL) return 1;
	
	// 세 개의 스레드 생성
	HANDLE hThread[3];
	hThread[0] = CreateThread(NULL, 0, WriteThread, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	
	
	WaitForMultipleObjects(3, hThread, TRUE, INFINITE); // 세 개의 스레드 종료 대기

	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);
	return 0;
}