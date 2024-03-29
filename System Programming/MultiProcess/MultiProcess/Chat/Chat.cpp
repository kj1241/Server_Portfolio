#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#define SLOT_NAME2 _T("\\\\.\\mailslot\\mailbox2")
/*�� �ּҸ� �̿��Ͽ� �и��� ��ǻ�Ͱ����� ���μ����� ��Ű��� ������ �����δ� ���� �̿���� �ʴ´�*/

DWORD _tmain(int argc, int *argv[])
{
	HANDLE hMailSlot;  //mailslot �ڵ�
	TCHAR messageBox[50];//�޼��� �ڽ� ���� ũ��
	DWORD bytesRead;  //ũ��.
	DWORD pc = 0;
	DWORD Load_FIle[10] = {};
	DWORD sum = 0;

	// mailslot ���� 
	hMailSlot = CreateMailslot(
		SLOT_NAME2,
		0, //���Ͻ��� ����ũ��
		MAILSLOT_WAIT_FOREVER,
		NULL);

	//���� ���ҽ� ����ó��
	if (hMailSlot == INVALID_HANDLE_VALUE)
	{
		_fputts(_T("Unable to create mailslot!\n"), stdout);
		return 1;
	}



	/* Message ���� */
	_fputts(_T("Message\n"), stdout);
	while (pc != 10)
	{

		if (!ReadFile(hMailSlot, messageBox, sizeof(TCHAR) * 50, &bytesRead, NULL))
		{
			_fputts(_T("Unable to read!\n"), stdout);
			if (hMailSlot != NULL)
				CloseHandle(hMailSlot);
			return 1;
		}

		if (!_tcsncmp(messageBox, _T("exit\n"), 4))
		{
			_fputts(_T("Good Bye!\n"), stdout);
			break;
		}
		if (bytesRead == 0)
			messageBox[bytesRead / sizeof(TCHAR)] = '0'; //NULL ���� ����.
		_fputts(messageBox, stdout);

		//Load_FIle[pc][50] = *messageBox; //���� ����Ű�� ���� �̷��� ���� �ȵ�
		memcpy(Load_FIle, messageBox, bytesRead);

		pc = pc + 1;
	}
	CloseHandle(hMailSlot);

	for (int i = 0; i < 10; i++)
	{
		sum = sum + Load_FIle[i];
	}

	_tprintf(_T("Score:%d"), sum);
	_tprintf(_T("Average:%d"), sum / 10);
	
	return 0;
}