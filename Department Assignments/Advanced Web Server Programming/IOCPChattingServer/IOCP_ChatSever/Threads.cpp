#include "stdafx.h"
#include "Threads.h"

Threads *Threads::pThreads=nullptr;

Threads::Threads()
{
	pThreads = this;
}

Threads::~Threads()
{
	pThreads = nullptr;
}

unsigned __stdcall Threads::CommThread(void* pArguments)
{
	threadData = (THREADDATA*)pArguments;
	
	hDlg = threadData->hDlg;
	hIOCP = threadData->hIOCP;
	lstnsock = threadData->lstnsock;
	acceptBuf = threadData->acceptBuf;
	AOV = threadData->AOV;
	listOverlapped = threadData->listOverlapped;
	cs = threadData->cs;
	threadCount = threadData->threadCount;

	InterlockedIncrement(threadCount); //동기화 1씩증가
	__try
	{
		__try
		{
			return InnerCommThread();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
	__finally
	{
		InterlockedDecrement(threadCount); //감소;
	}

	return 0;
}

unsigned __stdcall Threads::InnerCommThread(void)
{
	try
	{
		while (1)
		{
			DWORD cbTransferred;
			SOCKET sock;
			WSAOVERLAPPED* pOV = NULL;

			int nRet = GetQueuedCompletionStatus(hIOCP, &cbTransferred, (LPDWORD)&sock, (LPOVERLAPPED*)&pOV, INFINITE);
			if (sock == lstnsock)
			{
				ACCEPTOVERLAPPED* pAOV = (ACCEPTOVERLAPPED*)pOV;
				OnAccept(pAOV->commsock, acceptBuf);
				SOCKET commsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (commsock != INVALID_SOCKET)
				{
					AOV.commsock = commsock;
					DWORD dwBytes;
					AcceptEx(lstnsock, commsock, acceptBuf, 0,
						sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
						&dwBytes, (WSAOVERLAPPED*)&AOV);
				}
				continue;
			}

			if (sock == 0)
				break;

			char buf[128] = { 0 };
			wsprintf(buf, "sock = %d\n", sock);
			OutputDebugString(buf);

			if (cbTransferred == 0)
			{
				OnClose((OVERLAPPEDSOCK*)pOV);
				continue;
			}

			OVERLAPPEDSOCK* pMOV = (OVERLAPPEDSOCK*)pOV;
			OnRead(pMOV, pMOV->wsaBuf.buf, cbTransferred);
		}
	}
	catch (char* errmsg)
	{
		LPVOID lpOSMsg;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpOSMsg, 0, NULL);
		MessageBox(hDlg, (LPCTSTR)lpOSMsg, errmsg, MB_OK);
		LocalFree(lpOSMsg);
	}

	return 0;
}

unsigned __stdcall Threads::communicationThread(void* pArguments)
{
	return pThreads->CommThread(pArguments);
}

void Threads::OnAccept(SOCKET commsock, char* AcceptBuf)
{
	OVERLAPPEDSOCK* pMOV = new OVERLAPPEDSOCK;
	ZeroMemory(&pMOV->ov, sizeof(WSAOVERLAPPED));
	pMOV->wsaBuf.buf = new char[1024];
	pMOV->wsaBuf.len = 1024;
	pMOV->commsock = commsock;

	CreateIoCompletionPort((HANDLE)commsock, hIOCP, (DWORD)commsock, 0);

	DWORD dwBytes, dwFlags = 0;
	WSARecv(commsock, &pMOV->wsaBuf, 1, &dwBytes, &dwFlags,(WSAOVERLAPPED*)pMOV, NULL);

	EnterCriticalSection(&cs);
	listOverlapped.push_back(pMOV);
	LeaveCriticalSection(&cs);

	struct sockaddr_in* cliaddr;
	int nlen = sizeof(cliaddr);
	GetAcceptExSockaddrs(AcceptBuf, 0,	sizeof(sockaddr_in) + 16, 0, (sockaddr**)&cliaddr, &nlen,	NULL, NULL);

	char msg[128] = { 0 };
	wsprintf(msg, "접속자:(%s), 사용자수:%d",inet_ntoa(cliaddr->sin_addr), listOverlapped.size());
	AddString(hDlg, IDC_CONN_LIST, msg);
}

void Threads::AddString(HWND hDlg, UINT nID, char* msg)
{
	HWND hList = GetDlgItem(hDlg, nID);
	int n = (int)SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)msg);
	SendMessage(hList, LB_SETTOPINDEX, n, 0);
}

void Threads::OnRead(OVERLAPPEDSOCK* pMOV, char* buf, int nRead)
{
	EnterCriticalSection(&cs);
	list<OVERLAPPEDSOCK*>::iterator it = listOverlapped.begin();
	while (it != listOverlapped.end())
	{
		OVERLAPPEDSOCK* pMOV = *it;
		send(pMOV->commsock, buf, nRead, 0);
		it++;
	}
	LeaveCriticalSection(&cs);

	DWORD dwFlags = 0, dwBytes;
	WSARecv(pMOV->commsock, &pMOV->wsaBuf, 1, &dwBytes, &dwFlags,(WSAOVERLAPPED*)pMOV, NULL);
}

void Threads::OnClose(OVERLAPPEDSOCK* pMOV)
{
	EnterCriticalSection(&cs);
	listOverlapped.remove(pMOV);
	LeaveCriticalSection(&cs);

	closesocket(pMOV->commsock);
	delete pMOV->wsaBuf.buf;;
	delete pMOV;

	char msg[128] = { 0 };
	wsprintf(msg, "사용자수:%d", listOverlapped.size());
	AddString(hDlg, IDC_CONN_LIST, msg);
}

