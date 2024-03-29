#pragma once

class WinAPI
{
public:
    WinAPI(); //������
    ~WinAPI(); //�Ҹ���
    bool Init(HINSTANCE hInstance, int nCmdShow); //�ʱ�ȭ
    int Run(); //����
    HWND GetHwnd(); //window �ڵ� ���

    BOOL CALLBACK MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam); //���� ���̾�α�
    BOOL CALLBACK LoginDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam); //�α��� ���̾�α�
    static WinAPI* pWinAPI;

private:
    static HWND WinAPI_hwnd; //������ �ڵ�

    HWND hDlg;
    WSADATA wsaData;
    BYTE nMajor = 2, nMinor = 2;
    WORD wVersionRequested;
    int	nRet;
    SOCKET commsock;
    char chatName[128] = { 0 };

    BOOL OnLogIn(HWND hDlg); //�α��� ó��
    void OnSend(HWND hDlg); //�޽��� ����
    void OnRead(HWND hDlg); //�޽��� ����
    void AddString(HWND hDlg, UINT nID, char* msg); //���ڿ� ���ϱ�

};