// ComTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
//#include "test_comapi_load.h"
//#include "test_comapi_speed.h"
//#include "test_comm.h"
#include "moxa/pcomm.h"
#include "serialport/serialport.h"

#pragma comment(lib, "moxa/pcomm")

#define HR_DEBUG_PRINT_PREFIX_MAX_SIZE 80	// max size of prefix string

// Global variables
static TCHAR _g_stringPrefix[HR_DEBUG_PRINT_PREFIX_MAX_SIZE] = { 0 };

BOOL __cdecl HR_PRINT_SET_PREFIX(__in LPCTSTR Prefix)
{
	errno_t result = _tcscpy_s(_g_stringPrefix, HR_DEBUG_PRINT_PREFIX_MAX_SIZE, Prefix);

	return (result == 0) ? TRUE : FALSE;
}

BOOL __cdecl HR_PRINT_GET_PREFIX(__out LPTSTR Prefix, __in size_t size)
{
	errno_t result = _tcsncpy_s(Prefix, size, _g_stringPrefix, HR_DEBUG_PRINT_PREFIX_MAX_SIZE);

	return (result == 0) ? TRUE : FALSE;
}

ULONG __cdecl HrDbgPrint(__in LPCTSTR Format, ...)
{
	va_list ap;
	ULONG n;
	TCHAR szData[356] = { 0 };

	lstrcat(szData, _g_stringPrefix);

	va_start(ap, Format);

	n = _vsntprintf_s(&szData[lstrlen(_g_stringPrefix)], 256, 256, Format, ap);

	va_end(ap);

	OutputDebugString(szData);

	return n;
}

ULONG __cdecl HR_PRINT_METHOD_BEGIN(__in LPCTSTR MethodName)
{
	return HrDbgPrint(_T("-> %s\n"), MethodName);
}

ULONG __cdecl HR_PRINT_METHOD_END(__in LPCTSTR MethodName)
{
	return HrDbgPrint(_T("<- %s\n"), MethodName);
}


SerialPort serialPort;

INT WriteBytes = 0;
int ReadBytes = 0;
bool flag = true;


#include <Windows.h>

DWORD WINAPI ReadProc(void*)
{
	char buf[1024];
	while (flag)
	{
		ReadBytes += serialPort.read(buf, 1024);
	}

	return 0;
}

DWORD WINAPI WriteProc(void*)
{
	char buf[1024];
	while (flag)
	{
		WriteBytes += serialPort.write(buf, 1024);
	}
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HR_PRINT_SET_PREFIX(_T("#> TEST"));

		// test serial port
		//test_comm();

		// test com api load
		//test_comapi_load();

		// test com api speed
		//test_comapi_speed();


		serialPort.set(115200, 8, "None", 1);

		CreateThread(NULL, 0, ReadProc, NULL, 0, NULL);
		CreateThread(NULL, 0, WriteProc, NULL, 0, NULL);


		while (1)
		{
			int c = getchar();
			if (c == 'p') {
				printf("RX:%d, TX:%d\n", ReadBytes, WriteBytes);
			}
			else if (c == 'q') {
				flag = false;
				break;
			}
		}

		system("pause");
		return 0;
}