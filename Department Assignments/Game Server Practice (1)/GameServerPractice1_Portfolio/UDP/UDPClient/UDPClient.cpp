#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVERIP   L"127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

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

	std::wcout.imbue(std::locale("kor"));

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET) 
		err_quit(L"socket()");

	// ���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	if (InetPton(AF_INET, SERVERIP, &serveraddr.sin_addr.s_addr) == 1)
	{
		serveraddr.sin_port = htons(SERVERPORT);
	}

	else
	{
		err_quit(L"IPAddr()");
	}


	// ������ ��ſ� ����� ����
	SOCKADDR_IN peeraddr;
	int addrlen;
	wchar_t buf[BUFSIZE+1];
	int len;

	// ������ ������ ���
	while(1){
		// ������ �Է�
		std::wcout << L"\n ���� ������:";
		if (fgetws(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = static_cast<int>(wcslen(buf));
		if (len > 0 && buf[len - 1] == L'\n')
			buf[len - 1] = L'\0';
		if (len == 0)
			break;

		// ������ ������
		retval = sendto(sock, reinterpret_cast<char*>(buf), len * sizeof(wchar_t), 0,(SOCKADDR *)&serveraddr, sizeof(serveraddr));
		if(retval == SOCKET_ERROR){
			err_display(L"sendto()");
			continue;
		}
		std::wcout << L"UDP Ŭ���̾�Ʈ: " << retval << L"����Ʈ�� ���½��ϴ�.\n";

		// ������ �ޱ�
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, reinterpret_cast<char*>(buf), BUFSIZE, 0,(SOCKADDR *)&peeraddr, &addrlen);
		if(retval == SOCKET_ERROR){
			err_display(L"recvfrom()");
			continue;
		}

		// �۽����� IP �ּ� üũ
		if(memcmp(&peeraddr, &serveraddr, sizeof(peeraddr))){
			std::wcout << L"�߸��� �������Դϴ�!\n";
			continue;
		}

		// ���� ������ ���
		buf[retval] = '\0';
		std::wcout << L"UDP Ŭ���̾�Ʈ: " << retval << L"����Ʈ�� �޾ҽ��ϴ�.\n";
		std::wcout << "���� ������" << buf << "\n";
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}