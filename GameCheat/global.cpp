#include "public.h"

namespace global
{
	HWND hwndGame = nullptr;
	HWND hwndCurrent = nullptr;
	unsigned long uGamePid = 0;
	HANDLE hGameProcess = nullptr;
	char* pServer_css_Module = nullptr;
	char* pEngine_Module = nullptr;
}