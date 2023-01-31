#include "Thread.h"

int Thread::RemoveEndl(char* buf, int size)
{
	for (int i = 0; i < size; ++i)
	{
		if (buf[i] == '\n')
			return  i;
	}
	return -1;
}

void Thread::sendMessages(void * arg)
{
	int mSock = (int)arg;
	char tmpBuf[BUFFERSIZE];
	char nameBuf[NAMESIZE];
	char sndBuf[NAMESIZE + BUFFERSIZE];
	ZeroMemory(nameBuf, sizeof(nameBuf));
	ZeroMemory(tmpBuf, sizeof(tmpBuf));
	ZeroMemory(sndBuf, sizeof(sndBuf));

	cout << "종료를 입력하고 닫으려면 'Enter를 누르세요." << endl;
	cout << "닉네임을 적으세요"<<endl;
	fgets(nameBuf, NAMESIZE, stdin);
	nameBuf[RemoveEndl(nameBuf, NAMESIZE)] = '\0';


	sprintf(sndBuf, "%s,님이 로그인하셨습니다.", nameBuf);
	send(mSock, sndBuf, sizeof(sndBuf), 0);

	while (1)
	{
		ZeroMemory(tmpBuf, sizeof(tmpBuf));
		ZeroMemory(sndBuf, sizeof(sndBuf));
		fgets(tmpBuf, BUFFERSIZE, stdin);
		tmpBuf[RemoveEndl(tmpBuf, BUFFERSIZE)] = '\0';

		if (strncmp(strlwr(tmpBuf), "/bye", 4) == 0)
		{

			sprintf(sndBuf, "%s, 님이 로그아웃 하셨습니다..", nameBuf);
			send(mSock, sndBuf, sizeof(sndBuf), 0);
			closesocket(mSock);
			cout << "소켓연결이 닫혔습니다." << endl;
			break;

		}
		else
		{
			sprintf(sndBuf, "%s 님이 말하셨습니다.\n %s", nameBuf, tmpBuf);
			send(mSock, sndBuf, sizeof(sndBuf), 0);
		}
	}
}

void Thread::recvMessages(void * arg)
{
	int mSock = (int)arg;
	char rcvBuf[NAMESIZE + BUFFERSIZE];

	while (1)
	{
		if (recv(mSock, rcvBuf, sizeof(rcvBuf), 0) <= 0)
			break;
		else
		{
			//printf("%s\n", rcvBuf);
			cout << rcvBuf<<endl;
		}
	}
}

unsigned Thread::sendMessage(void * arg)
{
	Thread *threads = new Thread;
	threads->sendMessages(arg);

	delete(threads);
	return 0;
}

unsigned Thread::recvMessage(void * arg)
{
	Thread *threads = new Thread;
	threads->recvMessages(arg);

	delete(threads);
	return 0;
}
