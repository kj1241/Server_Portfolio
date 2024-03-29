#include "Thread.h"

void Thread::sendMessages(void * arg)
{

	int mSock = (int)arg;
	while (1)
	{
		cout << endl;
		cout << "메세지를 적으세요>>" << endl;
		cin >> sendMessagePacket->message;
		sendMessagePacket->flag = 1;
		
		if (strncmp(strlwr(sendMessagePacket->message), "/bye", 4) == 0)
		{
			sendMessagePacket->flag = 0;
			sendMessagePacket->packetType = disConnectPacket;
			sendMessagePacket->packetSize = sizeof(message_data);
			memcpy(sendMessagePacket->message, "연결 해제", sizeof(sendMessagePacket->message));
			send(mSock, (char *)sendMessagePacket, sizeof(message_data), 0);
			cout << "연결 종료";
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
				cout << "ID: " << recvLoginPacket->id <<" 님이 접속하였습니다." << endl;
				cout << endl;
			}
			if (recvHeaderPacket->packetType == chatMessage)
			{
				recvMessagePacket = (message_data*)messageBuffer;
				cout << "ID: " << recvMessagePacket->id << endl;
				cout << "메세지: " << recvMessagePacket->message << endl;
				cout << endl;
			}
			if (recvHeaderPacket->packetType == disConnectPacket)
			{
				recvMessagePacket = (message_data*)messageBuffer;
				cout << "ID :" << recvMessagePacket->id <<" 님이 연결을 해제하였습니다." << endl;
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


