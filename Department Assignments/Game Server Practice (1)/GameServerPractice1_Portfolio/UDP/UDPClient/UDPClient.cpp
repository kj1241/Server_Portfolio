#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVERIP   L"127.0.0.1"
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

	// 소켓 주소 구조체 초기화
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


	// 데이터 통신에 사용할 변수
	SOCKADDR_IN peeraddr;
	int addrlen;
	wchar_t buf[BUFSIZE+1];
	int len;

	// 서버와 데이터 통신
	while(1){
		// 데이터 입력
		std::wcout << L"\n 보낼 데이터:";
		if (fgetws(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = static_cast<int>(wcslen(buf));
		if (len > 0 && buf[len - 1] == L'\n')
			buf[len - 1] = L'\0';
		if (len == 0)
			break;

		// 데이터 보내기
		retval = sendto(sock, reinterpret_cast<char*>(buf), len * sizeof(wchar_t), 0,(SOCKADDR *)&serveraddr, sizeof(serveraddr));
		if(retval == SOCKET_ERROR){
			err_display(L"sendto()");
			continue;
		}
		std::wcout << L"UDP 클라이언트: " << retval << L"바이트를 보냈습니다.\n";

		// 데이터 받기
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, reinterpret_cast<char*>(buf), BUFSIZE, 0,(SOCKADDR *)&peeraddr, &addrlen);
		if(retval == SOCKET_ERROR){
			err_display(L"recvfrom()");
			continue;
		}

		// 송신자의 IP 주소 체크
		if(memcmp(&peeraddr, &serveraddr, sizeof(peeraddr))){
			std::wcout << L"잘못된 데이터입니다!\n";
			continue;
		}

		// 받은 데이터 출력
		buf[retval] = '\0';
		std::wcout << L"UDP 클라이언트: " << retval << L"바이트를 받았습니다.\n";
		std::wcout << "받은 데이터" << buf << "\n";
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}