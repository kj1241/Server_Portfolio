#include "Thread.h"


Threads::Threads()
{
	//clientNumber = 0;
}

void Threads::threadsFunction(void * arg)
{
	int mClientSocket = (int)arg;


	for (int i = 0; i < clientNumber; ++i)
	{
		if (allClientSocket[i] == mClientSocket)
		{
			number = i;
			break;
		}
	}
	

	while (1)
	{
		GetMessageBufSize = recv(mClientSocket, messageBuffer, sizeof(message_data), 0);
		//유일하게 받는곳
		if (GetMessageBufSize <= 0) //강제 종료
			break;
		else
		{
			messageBuffer[GetMessageBufSize] = '\0';
			recvMessagePacket = (message_data *)messageBuffer;
			memcpy(recvMessagePacket->id, Session::UserId[number], sizeof(recvMessagePacket->id));

			curr_time = time(NULL);
			curr_tm = localtime(&curr_time);
			cout << curr_tm->tm_year + 1900 << "년 " << curr_tm->tm_mon + 1 << "일 " << curr_tm->tm_mday << "일" << endl;
			cout << curr_tm->tm_hour << "시 " << curr_tm->tm_min << "분 " << curr_tm->tm_sec << "초" << endl;
			
			cout << "id:" << recvMessagePacket->id << endl;
			cout << "메시지:" <<recvMessagePacket->message << endl;
			cout << endl;
		}
		WaitForSingleObject(&hMutex, INFINITE);
		if (recvMessagePacket->flag == 1)
		{
			for (int i = 0; i < clientNumber; ++i)
			{
				if (allClientSocket[i] != mClientSocket)
				{
					sendMessagePacket->packetType = chatMessage;
					sendMessagePacket->packetSize = recvMessagePacket->packetSize;
					memcpy(sendMessagePacket->id, Session::UserId[number], sizeof(sendMessagePacket->id));
					memcpy(sendMessagePacket->message, recvMessagePacket->message, sizeof(sendMessagePacket->message));
					sendMessagePacket->flag = recvMessagePacket->flag;
					sendMessagePacket->endMark = recvMessagePacket->endMark;
					send(allClientSocket[i], (char*)sendMessagePacket, sizeof(message_data), 0);
				}
			}
		}
		else //연결해제되면
		{
			for (int i = 0; i < clientNumber; ++i)
			{
				if (i == number)
					continue;
				
				sendMessagePacket->packetType = disConnectPacket;
				sendMessagePacket->packetSize = recvMessagePacket->packetSize;
				memcpy(sendMessagePacket->id, Session::UserId[number], sizeof(sendMessagePacket->id));
				memcpy(recvMessagePacket->message, "연결해제", sizeof(recvMessagePacket->message));
				sendMessagePacket->flag = recvMessagePacket->flag;
				sendMessagePacket->endMark = recvMessagePacket->endMark;
				send(allClientSocket[i], (char *)sendMessagePacket, sizeof(message_data), 0);
			}
			break; //while문 탈출
		}
		ReleaseMutex(&hMutex);
	}   //while break when disconnected

	WaitForSingleObject(&hMutex, INFINITE);

	for (int i = 0; i < clientNumber; ++i)
	{
		if (allClientSocket[i] == mClientSocket)
		{
			for (; i < clientNumber; ++i)
				allClientSocket[i] = allClientSocket[i + 1];
			break;
		}
	}
	--clientNumber;
	cout << "클라이언트 종료" << clientNumber << endl;
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
	if (sendLoginPacket!= nullptr)
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


