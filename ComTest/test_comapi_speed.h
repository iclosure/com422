
#pragma once

#include "../ComApi/comserial.h"
#include <Windows.h>
#include <stdio.h>
#include <assert.h>

#if defined(_DEBUG)
#pragma comment(lib, "../Debug/comapi.lib")
#else
#pragma comment(lib, "../Release/comapi.lib")
#endif  // ! _DEBUG

typedef struct _COMM_TRANSMIT_BUFFER
{
	long		size;
	char*		bufferSend;
	char*		bufferRecv;
	CSerialPort	serialport;
	PreciTimer	preciTimer;
	LONGLONG	intervalMSSend;
	LONGLONG	intervalMSRecv;
	long		transmitMaxSize;

	DWORD		sleepSend;
	DWORD		sleepRecv;

	FILE *		fp;

} COMM_TRANSMIT_BUFFER, *PCOMM_TRANSMIT_BUFFER;

DWORD WINAPI SpeedThreadSend(LPVOID lpParam)
{
	PCOMM_TRANSMIT_BUFFER transmitBuffer = reinterpret_cast<PCOMM_TRANSMIT_BUFFER>(lpParam);

	printf_s("start thread send\n");

	PreciTimer preciTimer;
	long count = 0;
	int sendCount = 0;
	int loopCount = 0;

	preciTimer.initialize();

	while (TRUE)
	{
		int ret = transmitBuffer->serialport.write(transmitBuffer->bufferSend, transmitBuffer->size);
		sendCount = ret;
		if (sendCount > 0)
		{
			count += sendCount;
			if (count % (transmitBuffer->size * 20) == 0)
			{
				printf_s("send: %8ld, count: %8ld\n", sendCount, count);
			}
			if (count >= transmitBuffer->transmitMaxSize)
			{
				break;
			}
		}
		else
		{
			int i = 0;
			i++;
		}

		Sleep(transmitBuffer->sleepSend);

		loopCount++;
	}

	transmitBuffer->intervalMSSend = preciTimer.intervalUS() - transmitBuffer->sleepSend * loopCount * 1000;

	printf_s("send -> count: %ld, loopCount: %d, interval: %ld us\n", count, loopCount, transmitBuffer->intervalMSSend);

	printf_s("end thread send\n");

	system("pause");

	return TRUE;
}

DWORD WINAPI SpeedThreadRecv(LPVOID lpParam)
{
	PCOMM_TRANSMIT_BUFFER transmitBuffer = reinterpret_cast<PCOMM_TRANSMIT_BUFFER>(lpParam);

	printf_s("start receive thread\n");

	PreciTimer preciTimer;
	long count = 0;
	int recvCount = 0;
	int loopCount = 0;

	while (TRUE)
	{
		int ch = transmitBuffer->serialport.read();
		if (ch == CCOM_SUCCESS && ch == 'z')
		{
			count ++;
			break;
		}
		Sleep(1);
	}

	preciTimer.initialize();

	while (TRUE)
	{
		recvCount = transmitBuffer->serialport.read(transmitBuffer->bufferRecv, transmitBuffer->size);
		if (recvCount > 0)
		{
			count += recvCount;
			if (count % (transmitBuffer->size * 50) == 0)
			{
				printf_s("recv: %8ld, count: %8ld\n", recvCount, count);
			}
			if (count >= transmitBuffer->transmitMaxSize)
			{
				//break;
				transmitBuffer->intervalMSRecv = preciTimer.intervalUS() - transmitBuffer->sleepRecv * loopCount * 1000;
				printf_s("receive -> count: %ld, loopCount: %d, interval: %ld us\n", count, loopCount, transmitBuffer->intervalMSRecv);
			}
		}

		Sleep(transmitBuffer->sleepRecv);

		loopCount++;
	}

	transmitBuffer->intervalMSRecv = preciTimer.intervalUS() - transmitBuffer->sleepRecv * loopCount * 1000;

	printf_s("receive -> count: %ld, loopCount: %d, interval: %ld us\n", count, loopCount, transmitBuffer->intervalMSRecv);

	printf_s("end thread receive\n");
	
	return TRUE;
}

