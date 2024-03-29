/*
���� ���μ���
*/

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#define SLOT_NAME _T("\\\\.\\mailslot\\mailbox")
#define SLOT_NAME2 _T("\\\\.\\mailslot\\mailbox2")

/*�� �ּҸ� �̿��Ͽ� �и��� ��ǻ�Ͱ����� ���μ����� ��Ű��� ������ �����δ� ���� �̿���� �ʴ´�*/

DWORD _tmain(int argc, int *argv[])
{
	HANDLE hMailSlot;  //mailslot �ڵ�
	HANDLE hMailSlot2;
	TCHAR messageBox[50] = {};//�޼��� �ڽ� ���� ũ��
	DWORD bytesRead;  //ũ��.
	TCHAR save_File[10][50] = {};
	DWORD pc = 0;
	
	// mailslot ���� 
	hMailSlot = CreateMailslot(
		SLOT_NAME,
		0, //���Ͻ��� ����ũ��
		MAILSLOT_WAIT_FOREVER,
		NULL);

	hMailSlot2 = CreateFile(SLOT_NAME2, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	//���� ���ҽ� ����ó��
	if (hMailSlot == INVALID_HANDLE_VALUE)
	{
		_fputts(_T("Unable to create mailslot!\n"), stdout);
		return 1;
	}


	/* Message ���� */
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
			messageBox[bytesRead / sizeof(TCHAR)] = '0'; //NULL ���� ����.
		_fputts(messageBox, stdout);
		
		//save_File[pc][50] = *messageBox; //���� ����Ű�� ���� �̷��� ���� �ȵ�
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



