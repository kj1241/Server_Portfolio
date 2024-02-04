#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVERPORT 9000
#define BUFSIZE    512

//예전에는 goto문 썼는데 코드를 goto문 없이 바꿈

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
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) 
		err_quit(L"socket()");

	// 넌블로킹 소켓으로 전환
	u_long on = 1;
	retval = ioctlsocket(listen_sock, FIONBIO, &on);
	if(retval == SOCKET_ERROR) 
		err_quit(L"ioctlsocket()");

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

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	wchar_t buf[BUFSIZE+1];

	while (1) {
		// accept()
	/*ACCEPT_AGAIN:
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				goto ACCEPT_AGAIN;
			err_display(L"accept()");
			break;
		}*/

		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			err_display(L"accept()");
			break;
		}


		// 접속한 클라이언트 정보 출력
		char ipBuffer[INET_ADDRSTRLEN]; //이건 유니코드 못쓰는데?확인좀
		if (inet_ntop(AF_INET, &clientaddr.sin_addr, ipBuffer, INET_ADDRSTRLEN) != nullptr) {
			std::wcout << L"\nTCP 서버 클라이언트 접속: IP 주소=" << ipBuffer << L", 포트 번호=" << ntohs(clientaddr.sin_port) << L"\n";
		}
		else {
			std::wcerr << L"inet_ntop failed\n";
			break;
		}


		// 클라이언트와 데이터 통신
		while (1) {
			// 데이터 받기
	/*	RECEIVE_AGAIN:
			retval = recv(client_sock, reinterpret_cast<char*>(buf), BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAEWOULDBLOCK)
					goto RECEIVE_AGAIN;
				err_display(L"recv()");
				break;
			}
			else if (retval == 0)
				break;*/
			retval = recv(client_sock, reinterpret_cast<char*>(buf), BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK)
					continue;
				err_display(L"recv()");
				break;
			}
			else if (retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval / sizeof(wchar_t)] = L'\0';
			std::wcout << L"TCP " << ipBuffer << L"::" << ntohs(clientaddr.sin_port) << L": " << buf << L"\n";


			
		//	// 데이터 보내기
		//SEND_AGAIN:
		//	retval = send(client_sock, reinterpret_cast<char*>(buf), retval, 0);
		//	if (retval == SOCKET_ERROR) {
		//		if (WSAGetLastError() == WSAEWOULDBLOCK)
		//			goto SEND_AGAIN;
		//		err_display(L"send()");
		//		break;
		//	}
			// 데이터 보내기 
			int sentBytes = 0;
			while (sentBytes < retval)
			{
				retval = send(client_sock, reinterpret_cast<char*>(buf + sentBytes) , retval - sentBytes, 0);
				if (retval == SOCKET_ERROR)
				{
					if (WSAGetLastError() == WSAEWOULDBLOCK)
						continue;
					err_display(L"send()");
					break;
				}
				sentBytes += retval;
			}
		}

		// closesocket()
		closesocket(client_sock);
		std::wcout << L"TCP 서버 클라이언트 종료:  IP 주소=" << inet_ntop(AF_INET, &clientaddr.sin_addr, (char*)buf, INET_ADDRSTRLEN) << L", 포트 번호=" << ntohs(clientaddr.sin_port) << L"\n";
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}