#include "stdafx.h"
#include "Server.h"

int main()
{
	Server* sv = new Server;
	try
	{
		sv->CheckIP();
		sv->SetSocket();
		sv->MakeIOCP();
		sv->ConfirmSystem();
		sv->SetBind();
		sv->SetListen();
		sv->MainLoop();

	}
	catch (char* errmsg)
	{
		cout << errmsg << endl;
		LPVOID lposMsg;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lposMsg, 0, NULL);
		cout << "[" << (char*)lposMsg << "]" << endl;
		LocalFree(lposMsg);
	}
	delete(sv);
}