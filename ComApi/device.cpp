#include "StdAfx.h"
#include "comapi.h"
#include "device.h"
#include <assert.h>

CDevice::CDevice(int Port) :
	KPairThread(_T(__FUNCTION__)_T("-serialport")),
	m_port(Port),
	m_hFile(NULL),
	m_waitEvtRead(new KWaitEvent(_T(__FUNCTION__)_T("-read"))),
	m_waitEvtWrite(new KWaitEvent(_T(__FUNCTION__)_T("-write")))
{
	RtlZeroMemory(&m_readOverlapped, sizeof(OVERLAPPED));
	RtlZeroMemory(&m_writeOverlapped, sizeof(OVERLAPPED));
	RtlZeroMemory(&m_dcb, sizeof(DCB));
	RtlZeroMemory(&m_commTimeouts, sizeof(COMMTIMEOUTS));

	m_readOverlapped.hEvent = m_waitEvtRead->handle();
	m_writeOverlapped.hEvent = m_waitEvtWrite->handle();

	m_waitQueue.add(m_waitEvtRead);
	m_waitQueue.add(m_waitEvtWrite);

	m_commError = 0;

	RtlZeroMemory(m_writeBuffer, sizeof(char)* 4096);
	m_writeBufferSize = 4096;
	m_nWriteSize = 0;
}

CDevice::~CDevice()
{
	close();

	delete m_waitEvtRead;
	delete m_waitEvtWrite;
}

int CDevice::open()
{
	TCHAR portName[MAX_PATH];

	_stprintf_s(portName, _T("\\\\.\\COM%d"), m_port);

	// 创建设备
	m_hFile = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_hFile = NULL;
		return CCOM_OPENFAIL;
	}

	if (!GetCommState(m_hFile, &m_dcb))
	{
		goto label_winfail_return;
	}

	m_commTimeouts.ReadIntervalTimeout = MAXDWORD;
	m_commTimeouts.ReadTotalTimeoutMultiplier = 0;
	m_commTimeouts.ReadTotalTimeoutConstant = 0;
	m_commTimeouts.WriteTotalTimeoutMultiplier = 0;
	m_commTimeouts.WriteTotalTimeoutConstant = 0;
	m_readTotalTimeouts = 0;
	m_readIntervalTimeouts = MAXDWORD;
	m_writeTotalTimeouts = 0;

	if (!SetCommTimeouts(m_hFile, &m_commTimeouts))
	{
		goto label_winfail_return;
	}

	m_dcb.EvtChar = 'q';
	m_dcb.fDtrControl = DTR_CONTROL_DISABLE;	// set DTR bit low!
	m_dcb.fRtsControl = RTS_CONTROL_DISABLE;	// set RTS bit low!
	m_dcb.BaudRate = 9600;
	m_dcb.ByteSize = 8;
	m_dcb.Parity = NOPARITY;
	m_dcb.StopBits = ONESTOPBIT;

	if (!SetCommState(m_hFile, &m_dcb))
	{
		goto label_winfail_return;
	}

	if (!SetCommMask(m_hFile, EV_RXCHAR | EV_RX80FULL))
	{
		goto label_winfail_return;
	}

	if (!PurgeComm(m_hFile, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT))
	{
		goto label_winfail_return;
	}

	// 启动线程
	KPairThread::start(PairAll);

	return CCOM_SUCCESS;

label_winfail_return:

	if (m_hFile != NULL)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}

	return CCOM_WINFAILURE;
}

int CDevice::close()
{
	// 终止线程
	KPairThread::stop(PairAll);

	if (m_hFile != NULL)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}

	return CCOM_SUCCESS;
}

int CDevice::flush(int Function)
{
	if (Function < 0 || Function > 2)
	{
		return CCOM_BADPARAM;
	}

	DWORD flags;

	switch (Function)
	{
	case CCOM_FLUSH_INPUT: flags = PURGE_RXABORT | PURGE_RXCLEAR; break;
	case CCOM_FLUSH_OUTPUT: flags = PURGE_TXABORT | PURGE_TXCLEAR; break;
	case CCOM_FLUSH_BOTH: flags = PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR; break;
	default: return CCOM_BADPARAM;
	}

	if (!PurgeComm(m_hFile, flags))
	{
		return CCOM_WINFAILURE;
	}

	return CCOM_SUCCESS;
}

