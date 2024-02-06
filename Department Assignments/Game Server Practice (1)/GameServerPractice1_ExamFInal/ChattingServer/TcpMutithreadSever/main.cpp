#include "Sever.h"
int Sever::clientNumber = 0;
int Sever::allClientSocket[100];

int _tmain(int argc, _TCHAR* argv[])
{

	Sever *Sver = new Sever;
	try
	{

		Sver->SetSocket();
		Sver->SetBind();
		Sver->SetListen();
		Sver->MainLoop();
	}
	catch (char * errmsg)
	{
		cout << errmsg << endl;
		LPVOID lposMsg;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lposMsg, 0, NULL);
		cout << "[" << (char*)lposMsg << "]" << endl;
		LocalFree(lposMsg);
	}
	delete(Sver);
}