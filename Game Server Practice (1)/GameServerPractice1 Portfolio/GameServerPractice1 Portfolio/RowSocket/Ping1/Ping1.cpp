#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#define BUFSIZE 1500


/*
* 트레블 슈팅
gethostbyname -> getaddrinfo 로 변경

*/


// IP 헤더
typedef struct _IPHEADER
{
	u_char  ip_hl:4;  // header length
	u_char  ip_v:4;   // version
	u_char  ip_tos;   // type of service
	short   ip_len;   // total length
	u_short ip_id;    // identification
	short   ip_off;   // flags & fragment offset field
	u_char  ip_ttl;   // time to live
	u_char  ip_p;     // protocol
	u_short ip_cksum; // checksum
	IN_ADDR ip_src;   // source address
	IN_ADDR ip_dst;   // destination address
} IPHEADER;

// ICMP 메시지
typedef struct _ICMPMESSAGE
{
	u_char  icmp_type;  // type of message
	u_char  icmp_code;  // type sub code
	u_short icmp_cksum; // checksum
	u_short icmp_id;    // identifier
	u_short icmp_seq;   // sequence number
} ICMPMESSAGE;

#define ICMP_ECHOREQUEST 8
#define ICMP_ECHOREPLY   0

// ICMP 메시지 분석 함수
void DecodeICMPMessage(char *buf, int bytes, SOCKADDR_IN *from);
// 도메인 이름 -> IPv4 주소 변환 함수
//BOOL GetIPAddr(char *name, IN_ADDR *addr);
// 체크섬 계산 함수
u_short checksum(u_short *buffer, int size);
// 오류 출력 함수
void err_quit(const wchar_t *msg);
void err_display(const wchar_t *msg);

int wmain(int argc, wchar_t *argv[])
{
	int retval;

	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale("kor"));

	if(argc < 2){
		std::wcout << L"Useage: " << argv[0] << L"<host_name>\n";
		return 1;
	}

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) 
		return 1;

	// getaddrinfo 사용을 위한 힌트 구성
	//addrinfo hints;
	//ZeroMemory(&hints, sizeof(hints));
	//hints.ai_family = AF_INET;
	//hints.ai_socktype = SOCK_RAW;
	//hints.ai_protocol = IPPROTO_ICMP;

	ADDRINFOW hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;

	// getaddrinfo 호출
	//addrinfo* result = nullptr;
	//if (getaddrinfo(argv[1], nullptr, &hints, &result) != 0) {
	//	err_quit(L"getaddrinfo()");
	//	WSACleanup();
	//	return 1;
	//}
	PADDRINFOW result = nullptr;
	if (GetAddrInfoW(argv[1], nullptr, &hints, &result) != 0) {

		err_quit(L"getaddrinfo()");
		WSACleanup();
		return 1;
	}

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(sock == INVALID_SOCKET) 
		err_quit(L"socket()");


	// 소켓 옵션 설정
	int optval = 1000;
	retval = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,(char *)&optval, sizeof(optval));
	if(retval == SOCKET_ERROR) 
		err_quit(L"setsockopt()");

	retval = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,(char *)&optval, sizeof(optval));
	if(retval == SOCKET_ERROR) 
		err_quit(L"setsockopt()");

	// 소켓 주소 구조체 초기화
	SOCKADDR_IN destaddr;
	ZeroMemory(&destaddr, sizeof(destaddr));
	destaddr.sin_family = AF_INET;
	destaddr.sin_addr = ((sockaddr_in*)result->ai_addr)->sin_addr;

	// 데이터 통신에 사용할 변수
	ICMPMESSAGE icmpmsg;
	char buf[BUFSIZE];
	SOCKADDR_IN peeraddr;
	int addrlen;

	for(int i=0; i<4; ++i){
		// ICMP 메시지 초기화
		ZeroMemory(&icmpmsg, sizeof(icmpmsg));
		icmpmsg.icmp_type = ICMP_ECHOREQUEST;
		icmpmsg.icmp_code = 0;
		icmpmsg.icmp_id = (u_short)GetCurrentProcessId();
		icmpmsg.icmp_seq = i;
		icmpmsg.icmp_cksum = checksum((u_short *)&icmpmsg, sizeof(icmpmsg));

		// 에코 요청 ICMP 메시지 보내기
		retval = sendto(sock, (char *)&icmpmsg, sizeof(icmpmsg), 0,	(SOCKADDR *)&destaddr, sizeof(destaddr));
		if(retval == SOCKET_ERROR){
			if(WSAGetLastError() == WSAETIMEDOUT){
				std::wcout << L"Send timed out\n";
				continue;
			}
			err_display(L"sendto()");
			break;
		}

		// ICMP 메시지 받기
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, reinterpret_cast<char*>(buf), BUFSIZE, 0,(SOCKADDR *)&peeraddr, &addrlen);
		if(retval == SOCKET_ERROR){
			if(WSAGetLastError() == WSAETIMEDOUT){
				std::wcout << L"Request timed out!\n";
				continue;
			}
			err_display(L"recvfrom()");
			break;
		}

		// ICMP 메시지 분석
		DecodeICMPMessage(buf, retval, &peeraddr);

		Sleep(1000);
	}

	// closesocket()
	closesocket(sock);
	FreeAddrInfoW(result);
	// 윈속 종료
	WSACleanup();
	return 0;
}

// ICMP 메시지 분석 함수
void DecodeICMPMessage(char *buf, int len, SOCKADDR_IN *from)
{
	IPHEADER *iphdr = (IPHEADER *)buf;
	int iphdrlen = iphdr->ip_hl * 4;
	ICMPMESSAGE *icmpmsg = (ICMPMESSAGE *)(buf + iphdrlen);

	// 오류 체크
	if((len - iphdrlen) < 8){
		std::wcout << L"ICMP packet is too short\n";
		return;
	}
	if(icmpmsg->icmp_id != (u_short)GetCurrentProcessId()){
		std::wcout << L"Not a reponse to our echo request\n";
		return;
	}
	if(icmpmsg->icmp_type != ICMP_ECHOREPLY){
		std::wcout << L"Not a echo reply packet\n";
		return;
	}

	// 결과 출력
	std::wcout << L"Reply from" << inet_ntop(AF_INET, &from->sin_addr, (char*)buf, INET_ADDRSTRLEN) << ": total bytes = " << len << ", seq = " << icmpmsg->icmp_seq << "\n";
	return;
}

// 도메인 이름 -> IPv4 주소 변환 함수 (이거 더이상 안씀)
//BOOL GetIPAddr(char *name, IN_ADDR *addr)
//{
//	HOSTENT *ptr = gethostbyname(name);
//	if(ptr == NULL){
//		err_display(L"gethostbyname()");
//		return FALSE;
//	}
//	if(ptr->h_addrtype != AF_INET)
//		return FALSE;
//	memcpy(addr, ptr->h_addr, ptr->h_length);
//	return TRUE;
//}

// 체크섬 계산 함수
u_short checksum(u_short *buf, int len)
{
	u_long cksum = 0;
	u_short *ptr = buf;
	int left = len;

	while(left > 1){
		cksum += *ptr++;
		left -= sizeof(u_short);
	}

	if(left == 1)
		cksum += *(u_char *)buf;

	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return (u_short)(~cksum);
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