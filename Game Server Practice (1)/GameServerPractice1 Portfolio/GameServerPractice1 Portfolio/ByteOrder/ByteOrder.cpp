#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>

int wmain() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale("kor"));

    u_short x1 = 0x1234;
    u_long y1 = 0x12345678;
    u_short x2;
    u_long y2;

    // ȣ��Ʈ ����Ʈ -> ��Ʈ��ũ ����Ʈ
    std::wcout << L"ȣ��Ʈ ����Ʈ -> ��Ʈ��ũ ����Ʈ" << "\n";
    std::wcout << L"0x" << std::hex << y1 << L" -> 0x" << std::hex << (y2 = htonl(y1)) << "\n";
    std::wcout << L"0x" << std::hex << x1 << L" -> 0x" << std::hex << (x2 = htons(x1)) << "\n";

    // ��Ʈ��ũ ����Ʈ -> ȣ��Ʈ ����Ʈ
    std::wcout << L"\n��Ʈ��ũ ����Ʈ -> ȣ��Ʈ ����Ʈ" << "\n";
    std::wcout << L"0x" << std::hex << x2 << L" -> 0x" << std::hex << ntohs(x2) << "\n";
    std::wcout << L"0x" << std::hex << y2 << L" -> 0x" << std::hex << ntohl(y2) << "\n";

    // �߸��� ��� ��
    std::wcout << L"\n�߸��� ��� ��" << "\n";
    std::wcout << L"0x" << std::hex << x1 << L" -> 0x" << std::hex << htonl(x1) <<"\n";

    WSACleanup();
    return 0;
}