int test_comapi_speed()
{
	DWORD result = NULL;
	HANDLE hThreadSend = NULL;
	HANDLE hThreadRecv = NULL;
	PreciTimer preciTimer;
	COMM_TRANSMIT_BUFFER transmitBuffer = { 0 };
	int port = 26;
	errno_t error = 0;

	transmitBuffer.size = 4096;
	transmitBuffer.bufferSend = new char[transmitBuffer.size];
	transmitBuffer.bufferRecv = new char[transmitBuffer.size];

	ZeroMemory(transmitBuffer.bufferSend, transmitBuffer.size);
	ZeroMemory(transmitBuffer.bufferRecv, transmitBuffer.size);

	unsigned char ch_cnt = 0;
	for (LONG i = 0; i < transmitBuffer.size; i++)
	{
		transmitBuffer.bufferSend[i] = ch_cnt++;
	}

	transmitBuffer.transmitMaxSize = transmitBuffer.size * 1000;

	printf_s("transmit count: %ld\n", transmitBuffer.size);
	printf_s("Please input the port number (1 ~ 256) of serial port: ");
	scanf_s("%d", &port);

	if (port < 1 || port > 256)
	{
		printf_s("the number (%d) if invalid!\n", port);
		goto label_exit;
	}

	// open serial port
	if (!transmitBuffer.serialport.initialize(port, true, 115200))
	{
		printf_s("error: open COM%d failed!\n", port);
		goto label_exit;
	}

	// get data from file test_data.txt
	transmitBuffer.fp = NULL;
	error = fopen_s(&transmitBuffer.fp, "test_save.txt", "w");
	if (error < 0)
	{
		printf_s("error: open file test_save.txt failed!\n");
		goto label_exit;
	}

	
	transmitBuffer.sleepSend = 1;
	transmitBuffer.sleepRecv = 1;

	hThreadSend = CreateThread(NULL, 0, SpeedThreadSend, &transmitBuffer, CREATE_SUSPENDED, NULL);
	hThreadRecv = CreateThread(NULL, 0, SpeedThreadRecv, &transmitBuffer, CREATE_SUSPENDED, NULL);

	result = ResumeThread(hThreadSend);
	result = ResumeThread(hThreadRecv);

	HANDLE handles[2] = { hThreadSend, hThreadRecv };
	preciTimer.initialize();
	result = WaitForMultipleObjects(2, handles, TRUE, 60 * 1000); // wait ? ms
	LONGLONG interval = preciTimer.intervalUS();
	switch(result)
	{
	case WAIT_OBJECT_0:
		printf_s("wait thread send: %ld us\n", interval);
		break;
	case WAIT_OBJECT_0 + 1:
		printf_s("wait thread receive: %ld us\n", interval);
		break;
	case WAIT_TIMEOUT:
		printf_s("wait thread timeout: %ld us\n", interval);
		break;
	default: break;
	}
	
	/*for (int i = 0; i < 1; i++)
	{
		int ret = transmitBuffer.serialport.write(transmitBuffer.bufferSend, transmitBuffer.size);
		printf_s("send count: %d\n", ret);
	}*/

label_exit:

	system("pause");

	transmitBuffer.serialport.close();

	if (transmitBuffer.bufferSend != NULL)
	{
		delete[] transmitBuffer.bufferSend;
	}
	if (transmitBuffer.bufferRecv != NULL)
	{
		delete[] transmitBuffer.bufferRecv;
	}
	if (hThreadSend != NULL)
	{
		CloseHandle(hThreadSend);
	}
	if (hThreadRecv != NULL)
	{
		CloseHandle(hThreadRecv);
	}

	if (transmitBuffer.fp != NULL)
	{
		fclose(transmitBuffer.fp);
	}

	return 0;
}
