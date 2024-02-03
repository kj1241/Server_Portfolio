#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVERPORT 9000
#define BUFSIZE    512

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
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	std::wcout.imbue(std::locale("kor"));

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET) 
		err_quit(L"socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) 
		err_quit(L"bind()");

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN clientaddr;
	int addrlen;
	wchar_t buf[BUFSIZE+1];

	// 클라이언트와 데이터 통신
	while(1){
		// 데이터 받기
		addrlen = sizeof(clientaddr);
		retval = recvfrom(sock, reinterpret_cast<char*>(buf), BUFSIZE, 0,(SOCKADDR *)&clientaddr, &addrlen);
		if(retval == SOCKET_ERROR){
			err_display(L"recvfrom()");
			continue;
		}

		// 받은 데이터 출력
		buf[retval / sizeof(wchar_t)] = L'\0';
		
		char ipBuffer[INET_ADDRSTRLEN]; //이건 유니코드 못쓰는데?확인좀
		if (inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN) != nullptr) {
			std::wcout << L"\nUDP 서버 클라이언트 접속: IP 주소=" << ipBuffer << L", 포트 번호=" << ntohs(clientaddr.sin_port) <<L": "<< buf<< L"\n";
		}
		else {
			std::wcerr << L"inet_ntop failed\n";
			break;
		}

		// 데이터 보내기

		retval = sendto(sock, reinterpret_cast<char*>(buf), retval, 0,
			(SOCKADDR *)&clientaddr, sizeof(clientaddr));
		if(retval == SOCKET_ERROR){
			err_display(L"sendto()");
			continue;
		}
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}