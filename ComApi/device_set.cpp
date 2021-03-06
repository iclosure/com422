#include "StdAfx.h"
#include "comapi.h"
#include "device.h"

int CDevice::config(int BaudRate, int DataBits, int Parity, int StopBits)
{
	if (BaudRate < 0 ||
		(DataBits < 0 || DataBits > MAXBYTE) ||
		(Parity   < 0 || Parity   > MAXBYTE) ||
		(StopBits < 0 || StopBits > MAXBYTE))
	{
		return CCOM_BADPARAM;
	}

	DCB dcb;

	RtlCopyMemory(&dcb, &m_dcb, sizeof(DCB));

	m_dcb.BaudRate = (DWORD)BaudRate;
	m_dcb.ByteSize = (BYTE)DataBits;
	m_dcb.Parity = (BYTE)Parity;
	m_dcb.StopBits = (BYTE)StopBits;

	if (!SetCommState(m_hFile, &m_dcb))
	{
		RtlCopyMemory(&m_dcb, &dcb, sizeof(DCB));
		SetCommState(m_hFile, &m_dcb);

		return CCOM_WINFAILURE;
	}

	return CCOM_SUCCESS;
}

int CDevice::setBaudRate(int BaudRate)
{
	if (BaudRate < 0)
	{
		return CCOM_BADPARAM;
	}

	DCB dcb;

	RtlCopyMemory(&dcb, &m_dcb, sizeof(DCB));

	m_dcb.BaudRate = (DWORD)BaudRate;

	if (!SetCommState(m_hFile, &m_dcb))
	{ 
		RtlCopyMemory(&m_dcb, &dcb, sizeof(DCB));
		SetCommState(m_hFile, &m_dcb);

		return CCOM_WINFAILURE;
	}

	return CCOM_SUCCESS;
}

int CDevice::setParity(int Parity)
{
	if (Parity < 0 || Parity > MAXBYTE)
	{
		return CCOM_BADPARAM;
	}

	DCB dcb;

	RtlCopyMemory(&dcb, &m_dcb, sizeof(DCB));

	m_dcb.Parity = (BYTE)Parity;

	if (!SetCommState(m_hFile, &m_dcb))
	{
		RtlCopyMemory(&m_dcb, &dcb, sizeof(DCB));
		SetCommState(m_hFile, &m_dcb);

		return CCOM_WINFAILURE;
	}

	return CCOM_SUCCESS;
}

int CDevice::setDataBits(int DataBits)
{
	if (DataBits < 4 || DataBits > MAXBYTE)
	{
		return CCOM_BADPARAM;
	}

	DCB dcb;

	RtlCopyMemory(&dcb, &m_dcb, sizeof(DCB));

	m_dcb.ByteSize = (BYTE)DataBits;

	if (!SetCommState(m_hFile, &m_dcb))
	{
		RtlCopyMemory(&m_dcb, &dcb, sizeof(DCB));
		SetCommState(m_hFile, &m_dcb);

		return CCOM_WINFAILURE;
	}

	return CCOM_SUCCESS;
}

int CDevice::setStopBits(int StopBits)
{
	if (StopBits < 0 || StopBits > MAXBYTE)
	{
		return CCOM_BADPARAM;
	}

	DCB dcb;

	RtlCopyMemory(&dcb, &m_dcb, sizeof(DCB));

	m_dcb.StopBits = (BYTE)StopBits;

	if (!SetCommState(m_hFile, &m_dcb))
	{
		RtlCopyMemory(&m_dcb, &dcb, sizeof(DCB));
		SetCommState(m_hFile, &m_dcb);

		return CCOM_WINFAILURE;
	}

	return CCOM_SUCCESS;
}

int CDevice::setFlowCtrl(int Mode)
{
	if (Mode < 0 || Mode > 0x3F)
	{
		return CCOM_BADPARAM;
	}

	DCB dcb;

	RtlCopyMemory(&dcb, &m_dcb, sizeof(DCB));

	m_dcb.fOutxCtsFlow = (Mode >> 0) & 0x01;	// bit0 of Mode : fOutxCtsFlow
	m_dcb.fRtsControl = (Mode >> 1) & 0x01;		// bit1 of Mode : fRtsControl
	m_dcb.fOutX = (Mode >> 2) & 0x01;			// bit2 of Mode : fOutX
	m_dcb.fInX = (Mode >> 3) & 0x01;			// bit3 of Mode : fInX
	m_dcb.fOutxDsrFlow = (Mode >> 4) & 0x01;	// bit4 of Mode : fOutxDsrFlow
	m_dcb.fDtrControl = (Mode >> 5) & 0x01;		// bit5 of Mode : fDtrControl
	
	if (!SetCommState(m_hFile, &m_dcb))
	{
		RtlCopyMemory(&m_dcb, &dcb, sizeof(DCB));
		SetCommState(m_hFile, &m_dcb);

		return CCOM_WINFAILURE;
	}

	return CCOM_SUCCESS;
}

