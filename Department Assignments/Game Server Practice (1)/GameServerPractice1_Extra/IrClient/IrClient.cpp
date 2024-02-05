#define _WIN32_WINDOWS 0x0410 // ������ 98 �̻�
#define _WIN32_WINNT   0x0500 // ������ 2000 �̻�

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <af_irda.h>
#include <iostream>
#include <ws2tcpip.h>

#define BUFSIZE 512

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

// ����� ���� ������ ���� �Լ�
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

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) return 1;

	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale("kor"));

	// socket()
	SOCKET sock = socket(AF_IRDA, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
		err_quit(L"socket()");

	// �ִ� 10���� IrDA ��ġ �˻�
	struct MyDevList{
		ULONG numDevice;             // IrDA ��ġ ����
		IRDA_DEVICE_INFO Device[10]; // IrDA ��ġ ����
	} optval;
	optval.numDevice = 0;            // IrDA ��ġ ������ 0���� �ʱ�ȭ
	int optlen = sizeof(optval);
	retval = getsockopt(sock, SOL_IRLMP, IRLMP_ENUMDEVICES,(char *)&optval, &optlen);
	if(retval == SOCKET_ERROR) 
		err_quit(L"getsockopt");

	// ��ġ �˻� ��� ���
	printf("[IrDA Ŭ���̾�Ʈ] %d���� ���ܼ� ��ġ �߰�!\n", optval.numDevice);
	if(optval.numDevice == 0) exit(1);

	// ù ��° IrDA ��ġ�� ����
	SOCKADDR_IRDA serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.irdaAddressFamily = AF_IRDA;
	memcpy(&serveraddr.irdaDeviceID[0], &optval.Device[0].irdaDeviceID[0], 4);
	strcpy_s(serveraddr.irdaServiceName, "IrServer");
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) 
		err_quit(L"connect()");

	// ������ ��ſ� ����� ����
	wchar_t buf[BUFSIZE+1];
	int len;

	// ������ ������ ���
	while(1){
		// ������ �Է�
		ZeroMemory(buf, sizeof(buf));
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
		std::wcout << L"IrDA Ŭ���̾�Ʈ: " << retval << L"����Ʈ�� ���½��ϴ�.\n";

		// ������ �ޱ�
		retval = recvn(sock, reinterpret_cast<char*>(buf), retval, 0);
		if(retval == SOCKET_ERROR){
			err_display(L"recv()");
			break;
		}
		else if(retval == 0)
			break;

		// ���� ������ ���
		buf[retval] = '\0';
		std::wcout << L"TCP Ŭ���̾�Ʈ: " << retval << L"����Ʈ�� �޾ҽ��ϴ�.\n";
		std::wcout << L"���� ������: " << buf << L"\n";
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}