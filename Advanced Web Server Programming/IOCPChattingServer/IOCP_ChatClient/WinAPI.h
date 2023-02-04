#pragma once

class WinAPI
{
public:
    WinAPI(); //생성자
    ~WinAPI(); //소멸자
    bool Init(HINSTANCE hInstance, int nCmdShow); //초기화
    int Run(); //실행
    HWND GetHwnd(); //window 핸들 얻기

    BOOL CALLBACK MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam); //메인 다이얼로그
    BOOL CALLBACK LoginDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam); //로그인 다이얼로그
    static WinAPI* pWinAPI;

private:
    static HWND WinAPI_hwnd; //윈도우 핸들

    HWND hDlg;
    WSADATA wsaData;
    BYTE nMajor = 2, nMinor = 2;
    WORD wVersionRequested;
    int	nRet;
    SOCKET commsock;
    char chatName[128] = { 0 };

    BOOL OnLogIn(HWND hDlg); //로그인 처리
    void OnSend(HWND hDlg); //메시지 전송
    void OnRead(HWND hDlg); //메시지 수신
    void AddString(HWND hDlg, UINT nID, char* msg); //문자열 더하기

};