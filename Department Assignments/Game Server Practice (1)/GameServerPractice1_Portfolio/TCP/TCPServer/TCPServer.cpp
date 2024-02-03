#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

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

int main(int argc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	std::wcout.imbue(std::locale("kor"));

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit(L"socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit(L"bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit(L"listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	wchar_t buf[BUFSIZE + 1];

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display(L"accept()");
			break;
		}
		char ipBuffer[INET_ADDRSTRLEN]; //�̰� �����ڵ� �����µ�?Ȯ����
		if (inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN) != nullptr) {
			std::wcout << L"\nTCP ���� Ŭ���̾�Ʈ ����: IP �ּ�=" << ipBuffer << L", ��Ʈ ��ȣ=" << ntohs(clientaddr.sin_port) << L"\n";
		}
		else {
			std::wcerr << L"inet_ntop failed\n";
			break;
		}

		// Ŭ���̾�Ʈ�� ������ ���
		while (1) {
			// ������ �ޱ�
			retval = recv(client_sock, reinterpret_cast<char*>(buf), BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display(L"recv()");
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ���
			buf[retval / sizeof(wchar_t)] = L'\0';
			std::wcout << L"TCP " << ipBuffer << L"::" << ntohs(clientaddr.sin_port) << L": " << buf << L"\n";

			// ������ ������
			retval = send(client_sock, reinterpret_cast<char*>(buf), retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display(L"send()");
				break;
			}
		}

		// closesocket()
		closesocket(client_sock);
		std::wcout << L"TCP ���� Ŭ���̾�Ʈ ����:  IP �ּ�=" << inet_ntop(AF_INET, &clientaddr.sin_addr, (char*)buf, INET_ADDRSTRLEN) << L", ��Ʈ ��ȣ=" << ntohs(clientaddr.sin_port) << L"\n";
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}