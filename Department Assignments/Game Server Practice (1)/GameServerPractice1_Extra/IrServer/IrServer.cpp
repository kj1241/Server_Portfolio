#define _WIN32_WINDOWS 0x0410 // 윈도우 98 이상
#define _WIN32_WINNT   0x0500 // 윈도우 2000 이상

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <af_irda.h>
#include <iostream>
#include <ws2tcpip.h>

#define BUFSIZE 512

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

int main(int argc, char *argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) return 1;

	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale("kor"));

	// socket()
	SOCKET listen_sock = socket(AF_IRDA, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) 
		err_quit(L"socket()");	

	// bind()
	SOCKADDR_IRDA serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.irdaAddressFamily = AF_IRDA;
	strcpy_s(serveraddr.irdaServiceName, "IrServer");
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) 
		err_quit(L"bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) 
		err_quit(L"listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IRDA clientaddr;
	int addrlen;
	wchar_t buf[BUFSIZE+1];

	while(1){
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET){
			err_display(L"accept()");
			break;
		}
		std::wcout << L"\nIrDA 서버 클라이언트 접속: " << clientaddr.irdaServiceName << L"\n";

		// 클라이언트와 데이터 통신
		while(1){
			// 데이터 받기
			retval = recv(client_sock, reinterpret_cast<char*>(buf), BUFSIZE, 0);
			if(retval == SOCKET_ERROR){
				err_display(L"recv()");
				break;
			}
			else if(retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			std::wcout << L"받은 데이터: " << buf << L"\n";

			// 데이터 보내기
			retval = send(client_sock, reinterpret_cast<char*>(buf), retval, 0);
			if(retval == SOCKET_ERROR){
				err_display(L"send()");
				break;
			}
		}

		// closesocket()
		closesocket(client_sock);
		std::wcout << L"IrDA 서버 클라이언트 종료: " << clientaddr.irdaServiceName << L"\n";
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}