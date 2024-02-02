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

	//error 패킷 설계는했는데 왜 받는패킷이랑 보내는 패킷 분리를안해서 에러를 일으킴
	//어렴풋이 알고있었으면서
	//받는 패킷
	Header *recvHeaderPacket;
	Login_data *recvLoginPacket;
	message_data *recvMessagePacket;

	//보내는 패킷
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