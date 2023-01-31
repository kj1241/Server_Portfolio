/*
기말고사 MR.K B277004 김경주 2015.11.19
*/
#include <stdio.h>
#include <tchar.h>
#include <process.h>
#include <windows.h>
#include <fstream>
#include <iostream>

using namespace std;
/*
파일에서 데이터 가지오기
*/


HANDLE hSemaphore;//세마포어 핸들값,
static int total = 0;

DWORD* file_from_data(const TCHAR *adress)
{
	if (!adress)
	{
		return NULL;
	}
	DWORD *data = (DWORD *)malloc(sizeof(data) * 30);
	
	ifstream fin;

	fin.open("number.txt");
	if (fin.fail())
	{
		return NULL;
	}
	for (int i = 0; i <30; i++)
	{
		fin >> data[i];
	}
	fin.close();
	return data;
}

void* data_print(const DWORD *data)
{
	if (!data) return NULL;
	for (DWORD i = 0; i < 30; i++)
	{
		cout << data[i]<<endl;
	}
}

void crate_smapoher()
{
	hSemaphore = CreateSemaphore(
		NULL,       //
		3,  //세마포어 범위
		3,//세마포어 최대 횟수
		NULL);
	
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	DWORD * nPtr = (DWORD *)lpParam;
	DWORD number[10];
	
	UNREFERENCED_PARAMETER(lpParam);
	DWORD return_smapore;
	BOOL smapore_flog = TRUE;
	
	while (smapore_flog)
	{
		printf("요청 키 : 쓰레드 %d\n", GetCurrentThreadId());
		return_smapore = WaitForSingleObject(hSemaphore, 0L);
	
		switch (return_smapore)
		{
			
		case WAIT_OBJECT_0:
			printf("쓰레드 %d: 완료를 기다리는중\n", GetCurrentThreadId());	
			for (DWORD i = 0; i < 10; i++)
			{
				number[i] = *(nPtr + i);
				total = total + number[i];
				cout << total<<endl;
			}

			smapore_flog = FALSE;

			Sleep(200);
			if (!ReleaseSemaphore(hSemaphore, 1, NULL))
			{
				printf("세마포어 해제 에러: %d\n", GetLastError());
				break;
			}

		case WAIT_TIMEOUT:
			printf("쓰레드 %d: 시간 아웃\n", GetCurrentThreadId());

			Sleep(300);
			break;
		}
		
	}
	
	return TRUE;
}

int  _tmain(int argc, TCHAR* argv[])
{
	TCHAR adress[] = _T("number.txt");
	DWORD *data = file_from_data(adress);
	DWORD dwThreadID[3];
	HANDLE hThread[3];
	
	
	if (data == NULL)
	{
		cout << "파일을 열수 없습니다.";
	}
	else
	{
		data_print(data);
	}

	crate_smapoher();

	
	if (!hSemaphore)
	{
		cout << "세마포어 만들기 실패";
		return -1;
	}
	
	for (int i = 0; i < 3; i++)
	{
		hThread[i] =
			CreateThread(
				NULL,
				0,
				ThreadProc,
				(LPVOID)(data + i*10),
				0,
				&dwThreadID[0]);

		if (hThread[i] == NULL)
		{
			_tprintf(_T("만들수 없습니다. \n"));
			return -1;
		}
	}

	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	_tprintf(_T("합계: (1 ~ 10): %d \n"), total);

	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);
	CloseHandle(hSemaphore);


	return 0;
}
