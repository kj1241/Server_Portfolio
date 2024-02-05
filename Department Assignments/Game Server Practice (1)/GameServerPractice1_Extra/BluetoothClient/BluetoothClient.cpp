#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2bth.h>
#include <Initguid.h>
#include <iostream>
#include <ws2tcpip.h>

#define SERVERADDR L"00:09:31:00:13:F9"
#define BUFSIZE    512

DEFINE_GUID(BthServer_Service, 0x4672de25, 0x588d, 0x48af,
	0x80, 0x73, 0x5f, 0x2b, 0x7b, 0x0, 0x60, 0x1f);

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
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) return 1;

	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale("kor"));

	// ������� ��ġ �˻�(����)
	// - ���ǻ� SERVERADDR ��ũ�� ����� ����Ѵ�.

	// ������� ���� �˻� �غ�
	DWORD qslen = sizeof(WSAQUERYSET);
	WSAQUERYSET *qs = (WSAQUERYSET *)malloc(qslen);
	ZeroMemory(qs, qslen);
	qs->dwSize = qslen;
	qs->dwNameSpace = NS_BTH;
	qs->lpServiceClassId = (GUID *)&BthServer_Service;
	qs->lpszContext = (LPWSTR)SERVERADDR;
	DWORD flags = LUP_FLUSHCACHE | LUP_RETURN_ADDR;
	
	// ������� ���� �˻� ����
	HANDLE hLookup;
	retval = WSALookupServiceBegin(qs, flags, &hLookup);
	if(retval == SOCKET_ERROR){
		std::cout << "�߰ߵ� ������� ��ġ ����!\n";
		exit(1);
	}

	// �˻��� ������� ���� ���� Ȯ��
	SOCKADDR_BTH *sa = NULL;
	int serverport = 0;
	bool done = false;
	while(!done){
		retval = WSALookupServiceNext(hLookup, flags, &qslen, qs);
		if(retval == NO_ERROR){
			sa = (SOCKADDR_BTH *)qs->lpcsaBuffer->RemoteAddr.lpSockaddr; // ������� ��ġ ������ ��� �ִ� ���� �ּ� ����ü�� ����
			serverport = sa->port; // ���� ��Ʈ ��ȣ ����
			break;
		}
		else{
			if(WSAGetLastError() == WSAEFAULT){
				free(qs);
				qs = (WSAQUERYSET *)malloc(qslen);
			}
			else{
				done = true;
			}
		}
	}
	if(sa == NULL){
		std::wcout << L"������� ��ġ" << SERVERADDR << L"���� ���� ���� ���� ����\n";
		exit(1);
	}

	// ������� ���� �˻� ����
	WSALookupServiceEnd(hLookup);
	free(qs);

	// socket()
	SOCKET sock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if(sock == INVALID_SOCKET) 
		err_quit(L"socket()");

	// connect()
	
	SOCKADDR_BTH serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	int addrlen = sizeof(serveraddr);
	//WSAStringToAddress(SERVERADDR, AF_BTH, NULL, (SOCKADDR*)&serveraddr, &addrlen);
	WSAStringToAddressW((LPWSTR)SERVERADDR, AF_BTH, NULL, (SOCKADDR*)&serveraddr, &addrlen);
	serveraddr.port = serverport;
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) 
		err_quit(L"connect()");

	// ������ ��ſ� ����� ����
	wchar_t buf[BUFSIZE+1];
	int len;

	// ������ ������ ���
	while(1){
		// ������ �Է�
		std::wcout << L"\n���� ������: ";
		if (fgetws(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = static_cast<int>(wcslen(buf));
		if (len > 0 && buf[len - 1] == L'\n')
			buf[len - 1] = L'\0';
		if (len == 0)
			break;

		// ������ ������
		retval = send(sock, reinterpret_cast<char*>(buf), len * sizeof(wchar_t), 0);
		if(retval == SOCKET_ERROR){
			err_display(L"send()");
			break;
		}
		std::wcout << L"������� Ŭ���̾�Ʈ: " << retval << L"����Ʈ�� ���½��ϴ�.\n";
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}