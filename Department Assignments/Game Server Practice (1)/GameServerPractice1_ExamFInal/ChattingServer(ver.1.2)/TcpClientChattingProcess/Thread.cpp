#include "Thread.h"

void Thread::sendMessages(void * arg)
{

	int mSock = (int)arg;
	while (1)
	{
		cout << endl;
		cout << "�޼����� ��������>>" << endl;
		cin >> sendMessagePacket->message;
		sendMessagePacket->flag = 1;
		
		if (strncmp(strlwr(sendMessagePacket->message), "/bye", 4) == 0)
		{
			sendMessagePacket->flag = 0;
			sendMessagePacket->packetType = disConnectPacket;
			sendMessagePacket->packetSize = sizeof(message_data);
			memcpy(sendMessagePacket->message, "���� ����", sizeof(sendMessagePacket->message));
			send(mSock, (char *)sendMessagePacket, sizeof(message_data), 0);
			cout << "���� ����";
			break;
		}

		else
		{
			sendMessagePacket->packetType = chatMessage;
			send(mSock, (char *)sendMessagePacket, sizeof(message_data), 0);
		}

	}
}

void Thread::recvMessages(void * arg)
{
	int mSock = (int)arg;
	while (1)
	{
		GetMessageBufSize = recv(mSock, messageBuffer, sizeof(message_data),0);
		if (GetMessageBufSize <= 0)
			break;
		else
		{
			messageBuffer[GetMessageBufSize] = '\0';
			recvHeaderPacket = (Header *)messageBuffer;
			if (recvHeaderPacket->packetType == connectPacket)
			{
				recvLoginPacket = (Login_data*)messageBuffer;
				cout << "ID: " << recvLoginPacket->id <<" ���� �����Ͽ����ϴ�." << endl;
				cout << endl;
			}
			if (recvHeaderPacket->packetType == chatMessage)
			{
				recvMessagePacket = (message_data*)messageBuffer;
				cout << "ID: " << recvMessagePacket->id << endl;
				cout << "�޼���: " << recvMessagePacket->message << endl;
				cout << endl;
			}
			if (recvHeaderPacket->packetType == disConnectPacket)
			{
				recvMessagePacket = (message_data*)messageBuffer;
				cout << "ID :" << recvMessagePacket->id <<" ���� ������ �����Ͽ����ϴ�." << endl;
			}
		}
	}

}

Thread::Thread()
{

}

Thread::~Thread()
{

	if (sendLoginPacket != nullptr)
	{
		delete sendLoginPacket;
		sendLoginPacket = nullptr;
	}
	if (sendMessagePacket != nullptr)
	{
		delete sendMessagePacket;
		sendMessagePacket = nullptr;
	}
}

unsigned Thread::sendMessage(void * arg)
{
	Thread *threads = new Thread;
	threads->sendMessages(arg);

	if (threads != nullptr)
	{
		delete(threads);
		threads = nullptr;
	}
	return 0;
}

unsigned Thread::recvMessage(void * arg)
{
	Thread *threads = new Thread;
	threads->recvMessages(arg);

	if (threads != nullptr)
	{
		delete(threads);
		threads = nullptr;
	}
	return 0;
}


