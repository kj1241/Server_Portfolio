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


// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
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
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) 
		err_quit(L"socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;

	if (InetPton(AF_INET, SERVERIP, &serveraddr.sin_addr.s_addr) == 1)
	{
		serveraddr.sin_port = htons(SERVERPORT);
		retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR)
			err_quit(L"connect()");
	}
	else
	{
		err_quit(L"IPAddr()");
	}


	// ������ ��ſ� ����� ����
	wchar_t buf[BUFSIZE + 1];
	const wchar_t* testdata[] = {
		L"�ȳ��ϼ���",
		L"�̰� ���� �ۼ��� �ڵ� ���Դϴ�.",
		L"�����ڵ�� �ۼ��߽��ϴ�.",
		L"�ֽ� winsock2 �ڵ带 �����Ͽ����� ���� �������� �����ŵ� �˴ϴ�.",
	};
	int len;

	// ������ ������ ���
	for(int i=0; i<4; ++i){
		// ������ �Է�(�ùķ��̼�)
		len = wcslen(testdata[i]);
		wcsncpy_s(buf, testdata[i], len);
		buf[len] = L'\0';

		// ������ ������(���� ����)
		retval = send(sock, reinterpret_cast<char*>(&len), sizeof(int), 0);
		if(retval == SOCKET_ERROR){
			err_display(L"send()");
			break;
		}

		// ������ ������(���� ����)
		retval = send(sock, reinterpret_cast<char*>(buf), len * sizeof(wchar_t), 0);
		if (retval == SOCKET_ERROR) {
			err_display(L"send()");
			break;
		}
		std::wcout << L"TCP Ŭ���̾�Ʈ " << sizeof(int) << L"+" << retval << L"���½��ϴ�.\n";

	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}