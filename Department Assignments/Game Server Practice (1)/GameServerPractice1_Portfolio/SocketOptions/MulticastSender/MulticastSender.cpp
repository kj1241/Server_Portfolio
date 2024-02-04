#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#define MULTICASTIP L"235.7.8.9"
#define REMOTEPORT  9000
#define BUFSIZE     512

// ���� �Լ� ���� ��� �� ����
void err_quit(const wchar_t* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&lpMsgBuf, 0, NULL);
	MessageBoxW(NULL, (LPCWSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(const wchar_t* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&lpMsgBuf, 0, NULL);

	std::wcout << L"[" << msg << L"] " << (wchar_t*)lpMsgBuf << L'\n';
	LocalFree(lpMsgBuf);
}
int main(int argc, char *argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale("kor"));

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET) 
		err_quit(L"socket()");

	// ��Ƽĳ��Ʈ TTL ����
	int ttl = 2;
	retval = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL,	(char *)&ttl, sizeof(ttl));
	if(retval == SOCKET_ERROR) 
		err_quit(L"setsockopt()");

	// ���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;

	if (InetPton(AF_INET, MULTICASTIP, &remoteaddr.sin_addr.s_addr) == 1)
	{
		remoteaddr.sin_port = htons(REMOTEPORT);
	}
	else
	{
		err_quit(L"IPAddr()");
	}

	// ������ ��ſ� ����� ����
	wchar_t buf[BUFSIZE+1];
	int len;

	// ��Ƽĳ��Ʈ ������ ������
	while(1){
		// ������ �Է�
		std::wcout << L"\n���� ������: ";
		if (fgetws(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = static_cast<int>(wcslen(buf));
		if (len > 0 && buf[len - 1] == L'\n')
			buf[len - 1] = L'\0';
		if (len == 0)
			break;

		// ������ ������
		retval = sendto(sock, reinterpret_cast<char*>(buf), len * sizeof(wchar_t), 0,(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		if(retval == SOCKET_ERROR){
			err_display(L"sendto()");
			continue;
		}
		std::wcout << L"UDP: " << retval << L"����Ʈ�� ���½��ϴ�.\n";
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}