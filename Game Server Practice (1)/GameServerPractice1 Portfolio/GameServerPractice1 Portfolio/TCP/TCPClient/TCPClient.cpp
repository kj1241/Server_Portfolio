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


// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while(left > 0){
		received = recv(s, ptr, left, flags);
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
	
	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale("kor"));

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) 
		err_quit(L"socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	//serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
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

	/*serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit(L"connect()");*/


	// 데이터 통신에 사용할 변수
	//char buf[BUFSIZE+1];
	//int len;
	wchar_t buf[BUFSIZE + 1];
	int len;

	// 서버와 데이터 통신
	while(1){
		// 데이터 입력
		std::wcout << L"\n보낼 데이터: ";
		if (fgetws(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		/*len = wcslen(buf);
		if(buf[len-1] == '\n')
			buf[len-1] = '\0';
		if(wcslen(buf) == 0)
			break;*/

		len = static_cast<int>(wcslen(buf));
		if (len > 0 && buf[len - 1] == L'\n')
			buf[len - 1] = L'\0';
		if (len == 0)
			break;


		// 데이터 보내기
		retval = send(sock, reinterpret_cast<char*>(buf), len * sizeof(wchar_t), 0);
		if(retval == SOCKET_ERROR){
			err_display(L"send()");
			break;
		}
		std::wcout << L"TCP 클라이언트: " << retval <<L"바이트를 보냈습니다.\n";

		// 데이터 받기
		retval = recvn(sock, reinterpret_cast<char*>(buf), retval, 0);
		if(retval == SOCKET_ERROR){
			err_display(L"recv()");
			break;
		}
		else if(retval == 0)
			break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		std::wcout << L"TCP 클라이언트: " << retval << L"바이트를 받았습니다.\n";
		std::wcout << L"받은 데이터: " << buf << L"\n";
	}




	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}