#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iomanip>
#include <string>


/*

★★만들고 나서 좀 찾기 어려웠던 트레블 슈팅
바이너리 파일에는 전부 들어가는데 콘솔에서 한글 출력이 안되는 현상
std::wcout.imbue(std::locale("kor")); 한글 사용할거라고 선언해주자..

*/


int wmain() {

    WSADATA wsa;
    IN_ADDR ipv4num;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    std::wcout.imbue(std::locale("kor"));

    // 원래의 IPv4 주소 출력
    const wchar_t* ipv4test = L"147.46.114.70";
    std::wcout << L"IPv4 주소(변환 전) = " << ipv4test << L"\n";


    
    // inet_addr() 함수 연습
    // 변경사항 InetPton은 inet_addr와다르게 주소를 구조체에 저장하므로 따로 변환할 필요가 없음
    // inet_addr을 사용하면 std::wcout << L"IPv4 주소(다시 변환 후) = " << inet_ntoa(ipv4num) << "\n"; 사용해야됨
    //std::wcout << L"IPv4 주소(변환 후) = 0x" << std::hex << inet_addr(reinterpret_cast<const char*>(ipv4test)) << "\n";
    if (InetPton(AF_INET, ipv4test, &ipv4num) == 1) 
        std::wcout << L"IPv4 주소(변환 후) = 0x" << std::hex << ipv4num.S_un.S_addr << L"\n";
        //wprintf(L"IPv4 주소(변환 후) = 0x%x\n", ipv4num.S_un.S_addr);
    
    else
        std::wcout << L"IPv4 주소 변환 실패\n";

    std::wcout << L"\n";

    // 원래의 IPv6 주소 출력
    const wchar_t* ipv6test = L"2001:0230:abcd:ffab:0023:eb00:ffff:1111";
    std::wcout << "IPv6 주소(변환 전) = " << ipv6test << L"\n";
    //wprintf(L"IPv6 주소(변환 전) = %s\n", ipv6test);

    // WSAStringToAddress() 함수 연습
    SOCKADDR_IN6 ipv6num;
    int addrlen = sizeof(ipv6num);
    WSAStringToAddressW(const_cast<wchar_t*>(ipv6test), AF_INET6, NULL,
        reinterpret_cast<SOCKADDR*>(&ipv6num), &addrlen);
    std::wcout << L"IPv6 주소(변환 후) = 0x";
    for (int i = 0; i < 16; i++)
        std::wcout << std::setw(2) << std::setfill(L'0') << std::hex << static_cast<int>(ipv6num.sin6_addr.u.Byte[i]);
        //wprintf(L"%02x ", ipv6num.sin6_addr.u.Byte[i]);
    std::wcout << L"\n";

    // WSAAddressToString() 함수 연습
    wchar_t ipaddr[50];
    DWORD ipaddrlen = sizeof(ipaddr) / sizeof(ipaddr[0]);
    WSAAddressToStringW(reinterpret_cast<SOCKADDR*>(&ipv6num), sizeof(ipv6num), NULL, ipaddr, &ipaddrlen);
    std::wcout << L"IPv6 주소(다시 변환 후) = " << ipaddr << L"\n";

    WSACleanup();
    return 0;
}
