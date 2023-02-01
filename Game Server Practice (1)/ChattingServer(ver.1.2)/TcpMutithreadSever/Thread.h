#ifndef  THREAD_H
#define THREAD_H

#include "stdafx.h"
#include "Session.h"
#include "packet.h"

class Threads
{
protected:
	enum constant
	{
		PORT = 8080,
		BUFFERSIZE = 255,
		NAMESIZE = 10
	};
	unsigned threadID;
	HANDLE hMutex;
    static int allClientSocket[100];
    static int clientNumber;

	int number = 0;
	time_t curr_time;
	struct tm *curr_tm;

	int GetMessageBufSize;
	char messageBuffer[100];

	Login_data* recvLoginPacket;
	message_data* recvMessagePacket;

	Login_data* sendLoginPacket = new Login_data;
	message_data* sendMessagePacket = new message_data;



public :
	Threads();

	void threadsFunction(void *arg);
	static unsigned __stdcall broadcastAllClient(void *arg);

	

	~Threads();

};







#endif

