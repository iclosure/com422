
#pragma once

#include "../ComApi/comapi.h"
#include "../ComApi/comserial.h"
#include <Windows.h>

#if defined(_DEBUG)
#pragma comment(lib, "../Debug/comapi.lib")
#else
#pragma comment(lib, "../Release/comapi.lib")
#endif  // ! _DEBUG

DWORD WINAPI LoadThreadSend(LPVOID lpParam)
{
	while (TRUE)
	{

		Sleep(500);
	}
}

int test_comapi_load()
{
	HANDLE hThreadSend;

	hThreadSend = CreateThread(NULL, 0, LoadThreadSend, NULL, 0, NULL);

	getchar();

	CloseHandle(hThreadSend);

	return 0;
}