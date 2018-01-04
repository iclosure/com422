#pragma once

#include <Windows.h>
#include <string>
#include "comapi.h"

class CDevice : public KPairThread
{
public:
	CDevice(int Port);
	~CDevice(void);

public:
	int open();
	int close();

	int config(int Baudrate, int DataBits, int Parity, int StopBits);
	int setBaudRate(int BaudRate);
	int getBaudRate();
	int setParity(int Parity);
	int getParity();
	int setDataBits(int DataBits);
	int getDataBits();
	int setStopBits(int StopBits);
	int getStopBits();
	int setFlowCtrl(int Mode);
	int setDTR(int Mode);
	int setRTS(int Mode);
	int setLineControl(int Mode);
	int setReadTimeouts(unsigned long TotalTimeouts, unsigned long IntervalTimeouts);
	int getReadTimeouts(unsigned long* TotalTimeouts, unsigned long* IntervalTimeouts);
	int setWriteTimeouts(unsigned long TotalTimeouts);
	int getWriteTimeouts(unsigned long* TotalTimeouts);

	int getLineStatus();
	int getInQueue();
	int getOutQueue();
	int checkTxHold();
	int abortRead();
	int abortWrite();
	int flush(int Function);
	int setTermIrq(void(__crd_api*CallBack)(int Port), int Code);
	int setCntIrq(void(__crd_api*CallBack)(int Port), int Count);
	int setModemIrq(void(__crd_api*CallBack)(int Port));
	int setBreakIrq(void(__crd_api*CallBack)(int Port));
	int setTxEmptyIrq(void(__crd_api*CallBack)(int Port));
	int breakWithTime(int Time);
	int breakWithTimeEx(int Time);
	int setActOn();
	int setActOff();
	int getDataStatus();

	int read(void* Buffer, int Size);
	int write(void* Buffer, int Size);

protected:
	inline bool workFirst();
	inline bool workSecond();
	int start();
	int stop();

private:
	bool asyncReadChars();
	bool asyncWriteChars();

private:
	int					m_port;
	HANDLE				m_hFile;
	KWaitEvent*			m_waitEvtRead;
	KWaitEvent*			m_waitEvtWrite;
	KWaitEventQueue		m_waitQueue;
	OVERLAPPED			m_readOverlapped;
	OVERLAPPED			m_writeOverlapped;
	COMMTIMEOUTS		m_commTimeouts;
	DCB					m_dcb;

	char				m_writeBuffer[4096];
	DWORD				m_writeBufferSize;
	int					m_nWriteSize;

	KMutex				m_mutex;

	unsigned long		m_readTotalTimeouts;
	unsigned long		m_readIntervalTimeouts;
	unsigned long		m_writeTotalTimeouts;
	DWORD				m_commError;
};
