#ifndef STDAFX_H
#define STDAFX_H

#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include "resource.h"

#pragma comment(lib,"WS2_32.LIB")

#define WM_SOCKET_NOTIFY  (WM_USER +1)
#define CHAT_SERVER_IP	  "127.0.0.1"


#endif // !STDAFX_H