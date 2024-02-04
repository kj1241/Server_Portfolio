#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#define SERVERPORT 9000
#define BUFSIZE    512

/*
수행한 트러블슈팅

c2664 
char_t* -> const wchar_t 변경 

c4996 'inet_ntoa': Use inet_ntop() or InetNtop() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings
gethostbyname -> getaddrinfo
inet_addr -> inet_pton
inet_ntoa -> inet_ntop 변경

c6386 버퍼의 크기를 체크하지 않고 사용할 때 발생, 따라서 버퍼크기만 계산할수 있도록 변경
DWORD ipaddrlen = sizeof(ipaddr) -> sizeof(ipaddr) / sizeof(ipaddr[0]) 

*/


// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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

// TCP 서버(IPv4)
DWORD WINAPI TCPServer4(LPVOID arg)
{
	int retval;

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

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display(L"accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		//std::wcout << "TCP 서버 클라이언트 접속: IP 주소=" << inet_ntoa(clientaddr.sin_addr) << ", 포트 번호=" << ntohs(clientaddr.sin_port) << "\n";
		std::wcout << "TCP 서버 클라이언트 접속: IP 주소=" << inet_ntop(AF_INET, &clientaddr.sin_addr, buf, INET_ADDRSTRLEN) << ", 포트 번호=" << ntohs(clientaddr.sin_port) << "\n";
		// 클라이언트와 데이터 통신
		while (1) {
			// 데이터 받기
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display(L"recv()");
				break;
			}
			else if (retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			std::wcout << buf;
		}

		// closesocket()
		closesocket(client_sock);
		//std::wcout << "TCP 서버 클라이언트 종료: IP 주소=" << inet_ntoa(clientaddr.sin_addr) << ", 포트 번호=" << ntohs(clientaddr.sin_port);
		std::wcout << "TCP 서버 클라이언트 종료: IP 주소=" << inet_ntop(AF_INET, &clientaddr.sin_addr, buf, INET_ADDRSTRLEN)<< ", 포트 번호=" << ntohs(clientaddr.sin_port);
	}

	// closesocket()
	closesocket(listen_sock);
	return 0;
}

// TCP 서버(IPv6)
DWORD WINAPI TCPServer6(LPVOID arg)
{
	int retval;

	// socket()
	SOCKET listen_sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) 
		err_quit(L"socket()");

	// bind()
	SOCKADDR_IN6 serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_addr = in6addr_any;
	serveraddr.sin6_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) 
		err_quit(L"bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) 
		err_quit(L"listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN6 clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display(L"accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		wchar_t ipaddr[50];
		//DWORD ipaddrlen = sizeof(ipaddr);
		DWORD ipaddrlen = sizeof(ipaddr) / sizeof(ipaddr[0]);
		WSAAddressToString((SOCKADDR*)&clientaddr, sizeof(clientaddr),NULL, ipaddr, &ipaddrlen);
		std::wcout << "TCP 서버 클라이언트 접속: " << ipaddr << "\n";

		// 클라이언트와 데이터 통신
		while (1) {
			// 데이터 받기
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display(L"recv()");
				break;
			}
			else if (retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			std::wcout << buf;
		}

		// closesocket()
		closesocket(client_sock);
		std::wcout << "TCP 서버 클라이언트 종료: " << ipaddr << "\n";
	}

	// closesocket()
	closesocket(listen_sock);

	return 0;
}
int wmain(int argc, wchar_t* argv[])
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale("kor"));

	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, TCPServer4, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, TCPServer6, NULL, 0, NULL);

	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	// 윈속 종료
	WSACleanup();
	return 0;
}