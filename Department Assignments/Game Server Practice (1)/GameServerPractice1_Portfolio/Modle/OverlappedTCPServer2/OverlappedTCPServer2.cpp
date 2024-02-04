#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#define SERVERPORT 9000
#define BUFSIZE    512

// 소켓 정보 저장을 위한 구조체와 변수
struct SOCKETINFO
{
	WSAOVERLAPPED overlapped;
	SOCKET sock;
	wchar_t buf[BUFSIZE+1];
	int recvbytes;
	int sendbytes;
	WSABUF wsabuf;
};

SOCKET client_sock;
HANDLE hReadEvent, hWriteEvent;

// 비동기 입출력 시작과 처리 함수
DWORD WINAPI WorkerThread(LPVOID arg);
void CALLBACK CompletionRoutine(
	DWORD dwError, DWORD cbTransferred,
	LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags
);
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

	// 이벤트 객체 생성
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if(hReadEvent == NULL) return 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(hWriteEvent == NULL) return 1;

	// 스레드 생성
	HANDLE hThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
	if(hThread == NULL) return 1;
	CloseHandle(hThread);

	while(1){
		WaitForSingleObject(hReadEvent, INFINITE);
		// accept()
		client_sock = accept(listen_sock, NULL, NULL);
		if(client_sock == INVALID_SOCKET){
			err_display(L"accept()");
			break;
		}
		SetEvent(hWriteEvent);
	}

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 비동기 입출력 시작 함수
DWORD WINAPI WorkerThread(LPVOID arg)
{
	int retval;
	char ipBuffer[INET_ADDRSTRLEN];

	while(1){
		while(1){
			// alertable wait
			DWORD result = WaitForSingleObjectEx(hWriteEvent, INFINITE, TRUE);
			if(result == WAIT_OBJECT_0) 
				break;
			if(result != WAIT_IO_COMPLETION) 
				return 1;
		}

		// 접속한 클라이언트 정보 출력
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);
		
		std::wcout << L"\nTCP 서버 클라이언트 접속: IP 주소=" << inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN) << L", 포트 번호=" << ntohs(clientaddr.sin_port) << L'\n';

		// 소켓 정보 구조체 할당과 초기화
		SOCKETINFO *ptr = new SOCKETINFO;
		if(ptr == NULL){
			std::wcout << L"메모리가 부족합니다\n";
			return 1;
		}
		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->sock = client_sock;
		SetEvent(hReadEvent);
		ptr->recvbytes = ptr->sendbytes = 0;
		ptr->wsabuf.buf = reinterpret_cast<char*>(ptr->buf);
		ptr->wsabuf.len = BUFSIZE;

		// 비동기 입출력 시작
		DWORD recvbytes;
		DWORD flags = 0;
		retval = WSARecv(ptr->sock, &ptr->wsabuf, 1, &recvbytes,&flags, &ptr->overlapped, CompletionRoutine);
		if(retval == SOCKET_ERROR){
			if(WSAGetLastError() != WSA_IO_PENDING){
				err_display(L"WSARecv()");
				return 1;
			}
		}
	}
	return 0;
}

// 비동기 입출력 처리 함수(입출력 완료 루틴)
void CALLBACK CompletionRoutine(DWORD dwError, DWORD cbTransferred,LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	int retval;

	// 클라이언트 정보 얻기
	SOCKETINFO *ptr = (SOCKETINFO *)lpOverlapped;
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);
	char ipBuffer[INET_ADDRSTRLEN];

	// 비동기 입출력 결과 확인
	if(dwError != 0 || cbTransferred == 0){
		if(dwError != 0) err_display(dwError);
		closesocket(ptr->sock);
		std::wcout << L"TCP 서버 클라이언트 종료: IP 주소="<< inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN)<< L", 포트 번호=" << ntohs(clientaddr.sin_port) << L'\n';
		delete ptr;
		return;
	}

	// 데이터 전송량 갱신
	if(ptr->recvbytes == 0){
		ptr->recvbytes = cbTransferred;
		ptr->sendbytes = 0;
		// 받은 데이터 출력
		ptr->buf[ptr->recvbytes] = '\0';
		std::wcout << L"TCP/" << inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN)<< L":" << ntohs(clientaddr.sin_port) << L": " << ptr->buf << L'\n';
	}
	else{
		ptr->sendbytes += cbTransferred;
	}

	if(ptr->recvbytes > ptr->sendbytes){
		// 데이터 보내기
		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->wsabuf.buf = reinterpret_cast<char*>(ptr->buf + ptr->sendbytes);
		ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;

		DWORD sendbytes;
		retval = WSASend(ptr->sock, &ptr->wsabuf, 1, &sendbytes,0, &ptr->overlapped, CompletionRoutine);
		if(retval == SOCKET_ERROR){
			if(WSAGetLastError() != WSA_IO_PENDING){
				err_display(L"WSASend()");
				return;
			}
		}
	}
	else{
		ptr->recvbytes = 0;

		// 데이터 받기
		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->wsabuf.buf = reinterpret_cast<char*>( ptr->buf);
		ptr->wsabuf.len = BUFSIZE;

		DWORD recvbytes;
		DWORD flags = 0;
		retval = WSARecv(ptr->sock, &ptr->wsabuf, 1, &recvbytes,&flags, &ptr->overlapped, CompletionRoutine);
		if(retval == SOCKET_ERROR){
			if(WSAGetLastError() != WSA_IO_PENDING){
				err_display(L"WSARecv()");
				return;
			}
		}
	}
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
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::wcout << L"[error] " << (wchar_t*)lpMsgBuf << L'\n';
	LocalFree(lpMsgBuf);
}