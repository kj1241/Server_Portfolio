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

    // 호스트 바이트 -> 네트워크 바이트
    std::wcout << L"호스트 바이트 -> 네트워크 바이트" << "\n";
    std::wcout << L"0x" << std::hex << y1 << L" -> 0x" << std::hex << (y2 = htonl(y1)) << "\n";
    std::wcout << L"0x" << std::hex << x1 << L" -> 0x" << std::hex << (x2 = htons(x1)) << "\n";

    // 네트워크 바이트 -> 호스트 바이트
    std::wcout << L"\n네트워크 바이트 -> 호스트 바이트" << "\n";
    std::wcout << L"0x" << std::hex << x2 << L" -> 0x" << std::hex << ntohs(x2) << "\n";
    std::wcout << L"0x" << std::hex << y2 << L" -> 0x" << std::hex << ntohl(y2) << "\n";

    // 잘못된 사용 예
    std::wcout << L"\n잘못된 사용 예" << "\n";
    std::wcout << L"0x" << std::hex << x1 << L" -> 0x" << std::hex << htonl(x1) <<"\n";

    WSACleanup();
    return 0;
}