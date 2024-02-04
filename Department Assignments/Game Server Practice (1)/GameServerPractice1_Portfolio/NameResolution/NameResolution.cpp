#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#define TESTNAME L"www.naver.com"



/*
// ���ô� �ڵ�
HOSTENT *ptr = gethostbyname(reinterpret_cast<const char*>(name));
getaddrinfo() ����

mbstowcs -> mbstowcs_s ����
_TRUNCATE ������ ũ�� �߶��ִ°�
*/


// ���� �Լ� ���� ���
void err_display(const wchar_t* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    std::wcout << L"[" << msg << L"] " << (wchar_t*)lpMsgBuf;
    LocalFree(lpMsgBuf);
}

// �ڵ� ������ ���� getaddrinfo�� wchar_t�� ���� ���Ѵ�.
//BOOL GetIPAddr(const wchar_t* name, IN_ADDR* addr)
//{
//    addrinfo* result = NULL;
//    addrinfo hints;
//
//    ZeroMemory(&hints, sizeof(hints));
//    hints.ai_family = AF_INET;
//
//    INT ptr = getaddrinfo(reinterpret_cast<const char*>(name), NULL, &hints, &result);
//
//    if (ptr != 0) {
//        err_display(L"getaddrinfo()");
//        return FALSE;
//    }
//
//    sockaddr_in* sockaddr_ipv4 = reinterpret_cast<sockaddr_in*>(result->ai_addr);
//    *addr = sockaddr_ipv4->sin_addr;
//
//    freeaddrinfo(result);
//    return TRUE;
//}

//������ ����: �̰� �����ڵ� ���� �����ֳ� ...��¾�� ����� ���� �ٺ���
//���� gethostbyname�̰� ����.
BOOL GetIPAddr(const wchar_t* name, IN_ADDR* addr)
{
    ADDRINFOW* result = NULL;
    ADDRINFOW  hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;

    INT ptr = GetAddrInfoW(name, NULL, &hints, &result);

    if (ptr != 0) {
        err_display(L"getaddrinfo()");
        return FALSE;
    }

    sockaddr_in* sockaddr_ipv4 = reinterpret_cast<sockaddr_in*>(result->ai_addr);
    *addr = sockaddr_ipv4->sin_addr;

    FreeAddrInfoW(result);
    return TRUE;
}


// IPv4 �ּ� -> ������ �̸�
BOOL GetDomainName(IN_ADDR addr, wchar_t* name, int namelen)
{
    sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr = addr;

    char hostname[NI_MAXHOST];
    if (getnameinfo(reinterpret_cast<sockaddr*>(&sa), sizeof(sa), hostname, NI_MAXHOST, NULL, 0, 0) != 0) {
        err_display(L"getnameinfo()");
        return FALSE;
    }
    size_t convertedChars = 0;

    mbstowcs_s(&convertedChars, name, namelen, hostname, _TRUNCATE);
    return TRUE;
}

int main(int argc, char* argv[])
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale("kor"));

    std::wcout << L"������ �̸�(��ȯ ��) = " << TESTNAME << L"\n";

    // ������ �̸� -> IP �ּ�
    IN_ADDR addr;
    if (GetIPAddr(TESTNAME, &addr)) {
        // �����̸� ��� ���
        char addrStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr, addrStr, INET_ADDRSTRLEN);
        std::wcout << L"IP �ּ�(��ȯ ��) = " << addrStr << L"\n";

        // IP �ּ� -> ������ �̸�
        wchar_t name[256];
        if (GetDomainName(addr, name, sizeof(name) / sizeof(name[0]))) {
            // �����̸� ��� ���
            std::wcout << L"������ �̸�(�ٽ� ��ȯ ��) = " << name << L"\n";
        }
    }

    WSACleanup();
    return 0;
}
