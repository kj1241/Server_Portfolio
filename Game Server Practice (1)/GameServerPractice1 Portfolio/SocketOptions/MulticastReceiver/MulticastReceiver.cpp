#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>


#define MULTICASTIP L"235.7.8.9"
#define LOCALPORT   9000
#define BUFSIZE     512

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
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET) 
		err_quit(L"socket()");	

	// SO_REUSEADDR �ɼ� ����
	BOOL optval = TRUE;
	retval = setsockopt(sock, SOL_SOCKET,
		SO_REUSEADDR, (char *)&optval, sizeof(optval));
	if(retval == SOCKET_ERROR) 
		err_quit(L"setsockopt()");

	// bind()
	SOCKADDR_IN localaddr;
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(LOCALPORT);
	retval = bind(sock, (SOCKADDR *)&localaddr, sizeof(localaddr));
	if(retval == SOCKET_ERROR) 
		err_quit(L"bind()");
	
	// ��Ƽĳ��Ʈ �׷� ����
	struct ip_mreq mreq;
	if (InetPton(AF_INET, MULTICASTIP, &mreq.imr_multiaddr.s_addr) == 1)
	{
		mreq.imr_interface.s_addr = htonl(INADDR_ANY);
		retval = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,(char*)&mreq, sizeof(mreq));
		if (retval == SOCKET_ERROR) 
			err_quit(L"setsockopt()");
	}
	else
	{
		err_quit(L"IPAddr()");
	}


	// ������ ��ſ� ����� ����
	SOCKADDR_IN peeraddr;
	int addrlen;
	wchar_t buf[BUFSIZE+1];

	// ��Ƽĳ��Ʈ ������ �ޱ�
	while(1){
		// ������ �ޱ�
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, reinterpret_cast<char*>(buf), BUFSIZE, 0,(SOCKADDR *)&peeraddr, &addrlen);
		if(retval == SOCKET_ERROR){
			err_display(L"recvfrom()");
			continue;
		}

		// ���� ������ ���
		buf[retval / sizeof(wchar_t)] = L'\0';

		char ipBuffer[INET_ADDRSTRLEN]; //�̰� �����ڵ� �����µ�?Ȯ����
		if (inet_ntop(AF_INET, &peeraddr.sin_addr, ipBuffer, INET_ADDRSTRLEN) != nullptr) {
			std::wcout << L"\nUDP: IP �ּ�=" << ipBuffer << L", ��Ʈ ��ȣ=" << ntohs(peeraddr.sin_port) << L": " << buf << L"\n";
		}
		else {
			std::wcerr << L"inet_ntop failed\n";
			break;
		}
	}

	// ��Ƽĳ��Ʈ �׷� Ż��
	retval = setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,(char *)&mreq, sizeof(mreq));
	if(retval == SOCKET_ERROR) 
		err_quit(L"setsockopt()");

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}