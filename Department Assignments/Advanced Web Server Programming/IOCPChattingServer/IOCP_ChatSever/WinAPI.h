#pragma once
#include"Threads.h"

class WinAPI :public Threads
{
public:
    WinAPI(); //생성자
    ~WinAPI(); //소멸자
    bool Init(HINSTANCE hInstance, int nCmdShow); //초기화
    int Run(); //실행
    HWND GetHwnd(); //window 핸들 얻기

    BOOL CALLBACK MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static WinAPI* pWinAPI;

private:
    static HWND WinAPI_hwnd; //윈도우 핸들

    SYSTEM_INFO si;
    HWND hDlg = nullptr;
    HANDLE hIOCP = nullptr;	// IOCP 객체 핸들
    WSADATA wsaData;
    BYTE nMajor = 2, nMinor = 2;
    WORD wVersionRequested;
    int	nRet;
    CRITICAL_SECTION cs; //크리티컬섹션
    SOCKET lstnsock = INVALID_SOCKET;
    char acceptBuf[(sizeof(sockaddr_in) + 16) * 2];  //연동 버퍼
    ACCEPTOVERLAPPED AOV = { 0 }; // 접속 대기 소켓용 WSAOVERLAPPED
    long threadCount = 0;
    list<OVERLAPPEDSOCK*> listOverlapped;

    BOOL CreateListenSocket(HWND hDlg);//리슨 소켓

    THREADDATA* threadData;
};