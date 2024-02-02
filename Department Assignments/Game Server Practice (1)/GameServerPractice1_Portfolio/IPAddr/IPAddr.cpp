#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iomanip>
#include <string>


/*

�ڡڸ���� ���� �� ã�� ������� Ʈ���� ����
���̳ʸ� ���Ͽ��� ���� ���µ� �ֿܼ��� �ѱ� ����� �ȵǴ� ����
std::wcout.imbue(std::locale("kor")); �ѱ� ����ҰŶ�� ����������..

*/


int wmain() {

    WSADATA wsa;
    IN_ADDR ipv4num;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    std::wcout.imbue(std::locale("kor"));

    // ������ IPv4 �ּ� ���
    const wchar_t* ipv4test = L"147.46.114.70";
    std::wcout << L"IPv4 �ּ�(��ȯ ��) = " << ipv4test << L"\n";


    
    // inet_addr() �Լ� ����
    // ������� InetPton�� inet_addr�ʹٸ��� �ּҸ� ����ü�� �����ϹǷ� ���� ��ȯ�� �ʿ䰡 ����
    // inet_addr�� ����ϸ� std::wcout << L"IPv4 �ּ�(�ٽ� ��ȯ ��) = " << inet_ntoa(ipv4num) << "\n"; ����ؾߵ�
    //std::wcout << L"IPv4 �ּ�(��ȯ ��) = 0x" << std::hex << inet_addr(reinterpret_cast<const char*>(ipv4test)) << "\n";
    if (InetPton(AF_INET, ipv4test, &ipv4num) == 1) 
        std::wcout << L"IPv4 �ּ�(��ȯ ��) = 0x" << std::hex << ipv4num.S_un.S_addr << L"\n";
        //wprintf(L"IPv4 �ּ�(��ȯ ��) = 0x%x\n", ipv4num.S_un.S_addr);
    
    else
        std::wcout << L"IPv4 �ּ� ��ȯ ����\n";

    std::wcout << L"\n";

    // ������ IPv6 �ּ� ���
    const wchar_t* ipv6test = L"2001:0230:abcd:ffab:0023:eb00:ffff:1111";
    std::wcout << "IPv6 �ּ�(��ȯ ��) = " << ipv6test << L"\n";
    //wprintf(L"IPv6 �ּ�(��ȯ ��) = %s\n", ipv6test);

    // WSAStringToAddress() �Լ� ����
    SOCKADDR_IN6 ipv6num;
    int addrlen = sizeof(ipv6num);
    WSAStringToAddressW(const_cast<wchar_t*>(ipv6test), AF_INET6, NULL,
        reinterpret_cast<SOCKADDR*>(&ipv6num), &addrlen);
    std::wcout << L"IPv6 �ּ�(��ȯ ��) = 0x";
    for (int i = 0; i < 16; i++)
        std::wcout << std::setw(2) << std::setfill(L'0') << std::hex << static_cast<int>(ipv6num.sin6_addr.u.Byte[i]);
        //wprintf(L"%02x ", ipv6num.sin6_addr.u.Byte[i]);
    std::wcout << L"\n";

    // WSAAddressToString() �Լ� ����
    wchar_t ipaddr[50];
    DWORD ipaddrlen = sizeof(ipaddr) / sizeof(ipaddr[0]);
    WSAAddressToStringW(reinterpret_cast<SOCKADDR*>(&ipv6num), sizeof(ipv6num), NULL, ipaddr, &ipaddrlen);
    std::wcout << L"IPv6 �ּ�(�ٽ� ��ȯ ��) = " << ipaddr << L"\n";

    WSACleanup();
    return 0;
}
