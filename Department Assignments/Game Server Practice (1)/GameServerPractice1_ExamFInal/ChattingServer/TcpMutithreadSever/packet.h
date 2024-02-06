#ifndef PACKET_H
#define PACKET_H
#include "stdafx.h"


typedef union
{
	struct
	{
	public:

		char id[20];
		char password[20];
		bool flag;
	}Login_data;

	char buffer[41];
}LoginPacket;


typedef union
{
	struct
	{
	public:

		char id[20];
		char message[32];
		bool flag;
	}connect_data;

	char buffer[53];
}connectPacket;


typedef union
{
	struct
	{
	public:

		char id[20];
		char message[32];
		bool flag;
	}disconnect_data;

	char buffer[53];
}disconnectPacket;

#endif