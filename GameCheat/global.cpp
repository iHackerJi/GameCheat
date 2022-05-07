#include "public.h"

namespace global
{
	HWND hwndGame = nullptr;
	HWND hwndCurrent = nullptr;
	unsigned long uGamePid = 0;
	HANDLE hGameProcess = nullptr;
	char* pClient_Module = nullptr;
	char* pEngine_Module = nullptr;
	char* pClient_State = nullptr;
}