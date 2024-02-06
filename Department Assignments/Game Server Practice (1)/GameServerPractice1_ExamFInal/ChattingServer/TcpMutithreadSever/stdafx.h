#ifndef STDAFX_H
#define STDAFX_H

#pragma comment(lib,"ws2_32")
#include <iostream>
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <stdio.h>
#include <process.h>
#include <cstdio>
#include <iostream>
#include <process.h>
#include <cstdio>
#include <sstream>
#include <string.h>
#include "packet.h"
using namespace std;

enum constant
{
	PORT = 8080,
	BUFFERSIZE = 255,
	NAMESIZE = 10
};
static LoginPacket LPacket[100];
static connectPacket CPacket[100];
static disconnectPacket DPacket[100];



#endif // !1