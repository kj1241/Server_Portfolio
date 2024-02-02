#include "Thread.h"

Threads::Threads()
{
	//clientNumber = 0;
}

void Threads::threadsFunction(void * arg)
{
	char cliBuf[NAMESIZE + BUFFERSIZE];
	int mClientSocket = (int)arg;
	int i;
	while (1)
	{
		ZeroMemory(cliBuf, sizeof(cliBuf));
		if (recv(mClientSocket, cliBuf, sizeof(cliBuf), 0) <= 0)
			break;
		
		cout<< cliBuf<<endl;
		
		WaitForSingleObject(&hMutex, INFINITE);
		
		for (i = 0; i < clientNumber; ++i)
		{
			if (allClientSocket[i] != mClientSocket)
				send(allClientSocket[i], cliBuf, sizeof(cliBuf), 0);
		}
		ReleaseMutex(&hMutex);
	}  

	WaitForSingleObject(&hMutex, INFINITE);

	for (i = 0; i < clientNumber; ++i)
	{
		if (allClientSocket[i] == mClientSocket)
		{
			for (; i < clientNumber; ++i)
				allClientSocket[i] = allClientSocket[i + 1];
			break;
		}
	}
	--clientNumber;
	cout<<"클라이언트 종료"<< clientNumber<<endl;
	ReleaseMutex(hMutex);
	closesocket(mClientSocket);
}

unsigned Threads::broadcastAllClient(void * arg)
{
	Threads *threads = new Threads;
	threads->threadsFunction(arg);

	delete(threads);
	return 0;
}


Threads::~Threads()
{
}


