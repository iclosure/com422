
#include "stdafx.h"
#include "comapi.h"
#include "device.h"

static CDevice** _g_arrayDevice = NULL;		// 设备对象数组[0.1..256]

#define CCOM_PORT_INVALID_RETURN(_port_) \
	if ((_port_ < 1 || _port_ > 256) || \
	(_g_arrayDevice[_port_] == NULL)) { return CCOM_BADPORT; }

DWORD DeviceInitialize()
{
	if (_g_arrayDevice == NULL)
	{
		_g_arrayDevice = new CDevice*[257];

		RtlZeroMemory(_g_arrayDevice, sizeof(CDevice*)* 257);
	}

	return TRUE;
}

DWORD DeviceCleanup()
{
	if (_g_arrayDevice != NULL)
	{
		delete[] _g_arrayDevice;

		_g_arrayDevice = NULL;
	}

	return TRUE;
}

int __crd_api com_open(int Port)
{
	if (Port < 1 || Port > 256)
	{
		return CCOM_BADPORT;
	}

	if (_g_arrayDevice[Port] == NULL)
	{
		_g_arrayDevice[Port] = new CDevice(Port);
	}

	return _g_arrayDevice[Port]->open();
}

int __crd_api com_close(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	int result = _g_arrayDevice[Port]->close();

	if (result == CCOM_SUCCESS)
	{
		delete _g_arrayDevice[Port];
		_g_arrayDevice[Port] = NULL;
	}

	return result;
}

int __crd_api com_flush(int Port, int Function)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->flush(Function);
}

/*Port status inquiry*/

int __crd_api com_lstatus(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->getLineStatus();
}

int __crd_api com_iqueue(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->getInQueue();
}

int __crd_api com_oqueue(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->getOutQueue();
}

int __crd_api com_data_status(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->getDataStatus();
}

int __crd_api com_tx_hold(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->checkTxHold();
}

/*Functions to config the options of a serial port*/

int __crd_api com_config(int Port, int BaudRate, int DataBits, int Parity, int StopBits)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->config(BaudRate, DataBits, Parity, StopBits);
}

int __crd_api com_set_baudrate(int Port, int BaudRate)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setBaudRate(BaudRate);
}

int __crd_api com_get_baudrate(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->getBaudRate();
}

int __crd_api com_set_databits(int Port, int DataBits)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setDataBits(DataBits);
}

int __crd_api com_get_databits(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->getDataBits();
}

int __crd_api com_set_parity(int Port, int Parity)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setParity(Parity);
}

int __crd_api com_get_parity(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->getParity();
}

int __crd_api com_set_stopbits(int Port, int StopBits)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setStopBits(StopBits);
}

int __crd_api com_get_stopbits(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->getStopBits();
}

int __crd_api com_flowctrl(int Port, int Mode)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setFlowCtrl(Mode);
}

int __crd_api com_DTR(int Port, int Mode)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setDTR(Mode);
}

int __crd_api com_RTS(int Port, int Mode)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setRTS(Mode);
}

int __crd_api com_lctrl(int Port, int Mode)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setLineControl(Mode);
}

/*Input Data (This category includes functions to read data from a serial port)*/

int __crd_api com_getch(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	char character;

	int result = _g_arrayDevice[Port]->read(&character, 1);
	
	if (result == CCOM_SUCCESS)
	{
		result = character;
	}

	return result;
}

int __crd_api com_read(int Port, char* Buffer, int Size)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->read(Buffer, Size);
}

int __crd_api com_setReadTimeouts(int Port, unsigned long TotalTimeouts, unsigned long IntervalTimeouts)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setReadTimeouts(TotalTimeouts, IntervalTimeouts);
}

int __crd_api com_getReadTimeouts(int Port, unsigned long* TotalTimeouts, unsigned long* IntervalTimeouts)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->getReadTimeouts(TotalTimeouts, IntervalTimeouts);
}

int __crd_api com_abort_read(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->abortRead();
}

/*Output Data (This category includes functions to write data to a serial port)*/

int __crd_api com_putch(int Port, int Character)
{
	CCOM_PORT_INVALID_RETURN(Port);

	char character = (char)Character;

	return _g_arrayDevice[Port]->write(&character, 1);
}

int __crd_api com_write(int Port, char* Buffer, int Size)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->write(Buffer, Size);
}

int __crd_api com_setWriteTimeouts(int Port, unsigned long TotalTimeouts)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setWriteTimeouts(TotalTimeouts);
}

int __crd_api com_getWriteTimeouts(int Port, unsigned long* TotalTimeouts)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->getWriteTimeouts(TotalTimeouts);
}

int __crd_api com_abort_write(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->abortWrite();
}

/*Event Service: Port Status Inquiry (This category includes functions
 *to set the communication event service routine for a serial port)*/

int __crd_api com_term_irq(int Port, void(__crd_api*CallBack)(int Port), int Code)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setTermIrq(CallBack, Code);
}

int __crd_api com_cnt_irq(int Port, void(__crd_api*CallBack)(int Port), int Count)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setCntIrq(CallBack, Count);
}

int __crd_api com_modem_irq(int Port, void(__crd_api*CallBack)(int Port))
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setModemIrq(CallBack);
}

int __crd_api com_break_irq(int Port, void(__crd_api*CallBack)(int Port))
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setBreakIrq(CallBack);
}

int __crd_api com_tx_empty_irq(int Port, void(__crd_api*CallBack)(int Port))
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setTxEmptyIrq(CallBack);
}

/*Miscellaneous (This category includes functions to do special function
*for a serial port.)*/

int __crd_api com_break(int Port, int Time)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->breakWithTime(Time);
}

int __crd_api com_break_ex(int Port, int Time)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->breakWithTimeEx(Time);
}

int __crd_api com_act_xon(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setActOn();
}

int __crd_api com_act_xoff(int Port)
{
	CCOM_PORT_INVALID_RETURN(Port);

	return _g_arrayDevice[Port]->setActOff();
}