int CDevice::abortRead()
{
	
	if (!m_waitEvtRead->setEvent())
	{
		return CCOM_WINFAILURE;
	}

	return this->flush(CCOM_FLUSH_INPUT);
}

int CDevice::abortWrite()
{
	if (!m_waitEvtWrite->setEvent())
	{
		return CCOM_WINFAILURE;
	}

	return this->flush(CCOM_FLUSH_OUTPUT);
}

int CDevice::breakWithTime(int Time)
{
	return this->breakWithTimeEx(Time * 0x37);
}

int CDevice::breakWithTimeEx(int Time)
{
	if (Time <= 0)
	{
		return CCOM_BADPARAM;
	}

	if (!SetCommBreak(m_hFile))
	{
		return CCOM_WINFAILURE;
	}

	Sleep(Time);

	if (!ClearCommBreak(m_hFile))
	{
		return CCOM_WINFAILURE;
	}

	return CCOM_SUCCESS;
}

int CDevice::read(void* Buffer, int Size)
{
	if (Buffer == NULL || Size <= 0)
	{
		return CCOM_BADPARAM;
	}

	DWORD bytesToRead = 0;

	if (!ReadFile(m_hFile, Buffer, Size, &bytesToRead, &m_readOverlapped))
	{
		return CCOM_WINFAILURE;
	}

	DWORD lastError = GetLastError();

	if (lastError != ERROR_IO_PENDING)
	{
		return CCOM_BADPORT;
	}

	if (!GetOverlappedResult(m_hFile, &m_readOverlapped, &bytesToRead, TRUE))
	{
		return CCOM_WINFAILURE;
	}

	m_waitEvtRead->resetEvent();

	if (bytesToRead == 0)
	{
		return CCOM_NODATA;
	}

	return (int)bytesToRead;
}

int CDevice::write(void* Buffer, int Size)
{
	if (Buffer == NULL || Size <= 0)
	{
		return CCOM_BADPARAM;
	}

	DWORD bytesToWrite = 0;

	if (!WriteFile(m_hFile, Buffer, Size, &bytesToWrite, &m_writeOverlapped))
	{
		return CCOM_WINFAILURE;
	}

	DWORD lastError = GetLastError();

	if (lastError != ERROR_IO_PENDING)
	{
		return CCOM_BADPORT;
	}

	if (!GetOverlappedResult(m_hFile, &m_writeOverlapped, &bytesToWrite, TRUE))
	{
		return CCOM_WINFAILURE;
	}

	m_waitEvtWrite->resetEvent();

	if (!PurgeComm(m_hFile, PURGE_TXABORT | PURGE_TXCLEAR))
	{
		return CCOM_WINFAILURE;
	}

	return (int)bytesToWrite;
}

bool CDevice::workFirst()
{
	DWORD dwEvtMask = 0;
	DWORD dwCommEvent = 0;
	DWORD dwError = 0;
	COMSTAT comstat;
	BOOL bResult = TRUE;

	bResult = WaitCommEvent(m_hFile, &dwEvtMask, &m_readOverlapped);

	if (!bResult)
	{
		dwError = GetLastError();
		switch (dwError)
		{
		case ERROR_IO_PENDING:
		{
			break;
		}
		case ERROR_INVALID_PARAMETER:
		{
			printf_s("Parameter is invalid!!\n");
			break;
		}
		default:
			break;
		}
	}
	else
	{
		bResult = ClearCommError(m_hFile, &dwError, &comstat);
		if (comstat.cbInQue == 0)
		{
			Sleep(10);

			return true;
		}
	} // end of 'if (!bResult)'

	WaitResult waitResult = waitObject(PairFirst, &m_waitQueue, KWaitEventQueue::WaitAny);
	switch (waitResult)
	{
	case WaitObjectFirst:
	{
		return false;
	}
	case WaitObject0:
	{
		GetCommMask(m_hFile, &dwCommEvent);

		if (dwCommEvent & EV_RXCHAR)	// 接收到字符，并置于输入缓冲区中
		{
			asyncReadChars();
		}

		if (dwCommEvent & EV_CTS)		// CTS信号状态发生变化
		{
		}

		if (dwCommEvent & EV_RXFLAG)	// 接收到事件字符，并置于输入缓冲区中 
		{
		}

		if (dwCommEvent & EV_BREAK)		// 输入中发生中断
		{
		}

		if (dwCommEvent & EV_ERR)		// 发生线路状态错误，线路状态错误包括CE_FRAME,CE_OVERRUN和CE_RXPARITY 
		{
		}

		if (dwCommEvent & EV_RING)		// 检测到振铃指示
		{
		}

		//m_waitEvtRead->resetEvent();

		break;
	}
	case WaitObject0 + 1:
	{
		asyncWriteChars();

		//m_waitEvtWrite->resetEvent();

		break;
	}
	default:
		break;
	}

	Sleep(10);

	return true;
}

