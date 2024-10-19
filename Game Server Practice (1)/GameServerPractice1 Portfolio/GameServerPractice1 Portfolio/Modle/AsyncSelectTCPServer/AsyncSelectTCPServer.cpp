#pragma warning(disable : 4996)
#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVERPORT 9000
#define BUFSIZE    512
#define WM_SOCKET  (WM_USER+1)

// 소켓 정보 저장을 위한 구조체와 변수
struct SOCKETINFO
{
	SOCKET sock;
	wchar_t buf[BUFSIZE+1];
	int recvbytes;
	int sendbytes;
	BOOL recvdelayed;
	SOCKETINFO *next;
};

SOCKETINFO *SocketInfoList;

// 윈도우 메시지 처리 함수
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ProcessSocketMessage(HWND, UINT, WPARAM, LPARAM);
// 소켓 관리 함수
BOOL AddSocketInfo(SOCKET sock);
SOCKETINFO *GetSocketInfo(SOCKET sock);
void RemoveSocketInfo(SOCKET sock,wchar_t* buf);
// 오류 출력 함수
void err_quit(const wchar_t* msg);
void err_display(const wchar_t* msg);
void err_display(int errcode);

int main(int argc, char *argv[])
{
	int retval;

	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale("kor"));

	// 윈도우 클래스 등록
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = L"AsyncSelectTCPServer";
	if(!RegisterClass(&wndclass)) return 1;

	// 윈도우 생성
	HWND hWnd = CreateWindow(L"AsyncSelectTCPServer", L"AsyncSelectTCPServer", WS_OVERLAPPEDWINDOW,
		0, 0, 600, 200, NULL, NULL, NULL, NULL);
	if(hWnd == NULL) return 1;
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

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

	// WSAAsyncSelect()
	retval = WSAAsyncSelect(listen_sock, hWnd,WM_SOCKET, FD_ACCEPT|FD_CLOSE);
	if(retval == SOCKET_ERROR) 
		err_quit(L"WSAAsyncSelect()");

	// 메시지 루프
	MSG msg;
	while(GetMessage(&msg, 0, 0, 0) > 0){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// 윈속 종료
	WSACleanup();
	return msg.wParam;
}

// 윈도우 메시지 처리
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg){
	case WM_SOCKET: // 소켓 관련 윈도우 메시지
		ProcessSocketMessage(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// 소켓 관련 윈도우 메시지 처리
void ProcessSocketMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 데이터 통신에 사용할 변수
	SOCKETINFO *ptr;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen, retval;
	char ipBuffer[INET_ADDRSTRLEN];

	// 오류 발생 여부 확인
	if(WSAGETSELECTERROR(lParam)){
		err_display(WSAGETSELECTERROR(lParam));
		RemoveSocketInfo(wParam, nullptr);
		return;
	}

	// 메시지 처리
	switch(WSAGETSELECTEVENT(lParam)){
	case FD_ACCEPT:
		addrlen = sizeof(clientaddr);
		client_sock = accept(wParam, (SOCKADDR *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET){
			err_display(L"accept()");
			return;
		}

		
		if (inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN) != nullptr) {
			std::wcout << L"\nTCP 서버 클라이언트 접속: IP 주소=" << ipBuffer << L", 포트 번호=" << ntohs(clientaddr.sin_port) << L"\n";
		}
		else {
			std::wcerr << L"inet_ntop failed\n";
			break;
		}

		
		retval = WSAAsyncSelect(client_sock, hWnd,WM_SOCKET, FD_READ|FD_WRITE|FD_CLOSE);
		if(retval == SOCKET_ERROR){
			err_display(L"WSAAsyncSelect()");
			RemoveSocketInfo(client_sock, nullptr);
		}
		break;
	case FD_READ:
		ptr = GetSocketInfo(wParam);
		if(ptr->recvbytes > 0){
			ptr->recvdelayed = TRUE;
			return;
		}
		// 데이터 받기
		retval = recv(ptr->sock, reinterpret_cast<char*>(ptr->buf), BUFSIZE, 0);
		if(retval == SOCKET_ERROR){
			err_display(L"recv()");
			RemoveSocketInfo(wParam, ptr->buf);
			return;
		}
		ptr->recvbytes = retval;
		// 받은 데이터 출력
		ptr->buf[retval / sizeof(wchar_t)] = L'\0';
		addrlen = sizeof(clientaddr);
		getpeername(wParam, (SOCKADDR *)&clientaddr, &addrlen);
		std::wcout << L"TCP " << ipBuffer << L"::" << ntohs(clientaddr.sin_port) << L": " << ptr->buf << L"\n";

	case FD_WRITE:
		ptr = GetSocketInfo(wParam);
		if(ptr->recvbytes <= ptr->sendbytes)
			return;
		// 데이터 보내기
		retval = recv(ptr->sock, reinterpret_cast<char*>(ptr->buf + ptr->sendbytes), ptr->recvbytes - ptr->sendbytes, 0);
		if(retval == SOCKET_ERROR){
			err_display(L"send()");
			RemoveSocketInfo(wParam, ptr->buf);
			return;
		}
		ptr->sendbytes += retval;
		// 받은 데이터를 모두 보냈는지 체크
		if(ptr->recvbytes == ptr->sendbytes){
			ptr->recvbytes = ptr->sendbytes = 0;
			if(ptr->recvdelayed){
				ptr->recvdelayed = FALSE;
				PostMessage(hWnd, WM_SOCKET, wParam, FD_READ);
			}
		}
		break;
	case FD_CLOSE:
		RemoveSocketInfo(wParam, nullptr);
		break;
	}
}

// 소켓 정보 추가
BOOL AddSocketInfo(SOCKET sock)
{
	SOCKETINFO *ptr = new SOCKETINFO;
	if(ptr == NULL){
		std::cout << "메모리가 부족합니다!\n";
		return FALSE;
	}

	ptr->sock = sock;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;
	ptr->recvdelayed = FALSE;
	ptr->next = SocketInfoList;
	SocketInfoList = ptr;

	return TRUE;
}

// 소켓 정보 얻기
SOCKETINFO *GetSocketInfo(SOCKET sock)
{
	SOCKETINFO *ptr = SocketInfoList;

	while(ptr){
		if(ptr->sock == sock)
			return ptr;
		ptr = ptr->next;
	}

	return NULL;
}

// 소켓 정보 제거
void RemoveSocketInfo(SOCKET sock, wchar_t* buf)
{
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(sock, (SOCKADDR *)&clientaddr, &addrlen);

	if (buf == nullptr)
		std::wcout << L"TCP 서버 클라이언트 접속에러\n";
	else
		std::wcout << L"TCP 서버 클라이언트 종료:  IP 주소=" << inet_ntop(AF_INET, &clientaddr.sin_addr, (char*)buf, INET_ADDRSTRLEN) << L", 포트 번호=" << ntohs(clientaddr.sin_port) << L"\n";

	SOCKETINFO *curr = SocketInfoList;
	SOCKETINFO *prev = NULL;

	while(curr){
		if(curr->sock == sock){
			if(prev)
				prev->next = curr->next;
			else
				SocketInfoList = curr->next;
			closesocket(curr->sock);
			delete curr;
			return;
		}
		prev = curr;
		curr = curr->next;
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

	std::wcout << L"[ error ] " << (wchar_t*)lpMsgBuf << L'\n';
	LocalFree(lpMsgBuf);
}