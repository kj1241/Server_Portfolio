#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#define MULTICASTIP L"235.7.8.9"
#define REMOTEPORT  9000
#define BUFSIZE     512

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

	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale("kor"));

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET) 
		err_quit(L"socket()");

	// 멀티캐스트 TTL 설정
	int ttl = 2;
	retval = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL,	(char *)&ttl, sizeof(ttl));
	if(retval == SOCKET_ERROR) 
		err_quit(L"setsockopt()");

	// 소켓 주소 구조체 초기화
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

	// 데이터 통신에 사용할 변수
	wchar_t buf[BUFSIZE+1];
	int len;

	// 멀티캐스트 데이터 보내기
	while(1){
		// 데이터 입력
		std::wcout << L"\n보낼 데이터: ";
		if (fgetws(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = static_cast<int>(wcslen(buf));
		if (len > 0 && buf[len - 1] == L'\n')
			buf[len - 1] = L'\0';
		if (len == 0)
			break;

		// 데이터 보내기
		retval = sendto(sock, reinterpret_cast<char*>(buf), len * sizeof(wchar_t), 0,(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		if(retval == SOCKET_ERROR){
			err_display(L"sendto()");
			continue;
		}
		std::wcout << L"UDP: " << retval << L"바이트를 보냈습니다.\n";
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}