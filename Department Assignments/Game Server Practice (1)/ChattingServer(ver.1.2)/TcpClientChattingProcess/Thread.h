#ifndef THREAD_H
#define THREAD_H

#include "stdafx.h"
class Thread
{
protected:
	enum constant
	{
		PORT = 8080,
		BUFFERSIZE = 255,
		NAMESIZE = 10
	};
	int GetMessageBufSize;
	char messageBuffer[100];

	//error ��Ŷ ������ߴµ� �� �޴���Ŷ�̶� ������ ��Ŷ �и������ؼ� ������ ����Ŵ
	//���ǲ�� �˰��־����鼭
	//�޴� ��Ŷ
	Header *recvHeaderPacket;
	Login_data *recvLoginPacket;
	message_data *recvMessagePacket;

	//������ ��Ŷ
	Login_data* sendLoginPacket=new Login_data;
	message_data* sendMessagePacket=new message_data;



public:
	void sendMessages(void *arg);
	void recvMessages(void *arg);

	Thread();
	~Thread();

	static unsigned __stdcall sendMessage(void* arg);
	static unsigned __stdcall recvMessage(void* arg);
};



#endif