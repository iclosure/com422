
#pragma once

#include "../comapi/comapi.h"
#include <Windows.h>

#if _DEBUG
#pragma comment(lib, "../Debug/comapi.lib")
#else
#pragma comment(lib, "../Release/comapi.lib")
#endif  // ! _DEBUG

DWORD WINAPI ThreadSend(LPVOID lpParam)
{
	while (TRUE)
	{

		Sleep(500);
	}
}

int test_comapi()
{
	HANDLE hThreadSend;

	hThreadSend = CreateThread(NULL, 0, ThreadSend, NULL, 0, NULL);

	getchar();

	CloseHandle(hThreadSend);

	return 0;
}