int CDevice::setDTR(int Mode)
{
	if (Mode < 0 || Mode > 1)
	{
		return CCOM_BADPARAM;
	}

	if (!EscapeCommFunction(m_hFile, (CLRDTR - Mode)))
	{
		return CCOM_WINFAILURE;
	}

	return CCOM_SUCCESS;
}

int CDevice::setRTS(int Mode)
{
	if (Mode < 0 || Mode > 1)
	{
		return CCOM_BADPARAM;
	}

	if (!EscapeCommFunction(m_hFile, (CLRRTS - Mode)))
	{
		return CCOM_WINFAILURE;
	}

	return CCOM_SUCCESS;
}

int CDevice::setLineControl(int Mode)
{
	if (Mode < 0 || Mode > 3)
	{
		return CCOM_BADPARAM;
	}

	if (!EscapeCommFunction(m_hFile, (CLRDTR - ((Mode >> 0) & 0x01))))
	{
		return CCOM_WINFAILURE;
	}

	if (!EscapeCommFunction(m_hFile, (CLRRTS - ((Mode >> 1) & 0x01))))
	{
		return CCOM_WINFAILURE;
	}

	return CCOM_SUCCESS;
}

int CDevice::setReadTimeouts(unsigned long TotalTimeouts, unsigned long IntervalTimeouts)
{
	COMMTIMEOUTS commTimeouts;

	if (!GetCommTimeouts(m_hFile, &commTimeouts))
	{
		return CCOM_WINFAILURE;
	}

	commTimeouts.ReadTotalTimeoutMultiplier = 0;

	if (TotalTimeouts == MAXDWORD)
	{
		commTimeouts.ReadIntervalTimeout = MAXDWORD;
		commTimeouts.ReadTotalTimeoutConstant = 0;
	}
	else if (TotalTimeouts == 0)
	{
		if (IntervalTimeouts == 0)
		{
			commTimeouts.ReadIntervalTimeout = MAXDWORD;
			commTimeouts.ReadTotalTimeoutConstant = MAXDWORD;
		}
		else
		{
			commTimeouts.ReadIntervalTimeout = IntervalTimeouts;
			commTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
			commTimeouts.ReadTotalTimeoutConstant = 0;
		}
	}
	else
	{
		commTimeouts.ReadIntervalTimeout = IntervalTimeouts;
		commTimeouts.ReadTotalTimeoutConstant = TotalTimeouts;
	}

	if (!SetCommTimeouts(m_hFile, &commTimeouts))
	{
		return CCOM_WINFAILURE;
	}

	m_readTotalTimeouts = TotalTimeouts;
	m_readIntervalTimeouts = IntervalTimeouts;

	return CCOM_SUCCESS;
}

int CDevice::setWriteTimeouts(unsigned long TotalTimeouts)
{
	if (TotalTimeouts == MAXDWORD)
	{
		return CCOM_BADPARAM;
	}

	COMMTIMEOUTS commTimeouts;

	if (!GetCommTimeouts(m_hFile, &commTimeouts))
	{
		return CCOM_WINFAILURE;
	}

	commTimeouts.WriteTotalTimeoutConstant = TotalTimeouts;

	if (!SetCommTimeouts(m_hFile, &commTimeouts))
	{
		return CCOM_WINFAILURE;
	}

	m_writeTotalTimeouts = TotalTimeouts;

	return CCOM_SUCCESS;
}

int CDevice::setTermIrq(void(__crd_api*CallBack)(int Port), int Code)
{
	return CCOM_SUCCESS;
}

int CDevice::setCntIrq(void(__crd_api*CallBack)(int Port), int Count)
{
	return CCOM_SUCCESS;
}

int CDevice::setModemIrq(void(__crd_api*CallBack)(int Port))
{
	return CCOM_SUCCESS;
}

int CDevice::setBreakIrq(void(__crd_api*CallBack)(int Port))
{
	return CCOM_SUCCESS;
}

int CDevice::setTxEmptyIrq(void(__crd_api*CallBack)(int Port))
{
	return CCOM_SUCCESS;
}

int CDevice::setActOn()
{
	if (!EscapeCommFunction(m_hFile, SETXON))
	{
		return CCOM_WINFAILURE;
	}

	return CCOM_SUCCESS;
}

int CDevice::setActOff()
{
	if (!EscapeCommFunction(m_hFile, SETXOFF))
	{
		return CCOM_WINFAILURE;
	}

	return CCOM_SUCCESS;
}
