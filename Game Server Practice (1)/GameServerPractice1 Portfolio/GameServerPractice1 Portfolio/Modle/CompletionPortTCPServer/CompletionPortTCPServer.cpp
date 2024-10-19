#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#define SERVERPORT 9000
#define BUFSIZE    512

// ���� ���� ������ ���� ����ü
struct SOCKETINFO
{
	OVERLAPPED overlapped;
	SOCKET sock;
	wchar_t buf[BUFSIZE+1];
	int recvbytes;
	int sendbytes;
	WSABUF wsabuf;
};

// �۾��� ������ �Լ�
DWORD WINAPI WorkerThread(LPVOID arg);
// ���� ��� �Լ�
void err_quit(const wchar_t* msg);
void err_display(const wchar_t* msg);

int main(int argc, char *argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) 
		return 1;

	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale("kor"));

	// ����� �Ϸ� ��Ʈ ����
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if(hcp == NULL) 
		return 1;

	// CPU ���� Ȯ��
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// (CPU ���� * 2)���� �۾��� ������ ����
	HANDLE hThread;
	for(int i=0; i<(int)si.dwNumberOfProcessors*2; ++i){
		hThread = CreateThread(NULL, 0, WorkerThread, hcp, 0, NULL);
		if(hThread == NULL) return 1;
		CloseHandle(hThread);
	}

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

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	DWORD recvbytes, flags;
	char ipBuffer[INET_ADDRSTRLEN];

	while(1){
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET){
			err_display(L"accept()");
			break;
		}

		std::wcout << L"\nTCP ���� Ŭ���̾�Ʈ ����: IP �ּ�=" << inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN) << L", ��Ʈ ��ȣ=" << ntohs(clientaddr.sin_port) << L'\n';


		// ���ϰ� ����� �Ϸ� ��Ʈ ����
		CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);

		// ���� ���� ����ü �Ҵ�
		SOCKETINFO *ptr = new SOCKETINFO;
		if(ptr == NULL) break;
		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->sock = client_sock;
		ptr->recvbytes = ptr->sendbytes = 0;
		ptr->wsabuf.buf = reinterpret_cast<char*>(ptr->buf);
		ptr->wsabuf.len = BUFSIZE;

		// �񵿱� ����� ����
		flags = 0;
		retval = WSARecv(client_sock, &ptr->wsabuf, 1, &recvbytes,
			&flags, &ptr->overlapped, NULL);
		if(retval == SOCKET_ERROR){
			if(WSAGetLastError() != ERROR_IO_PENDING){
				err_display(L"WSARecv()");
			}
			continue;
		}
	}

	// ���� ����
	WSACleanup();
	return 0;
}

// �۾��� ������ �Լ�
DWORD WINAPI WorkerThread(LPVOID arg)
{
	int retval;
	HANDLE hcp = (HANDLE)arg;
	char ipBuffer[INET_ADDRSTRLEN];
	
	while(1){
		// �񵿱� ����� �Ϸ� ��ٸ���
		DWORD cbTransferred;
		SOCKET client_sock;
		SOCKETINFO *ptr;
		retval = GetQueuedCompletionStatus(hcp, &cbTransferred,(PULONG_PTR)&client_sock, (LPOVERLAPPED *)&ptr, INFINITE);

		// Ŭ���̾�Ʈ ���� ���
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);
		
		// �񵿱� ����� ��� Ȯ��
		if(retval == 0 || cbTransferred == 0){
			if(retval == 0){
				DWORD temp1, temp2;
				WSAGetOverlappedResult(ptr->sock, &ptr->overlapped,
					&temp1, FALSE, &temp2);
				err_display(L"WSAGetOverlappedResult()");
			}
			closesocket(ptr->sock);
			std::wcout << L"TCP ���� Ŭ���̾�Ʈ ����: IP �ּ�=" << inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN) << L", ��Ʈ ��ȣ=" << ntohs(clientaddr.sin_port) << L'\n';
			delete ptr;
			continue;
		}

		// ������ ���۷� ����
		if(ptr->recvbytes == 0){
			ptr->recvbytes = cbTransferred;
			ptr->sendbytes = 0;
			// ���� ������ ���
			ptr->buf[ptr->recvbytes] = '\0';
			std::wcout << L"TCP/" << inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN) << L":" << ntohs(clientaddr.sin_port) << L": " << ptr->buf << L'\n';
		}
		else{
			ptr->sendbytes += cbTransferred;
		}
		
		if(ptr->recvbytes > ptr->sendbytes){
			// ������ ������
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->wsabuf.buf = reinterpret_cast<char*>(ptr->buf + ptr->sendbytes);
			ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;

			DWORD sendbytes;
			retval = WSASend(ptr->sock, &ptr->wsabuf, 1,
				&sendbytes, 0, &ptr->overlapped, NULL);
			if(retval == SOCKET_ERROR){
				if(WSAGetLastError() != WSA_IO_PENDING){
					err_display(L"WSASend()");
				}
				continue;
			}			
		}
		else{
			ptr->recvbytes = 0;

			// ������ �ޱ�
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->wsabuf.buf = reinterpret_cast<char*>(ptr->buf);
			ptr->wsabuf.len = BUFSIZE;

			DWORD recvbytes;
			DWORD flags = 0;
			retval = WSARecv(ptr->sock, &ptr->wsabuf, 1, 
				&recvbytes, &flags, &ptr->overlapped, NULL);
			if(retval == SOCKET_ERROR){
				if(WSAGetLastError() != WSA_IO_PENDING){
					err_display(L"WSARecv()");
				}
				continue;
			}
		}
	}

	return 0;
}

// ���� �Լ� ���� ��� �� ����
void err_quit(const wchar_t*msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(const wchar_t*msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::wcout << L"[" << msg << L"] " << (wchar_t*)lpMsgBuf << L'\n';
	LocalFree(lpMsgBuf);
}