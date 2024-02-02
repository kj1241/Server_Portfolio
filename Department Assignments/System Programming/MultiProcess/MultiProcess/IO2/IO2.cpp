/*
메인 프로세스
*/

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#define SLOT_NAME _T("\\\\.\\mailslot\\mailbox")
#define SLOT_NAME2 _T("\\\\.\\mailslot\\mailbox2")

/*이 주소를 이용하여 분리된 컴퓨터간에서 프로세스간 통신가능 하지만 실제로는 거의 이용되지 않는다*/

DWORD _tmain(int argc, int *argv[])
{
	HANDLE hMailSlot;  //mailslot 핸들
	HANDLE hMailSlot2;
	TCHAR messageBox[50] = {};//메세지 박스 버퍼 크기
	DWORD bytesRead;  //크기.
	TCHAR save_File[10][50] = {};
	DWORD pc = 0;
	
	// mailslot 생성 
	hMailSlot = CreateMailslot(
		SLOT_NAME,
		0, //메일슬롯 버퍼크기
		MAILSLOT_WAIT_FOREVER,
		NULL);

	hMailSlot2 = CreateFile(SLOT_NAME2, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	//생성 못할시 예외처리
	if (hMailSlot == INVALID_HANDLE_VALUE)
	{
		_fputts(_T("Unable to create mailslot!\n"), stdout);
		return 1;
	}


	/* Message 수신 */
	_fputts(_T("Message\n"), stdout);
	while (pc!=10)
	{
		
		if (!ReadFile(hMailSlot, messageBox, sizeof(TCHAR) * 50, &bytesRead, NULL))
		{
			_fputts(_T("Unable to read!\n"), stdout);
			if(hMailSlot!= NULL)
				CloseHandle(hMailSlot);
			return 1;
		}


		if (!_tcsncmp(messageBox, _T("exit\n"), 4))
		{
			_fputts(_T("Good Bye!\n"), stdout);
			break;
		}

		if(bytesRead==0)
			messageBox[bytesRead / sizeof(TCHAR)] = '0'; //NULL 문자 삽입.
		_fputts(messageBox, stdout);
		
		//save_File[pc][50] = *messageBox; //에러 일으키는 원인 이렇게 쓰면 안됨
		memcpy(save_File[pc], messageBox, bytesRead);

		if (!WriteFile(hMailSlot2, messageBox, _tcslen(messageBox) * sizeof(TCHAR), &bytesRead, NULL))
		{
			if (hMailSlot2 != NULL)
			{
				_fputts(_T("Unable to write chat!\n"), stdout);
				CloseHandle(hMailSlot2);
			}
		}


		pc = pc + 1;
	}
	if (hMailSlot != NULL)
		CloseHandle(hMailSlot);
	
	return 0;
}



