#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>


#define SERVERPORT 9000
#define BUFSIZE    512

// ���� ���� ������ ���� ����ü�� ����
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

// ���� ���� �Լ�
BOOL AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);

// ���� ��� �Լ�
void err_quit(const wchar_t *msg);
void err_display(const wchar_t *msg);
void err_display(int errcode);

int main(int argc, char *argv[])
{
	int retval;

	// ���� �ʱ�ȭ
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

	// ���� ���� �߰� & WSAEventSelect()
	AddSocketInfo(listen_sock);
	retval = WSAEventSelect(listen_sock, EventArray[nTotalSockets-1],
		FD_ACCEPT|FD_CLOSE);
	if(retval == SOCKET_ERROR) 
		err_quit(L"WSAEventSelect()");

	// ������ ��ſ� ����� ����
	WSANETWORKEVENTS NetworkEvents;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int i, addrlen;

	while(1){
		// �̺�Ʈ ��ü �����ϱ�
		i = WSAWaitForMultipleEvents(nTotalSockets, EventArray,	FALSE, WSA_INFINITE, FALSE);
		if(i == WSA_WAIT_FAILED) continue;
		i -= WSA_WAIT_EVENT_0;

		// ��ü���� ��Ʈ��ũ �̺�Ʈ �˾Ƴ���
		retval = WSAEnumNetworkEvents(SocketInfoArray[i]->sock,	EventArray[i], &NetworkEvents);
		if(retval == SOCKET_ERROR) 
			continue;

		// FD_ACCEPT �̺�Ʈ ó��
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
				std::wcout << L"\nTCP ���� Ŭ���̾�Ʈ ����: IP �ּ�=" << ipBuffer << L", ��Ʈ ��ȣ=" << ntohs(clientaddr.sin_port) << L"\n";
			}
			else {
				std::wcerr << L"inet_ntop failed\n";
				break;
			}

			if(nTotalSockets >= WSA_MAXIMUM_WAIT_EVENTS){
				std::wcout << "�� �̻� ������ �޾Ƶ��� �� �����ϴ�\n";
				closesocket(client_sock);
				continue;
			}

			// ���� ���� �߰� & WSAEventSelect()
			AddSocketInfo(client_sock);
			retval = WSAEventSelect(client_sock, EventArray[nTotalSockets-1],FD_READ|FD_WRITE|FD_CLOSE);
			if(retval == SOCKET_ERROR) 
				err_quit(L"WSAEventSelect()");
		}

		// FD_READ & FD_WRITE �̺�Ʈ ó��
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
				// ������ �ޱ�
				retval = recv(ptr->sock, reinterpret_cast<char*>(ptr->buf), BUFSIZE, 0);
				if(retval == SOCKET_ERROR){
					if(WSAGetLastError() != WSAEWOULDBLOCK){
						err_display(L"recv()");
						RemoveSocketInfo(i);
					}
					continue;
				}
				ptr->recvbytes = retval;
				// ���� ������ ���
				ptr->buf[retval / sizeof(wchar_t)] = L'\0';
				addrlen = sizeof(clientaddr);
				getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);
				std::wcout << L"TCP " << ipBuffer << L"::" << ntohs(clientaddr.sin_port) << L": " << ptr->buf << L"\n";
			}

			if(ptr->recvbytes > ptr->sendbytes){
				// ������ ������
				retval = send(ptr->sock, reinterpret_cast<char*>(ptr->buf + ptr->sendbytes), ptr->recvbytes - ptr->sendbytes, 0);
				if(retval == SOCKET_ERROR){
					if(WSAGetLastError() != WSAEWOULDBLOCK){
						err_display(L"send()");
						RemoveSocketInfo(i);
					}
					continue;
				}
				ptr->sendbytes += retval;
				// ���� �����͸� ��� ���´��� üũ
				if(ptr->recvbytes == ptr->sendbytes)
					ptr->recvbytes = ptr->sendbytes = 0;
			}
		}

		// FD_CLOSE �̺�Ʈ ó��
		if(NetworkEvents.lNetworkEvents & FD_CLOSE){
			if(NetworkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
				err_display(NetworkEvents.iErrorCode[FD_CLOSE_BIT]);
			RemoveSocketInfo(i);
		}
	}

	// ���� ����
	WSACleanup();
	return 0;
}

// ���� ���� �߰�
BOOL AddSocketInfo(SOCKET sock)
{
	SOCKETINFO *ptr = new SOCKETINFO;
	if(ptr == NULL){
		std::wcout << "�޸𸮰� �����մϴ�\n";
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

// ���� ���� ����
void RemoveSocketInfo(int nIndex)
{
	SOCKETINFO *ptr = SocketInfoArray[nIndex];

	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);

	char ipBuffer[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN) != nullptr) {
		std::wcout << L"\nTCP ���� Ŭ���̾�Ʈ ����: IP �ּ�=" << ipBuffer << L", ��Ʈ ��ȣ=" << ntohs(clientaddr.sin_port) << L"\n";
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

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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
// ���� �Լ� ���� ���
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