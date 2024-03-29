#include "stdafx.h"
#include "WinAPI.h"

HWND WinAPI::WinAPI_hwnd = nullptr; //핸들 값 초기화
WinAPI* WinAPI::pWinAPI = nullptr;


static BOOL CALLBACK mainDlgdProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return WinAPI::pWinAPI->MainDlgProc(hWnd, msg, wParam, lParam);
}


WinAPI::WinAPI()
{
	pWinAPI = this;
    threadData = new THREADDATA;
}

WinAPI::~WinAPI()
{
	pWinAPI = nullptr;

    if (threadData != nullptr)
    {
        delete threadData;
        threadData = nullptr;
    }


    closesocket(lstnsock);

    list<OVERLAPPEDSOCK*>::iterator it = listOverlapped.begin();
    while (it != listOverlapped.end())
    {
        OVERLAPPEDSOCK* pMOV = *it;
        closesocket(pMOV->commsock);
        delete pMOV->wsaBuf.buf;;
        delete pMOV;

        it++;
    }

    listOverlapped.clear();
    DeleteCriticalSection(&cs);
    WSACleanup();
 
}

bool WinAPI::Init(HINSTANCE hInstance, int nCmdShow)
{
    hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CHAT_SERVER), NULL, ::mainDlgdProc);
    if (hDlg == NULL)
        return 0;
    ShowWindow(hDlg, SW_SHOW);

    wVersionRequested = MAKEWORD(nMinor, nMajor);
    nRet = WSAStartup(wVersionRequested, &wsaData);
    if (nRet == SOCKET_ERROR || LOBYTE(wsaData.wVersion) != nMajor || HIBYTE(wsaData.wVersion) != nMinor)
    {
        MessageBox(hDlg, "socket Error", "socket 에러 에러", MB_OK);
        return 0;
    }
    InitializeCriticalSection(&cs); //임계영역 크리티컬섹션

    hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (hIOCP == NULL)
    {
        MessageBox(hDlg, "IOCP Error", "IOCP 포트 생성 에러", MB_OK);
        return 0;
    }

    if (!CreateListenSocket(hDlg))
        return 0;

    threadData->hDlg = hDlg;
    threadData->hIOCP = hIOCP;
    threadData->threadCount = &threadCount;
    threadData->lstnsock = lstnsock;
    threadData->listOverlapped = listOverlapped;
    threadData->cs = cs;
    threadData->AOV = AOV;
    threadData->acceptBuf = acceptBuf;

    ////코어수 만큼 스레드 생성
    GetSystemInfo(&si);
    for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; i++)
    {
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, communicationThread, threadData, 0, NULL);
        CloseHandle(hThread);
    }

}

int WinAPI::Run()
{
    // 메인 루프
    MSG msg = { 0 };

    while (GetMessage(&msg, NULL, 0, 0)) // 메시지가 있으면 처리
    {
        if (!IsDialogMessage(hDlg, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; i++)
        PostQueuedCompletionStatus(hIOCP, 1, 0, NULL);

    while (0 < threadCount)
        Sleep(100);

  

    return 0;    // WM_QUIT 메시지로 반환
}

HWND WinAPI::GetHwnd()
{
    return WinAPI_hwnd;
}



BOOL CALLBACK WinAPI::MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
        case IDC_CLOSE: //닫기 누를 경우
            DestroyWindow(hDlg);
            PostQuitMessage(0);
            break;
        }
        return TRUE;
    }

    return FALSE;
}

BOOL WinAPI::CreateListenSocket(HWND hDlg)
{
    try
    {
        lstnsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (lstnsock == INVALID_SOCKET)
            throw "listen socket 에러";

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(42006);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(lstnsock, (struct sockaddr*)&addr, sizeof(addr))== SOCKET_ERROR)
            throw "bind 에러";

        if (listen(lstnsock, SOMAXCONN) == SOCKET_ERROR)
            throw "listen 에러";

        CreateIoCompletionPort((HANDLE)lstnsock, hIOCP, (DWORD)lstnsock, 0);

        SOCKET commsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (commsock == INVALID_SOCKET)
            throw "socket 에러";

        AOV.commsock = commsock;
        DWORD dwBytes;
        AcceptEx(lstnsock, commsock, acceptBuf, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,&dwBytes, (WSAOVERLAPPED*)&AOV);
    }
    catch (char* errmsg)
    {
        LPVOID lpOSMsg;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpOSMsg, 0, NULL);
        MessageBox(hDlg, (LPCTSTR)lpOSMsg, errmsg, MB_OK);
        LocalFree(lpOSMsg);
        closesocket(lstnsock);
        return FALSE;
    }
    return TRUE;
}
