#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>


#define SERVERPORT 9000
#define BUFSIZE    512

// 소켓 정보 저장을 위한 구조체와 변수
struct SOCKETINFO
{
	SOCKET sock;
	wchar_t buf[BUFSIZE+1];
	int recvbytes;
	int sendbytes;
};

int nTotalSockets = 0;
SOCKETINFO *SocketInfoArray[WSA_MAXIMUM_WAIT_EVENTS];
WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];

// 소켓 관리 함수
BOOL AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);

// 오류 출력 함수
void err_quit(const wchar_t *msg);
void err_display(const wchar_t *msg);
void err_display(int errcode);

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

	// 소켓 정보 추가 & WSAEventSelect()
	AddSocketInfo(listen_sock);
	retval = WSAEventSelect(listen_sock, EventArray[nTotalSockets-1],
		FD_ACCEPT|FD_CLOSE);
	if(retval == SOCKET_ERROR) 
		err_quit(L"WSAEventSelect()");

	// 데이터 통신에 사용할 변수
	WSANETWORKEVENTS NetworkEvents;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int i, addrlen;

	while(1){
		// 이벤트 객체 관찰하기
		i = WSAWaitForMultipleEvents(nTotalSockets, EventArray,	FALSE, WSA_INFINITE, FALSE);
		if(i == WSA_WAIT_FAILED) continue;
		i -= WSA_WAIT_EVENT_0;

		// 구체적인 네트워크 이벤트 알아내기
		retval = WSAEnumNetworkEvents(SocketInfoArray[i]->sock,	EventArray[i], &NetworkEvents);
		if(retval == SOCKET_ERROR) 
			continue;

		// FD_ACCEPT 이벤트 처리
		char ipBuffer[INET_ADDRSTRLEN];
		if(NetworkEvents.lNetworkEvents & FD_ACCEPT){
			if(NetworkEvents.iErrorCode[FD_ACCEPT_BIT] != 0){
				err_display(NetworkEvents.iErrorCode[FD_ACCEPT_BIT]);
				continue;
			}

			addrlen = sizeof(clientaddr);
			client_sock = accept(SocketInfoArray[i]->sock,(SOCKADDR *)&clientaddr, &addrlen);
			if(client_sock == INVALID_SOCKET){
				err_display(L"accept()");
				continue;
			}

			if (inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN) != nullptr) {
				std::wcout << L"\nTCP 서버 클라이언트 접속: IP 주소=" << ipBuffer << L", 포트 번호=" << ntohs(clientaddr.sin_port) << L"\n";
			}
			else {
				std::wcerr << L"inet_ntop failed\n";
				break;
			}

			if(nTotalSockets >= WSA_MAXIMUM_WAIT_EVENTS){
				std::wcout << "더 이상 접속을 받아들일 수 없습니다\n";
				closesocket(client_sock);
				continue;
			}

			// 소켓 정보 추가 & WSAEventSelect()
			AddSocketInfo(client_sock);
			retval = WSAEventSelect(client_sock, EventArray[nTotalSockets-1],FD_READ|FD_WRITE|FD_CLOSE);
			if(retval == SOCKET_ERROR) 
				err_quit(L"WSAEventSelect()");
		}

		// FD_READ & FD_WRITE 이벤트 처리
		if(NetworkEvents.lNetworkEvents & FD_READ || NetworkEvents.lNetworkEvents & FD_WRITE)
		{
			if(NetworkEvents.lNetworkEvents & FD_READ
				&& NetworkEvents.iErrorCode[FD_READ_BIT] != 0)
			{
				err_display(NetworkEvents.iErrorCode[FD_READ_BIT]);
				continue;
			}
			if(NetworkEvents.lNetworkEvents & FD_WRITE
				&& NetworkEvents.iErrorCode[FD_WRITE_BIT] != 0)
			{
				err_display(NetworkEvents.iErrorCode[FD_WRITE_BIT]);
				continue;
			}

			SOCKETINFO *ptr = SocketInfoArray[i];

			if(ptr->recvbytes == 0){
				// 데이터 받기
				retval = recv(ptr->sock, reinterpret_cast<char*>(ptr->buf), BUFSIZE, 0);
				if(retval == SOCKET_ERROR){
					if(WSAGetLastError() != WSAEWOULDBLOCK){
						err_display(L"recv()");
						RemoveSocketInfo(i);
					}
					continue;
				}
				ptr->recvbytes = retval;
				// 받은 데이터 출력
				ptr->buf[retval / sizeof(wchar_t)] = L'\0';
				addrlen = sizeof(clientaddr);
				getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);
				std::wcout << L"TCP " << ipBuffer << L"::" << ntohs(clientaddr.sin_port) << L": " << ptr->buf << L"\n";
			}

			if(ptr->recvbytes > ptr->sendbytes){
				// 데이터 보내기
				retval = send(ptr->sock, reinterpret_cast<char*>(ptr->buf + ptr->sendbytes), ptr->recvbytes - ptr->sendbytes, 0);
				if(retval == SOCKET_ERROR){
					if(WSAGetLastError() != WSAEWOULDBLOCK){
						err_display(L"send()");
						RemoveSocketInfo(i);
					}
					continue;
				}
				ptr->sendbytes += retval;
				// 받은 데이터를 모두 보냈는지 체크
				if(ptr->recvbytes == ptr->sendbytes)
					ptr->recvbytes = ptr->sendbytes = 0;
			}
		}

		// FD_CLOSE 이벤트 처리
		if(NetworkEvents.lNetworkEvents & FD_CLOSE){
			if(NetworkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
				err_display(NetworkEvents.iErrorCode[FD_CLOSE_BIT]);
			RemoveSocketInfo(i);
		}
	}

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 소켓 정보 추가
BOOL AddSocketInfo(SOCKET sock)
{
	SOCKETINFO *ptr = new SOCKETINFO;
	if(ptr == NULL){
		std::wcout << "메모리가 부족합니다\n";
		return FALSE;
	}

	WSAEVENT hEvent = WSACreateEvent();
	if(hEvent == WSA_INVALID_EVENT){
		err_display(L"WSACreateEvent()");
		return FALSE;
	}

	ptr->sock = sock;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;
	SocketInfoArray[nTotalSockets] = ptr;
	EventArray[nTotalSockets] = hEvent;
	++nTotalSockets;

	return TRUE;
}

// 소켓 정보 삭제
void RemoveSocketInfo(int nIndex)
{
	SOCKETINFO *ptr = SocketInfoArray[nIndex];

	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);

	char ipBuffer[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN) != nullptr) {
		std::wcout << L"\nTCP 서버 클라이언트 종료: IP 주소=" << ipBuffer << L", 포트 번호=" << ntohs(clientaddr.sin_port) << L"\n";
	}
	else {
		std::wcerr << L"inet_ntop failed\n";
	}

	closesocket(ptr->sock);
	delete ptr;
	WSACloseEvent(EventArray[nIndex]);

	if(nIndex != (nTotalSockets-1)){
		SocketInfoArray[nIndex] = SocketInfoArray[nTotalSockets-1];
		EventArray[nIndex] = EventArray[nTotalSockets-1];
	}
	--nTotalSockets;
}

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
// 소켓 함수 오류 출력
void err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::wcout << L"[error] " << (wchar_t*)lpMsgBuf << L'\n';
	LocalFree(lpMsgBuf);
}