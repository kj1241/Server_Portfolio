#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVERPORT 9000
#define BUFSIZE    512

/*
고정기리를 받아서 가변길이의 크기를 미리 알고 그다음 가변기리만큼 데이터 받기
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

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while(left > 0){
		received = recv(s, reinterpret_cast<char*>(ptr), left, flags);
		if(received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if(received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

int main(int argc, char *argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	std::wcout.imbue(std::locale("kor"));

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) 
		err_quit(L"socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) 
		err_quit(L"bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) 
		err_quit(L"listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	wchar_t buf[BUFSIZE+1];
	int len = 0;

	while(1){
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display(L"accept()");
			break;
		}

		char ipBuffer[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN) != nullptr) {
			std::cout << "\nTCP 서버 클라이언트 접속: IP 주소=" << ipBuffer << ", 포트 번호=" << ntohs(clientaddr.sin_port) << "\n";
		}
		else {
			std::wcerr << L"inet_ntop failed\n";
			break;
		}

		// 클라이언트와 데이터 통신
		while(1){
			// 데이터 받기(고정 길이)
			retval = recv(client_sock, reinterpret_cast<char*>(&len), sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				err_display(L"recv()");
				break;
			}
			else if (retval == 0)
				break;

			// 데이터 받기(가변 길이)
			retval = recv(client_sock, reinterpret_cast<char*>(buf), len * sizeof(wchar_t), 0);
			if(retval == SOCKET_ERROR){
				err_display(L"recv()");
				break;
			}
			else if(retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval / sizeof(wchar_t)] = L'\0';
			std::wcout << L"TCP " << ipBuffer << L"::" << ntohs(clientaddr.sin_port) << L": " << buf << L"\n";
		}

		// closesocket()
		closesocket(client_sock);
		std::wcout << L"TCP 서버 클라이언트 종료:  IP 주소=" << inet_ntop(AF_INET, &clientaddr.sin_addr, (char*)buf, INET_ADDRSTRLEN) << L", 포트 번호=" << ntohs(clientaddr.sin_port) << L"\n";
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}