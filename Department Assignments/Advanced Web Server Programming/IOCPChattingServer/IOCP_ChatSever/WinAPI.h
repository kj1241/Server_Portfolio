#pragma once
#include"Threads.h"

class WinAPI :public Threads
{
public:
    WinAPI(); //������
    ~WinAPI(); //�Ҹ���
    bool Init(HINSTANCE hInstance, int nCmdShow); //�ʱ�ȭ
    int Run(); //����
    HWND GetHwnd(); //window �ڵ� ���

    BOOL CALLBACK MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static WinAPI* pWinAPI;

private:
    static HWND WinAPI_hwnd; //������ �ڵ�

    SYSTEM_INFO si;
    HWND hDlg = nullptr;
    HANDLE hIOCP = nullptr;	// IOCP ��ü �ڵ�
    WSADATA wsaData;
    BYTE nMajor = 2, nMinor = 2;
    WORD wVersionRequested;
    int	nRet;
    CRITICAL_SECTION cs; //ũ��Ƽ�ü���
    SOCKET lstnsock = INVALID_SOCKET;
    char acceptBuf[(sizeof(sockaddr_in) + 16) * 2];  //���� ����
    ACCEPTOVERLAPPED AOV = { 0 }; // ���� ��� ���Ͽ� WSAOVERLAPPED
    long threadCount = 0;
    list<OVERLAPPEDSOCK*> listOverlapped;

    BOOL CreateListenSocket(HWND hDlg);//���� ����

    THREADDATA* threadData;
};