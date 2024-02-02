#ifndef STDAFX_H
#define STDAFX_H

#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <process.h>
#include <cstdlib>
#include <list>
#include <tchar.h>
#include "resource.h"
#pragma comment(lib,"WS2_32.LIB")
#pragma comment(lib,"Mswsock.lib")


#pragma warning(disable : 4996)
using namespace std;


struct OVERLAPPEDSOCK
{
	WSAOVERLAPPED ov;
	WSABUF		  wsaBuf;
	SOCKET		  commsock;
};
struct ACCEPTOVERLAPPED
{
	WSAOVERLAPPED ov;
	SOCKET		  commsock;
};
struct THREADDATA //공유자원
{
	long* threadCount;
	HWND hDlg;
	HANDLE hIOCP;
	SOCKET lstnsock;
	char* acceptBuf;
	ACCEPTOVERLAPPED AOV;
	list<OVERLAPPEDSOCK*> listOverlapped;
	CRITICAL_SECTION cs;
};
#endif // !STDAFX_H
