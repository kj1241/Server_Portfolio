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

	cout << "���Ḧ �Է��ϰ� �������� 'Enter�� ��������." << endl;
	cout << "�г����� ��������"<<endl;
	fgets(nameBuf, NAMESIZE, stdin);
	nameBuf[RemoveEndl(nameBuf, NAMESIZE)] = '\0';


	sprintf(sndBuf, "%s,���� �α����ϼ̽��ϴ�.", nameBuf);
	send(mSock, sndBuf, sizeof(sndBuf), 0);

	while (1)
	{
		ZeroMemory(tmpBuf, sizeof(tmpBuf));
		ZeroMemory(sndBuf, sizeof(sndBuf));
		fgets(tmpBuf, BUFFERSIZE, stdin);
		tmpBuf[RemoveEndl(tmpBuf, BUFFERSIZE)] = '\0';

		if (strncmp(strlwr(tmpBuf), "/bye", 4) == 0)
		{

			sprintf(sndBuf, "%s, ���� �α׾ƿ� �ϼ̽��ϴ�..", nameBuf);
			send(mSock, sndBuf, sizeof(sndBuf), 0);
			closesocket(mSock);
			cout << "���Ͽ����� �������ϴ�." << endl;
			break;

		}
		else
		{
			sprintf(sndBuf, "%s ���� ���ϼ̽��ϴ�.\n %s", nameBuf, tmpBuf);
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