bool CDevice::workSecond()
{
	return false;
}

inline bool CDevice::asyncReadChars()
{
	BOOL  bRead = TRUE;
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD bytesToRead = 0;
	COMSTAT comstat;
	unsigned char buffer;

	while (true)
	{
		// 防止线程死锁
		//if (KPairThread::waitObject(KPairThread::PairAll, NULL) == WaitFailed)
		{
			break;
		}

		m_mutex.lock();

		bResult = ClearCommError(m_hFile, &dwError, &comstat);

		m_mutex.unlock();

		if (comstat.cbInQue == 0)
		{
			break;		// break out when all bytes have been read
		}

		m_mutex.lock();

		if (bRead)
		{
			bResult = ReadFile(m_hFile, &buffer, 1, &bytesToRead, &m_readOverlapped);
			if (!bResult)
			{
				dwError = GetLastError();
				switch (dwError)
				{
				case ERROR_IO_PENDING:
				{
					// asynchronous i/o is still in progress 
					// Proceed on to GetOverlappedResults();
					bRead = FALSE;
					break;
				}
				case ERROR_INVALID_PARAMETER:
				{
					assert(false);
					break;
				}
				default:
					// Another error has occurred.  Process this error.
					break;
				}
			}
			else
			{
				// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
				bRead = TRUE;
			}
		}	// end of 'if (bRead)'

		if (!bRead)
		{
			bRead = TRUE;

			bResult = GetOverlappedResult(m_hFile, &m_readOverlapped, &bytesToRead, TRUE);
			if (!bResult)
			{
				//assert(false);
			}
		} // end of 'if (!bRead)'

		m_mutex.unlock();

		Sleep(2);  // 2ms

	} // end of 'while(true)'

	return true;
}

inline bool CDevice::asyncWriteChars()
{
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;
	DWORD dwError = 0;
	DWORD bytesToWrite = 0;
	DWORD SendLen = m_nWriteSize;

	m_waitEvtWrite->resetEvent();

	m_mutex.lock();

	if (bWrite)
	{
		m_writeOverlapped.Offset = 0;
		m_writeOverlapped.OffsetHigh = 0;

		// Clear Buffer
		PurgeComm(m_hFile, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

		// 
		bResult = WriteFile(m_hFile, m_writeBuffer, m_nWriteSize, &bytesToWrite, &m_writeOverlapped);
		if (!bResult)
		{
			dwError = GetLastError();
			switch (dwError)
			{
			case ERROR_IO_PENDING:
			{
				// continue to GetOverlappedResults()
				bytesToWrite = 0;
				bWrite = FALSE;

				break;
			}
			case ERROR_INVALID_PARAMETER:
			{
				assert(false);
				break;
			}
			default:
				break;
			}
		}
	} // end of 'if (bWrite)'
	
	if (!bWrite)
	{
		bWrite = TRUE;

		bResult = GetOverlappedResult(m_hFile, &m_writeOverlapped, &bytesToWrite, TRUE);
		if (!bResult)
		{
			//assert(false);
		}
	}  // end of 'if (!bWrite)'

	if (bytesToWrite != m_nWriteSize)
	{
		//assert(false);
	}

	m_mutex.unlock();

	return true;
}
