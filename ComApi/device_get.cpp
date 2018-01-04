#include "StdAfx.h"
#include "comapi.h"
#include "device.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////////

int CDevice::getBaudRate()
{
	return (int)m_dcb.BaudRate;
}

int CDevice::getParity()
{
	return (int)m_dcb.Parity;
}

int CDevice::getDataBits()
{
	return (int)m_dcb.ByteSize;
}

int CDevice::getStopBits()
{
	return (int)m_dcb.StopBits;
}

int CDevice::getReadTimeouts(unsigned long* TotalTimeouts, unsigned long* IntervalTimeouts)
{
	*TotalTimeouts = m_readTotalTimeouts;
	*IntervalTimeouts = m_readIntervalTimeouts;
	
	return CCOM_SUCCESS;
}

int CDevice::getWriteTimeouts(unsigned long* TotalTimeouts)
{
	*TotalTimeouts = m_writeTotalTimeouts;

	return CCOM_SUCCESS;
}

int CDevice::getLineStatus()
{
	DWORD modemStatus = 0;

	if (!GetCommModemStatus(m_hFile, &modemStatus))
	{
		return CCOM_WINFAILURE;
	}



	return CCOM_SUCCESS;
}

int CDevice::getInQueue()
{
	DWORD commError;
	COMSTAT comStat;

	comStat.cbInQue = 0;

	if (!ClearCommError(m_hFile, &commError, &comStat))
	{
		return CCOM_WINFAILURE;
	}

	return comStat.cbInQue;
}

int CDevice::getOutQueue()
{
	DWORD commError;
	COMSTAT comStat;

	comStat.cbOutQue = 0;

	if (!ClearCommError(m_hFile, &commError, &comStat))
	{
		return CCOM_WINFAILURE;
	}

	return comStat.cbOutQue;
}

int CDevice::checkTxHold()
{
	DWORD commError = 0;
	COMSTAT comStat = { 0 };

	if (!ClearCommError(m_hFile, &commError, &comStat))
	{
		return CCOM_WINFAILURE;
	}

	int result = 0;

	if (comStat.fCtsHold)
	{
		result |= 1;
	}

	if (comStat.fXoffHold)
	{
		result |= 2;
	}

	return result;
}

int CDevice::getDataStatus()
{
	DWORD commError = 0;

	if (!ClearCommError(m_hFile, &commError, NULL))
	{
		return CCOM_WINFAILURE;
	}

	m_commError = commError;

	int result = 0;

	if (commError & CE_RXPARITY)
	{
		result |= 1;
	}

	if (commError & CE_FRAME)
	{
		result |= 2;
	}

	if (commError & CE_OVERRUN)
	{
		result |= 4;
	}

	if (commError & CE_RXOVER)
	{
		result |= 8;
	}

	return result;
}