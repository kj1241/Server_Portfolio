#include "stdafx.h"
#include "WinAPI.h"

HWND WinAPI::WinAPI_hwnd = nullptr; //핸들 값 초기화
WinAPI* WinAPI::pWinAPI = nullptr;

static BOOL CALLBACK mainDlgdProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return WinAPI::pWinAPI->MainDlgProc(hWnd, msg, wParam, lParam);
}

static BOOL CALLBACK loginDlgdProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return WinAPI::pWinAPI->LoginDlgProc(hWnd, msg, wParam, lParam);
}

WinAPI::WinAPI()
{
	pWinAPI = this;
}

WinAPI::~WinAPI()
{
	pWinAPI = nullptr;

	closesocket(commsock);
	WSACleanup();
}

bool WinAPI::Init(HINSTANCE hInstance, int nCmdShow)
{
	hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CHAT_CLIENT), NULL, ::mainDlgdProc);
	if (hDlg == NULL)
		return 0;

	wVersionRequested = MAKEWORD(nMinor, nMajor);
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet == SOCKET_ERROR || LOBYTE(wsaData.wVersion) != nMajor || HIBYTE(wsaData.wVersion) != nMinor)
	{
		MessageBox(hDlg, "socket Error", "socket 에러", MB_OK);
		return 0;
	}

	INT_PTR nID = DialogBox(hInstance, MAKEINTRESOURCE(IDD_CHAT_LOGIN),	NULL, ::loginDlgdProc);
	if (nID == IDCANCEL)
	{
		WSACleanup();
		return 0;
	}

	ShowWindow(hDlg, SW_SHOW);
	return TRUE;
}

int WinAPI::Run()
{
	// 메인 루프
	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!IsDialogMessage(hDlg, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


	return 0;
}

HWND WinAPI::GetHwnd()
{
	return WinAPI_hwnd;
}

BOOL WinAPI::MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SOCKET_NOTIFY:
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
			OnRead(hDlg);
			break;
		}
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_SEND:
			OnSend(hDlg);
			break;

		case IDCANCEL:
			DestroyWindow(hDlg);
			PostQuitMessage(0);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL WinAPI::LoginDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_LOGIN:
			if (OnLogIn(hDlg))
 				EndDialog(hDlg, IDC_LOGIN);
			break;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL WinAPI::OnLogIn(HWND hDlg)
{
	SetCursor(LoadCursor(NULL, IDC_WAIT));
	commsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (commsock != INVALID_SOCKET)
	{
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(42006);
		addr.sin_addr.s_addr = inet_addr(CHAT_SERVER_IP);
		if (connect(commsock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
			MessageBox(hDlg, "서버x", "연결 에러", MB_OK);
			closesocket(commsock);
		}
		else
		{
			//마이크로소프트!!! 다이얼로그 만들어줄때 왜 핸들주소가 바뀌냐고
			int temp=WSAAsyncSelect(commsock, pWinAPI->hDlg, WM_SOCKET_NOTIFY, FD_READ);
			GetDlgItemText(hDlg, IDC_CHAT_NAME, chatName, sizeof(chatName));
		}
	}
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	
	return (commsock != INVALID_SOCKET);
	
}

void WinAPI::OnSend(HWND hDlg)
{
	char msg[256] = { 0 };
	GetDlgItemText(hDlg, IDC_MSG, msg, sizeof(msg));//메시지
	SetDlgItemText(hDlg, IDC_MSG, "");

	char buf[128 + 256 + 1] = { 0 };
	wsprintf(buf, "%s>>%s", chatName, msg); //닉네임
	send(commsock, buf, (int)strlen(buf), 0);
}

void WinAPI::OnRead(HWND hDlg)
{
	char buf[128 *3 + 1] = { 0 };
	recv(commsock, buf, sizeof(buf) - 1, 0); //메시지 받기
	AddString(hDlg, IDC_MSG_LIST, buf);
}

void WinAPI::AddString(HWND hDlg, UINT nID, char* msg)
{
	HWND hList = GetDlgItem(hDlg, nID);
	int n = (int)SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)msg);
	SendMessage(hList, LB_SETTOPINDEX, n, 0);
}

