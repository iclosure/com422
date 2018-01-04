
#include "precomp.h"
#include "ioctlpdo.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PdoPortIoQueueCreate)
#pragma alloc_text(PAGE, EvtIoWritePdoPort)
#pragma alloc_text(PAGE, EvtIoReadPdoPort)
#pragma alloc_text(PAGE, EvtIoDeviceControlPdoPort)
#endif

NTSTATUS PdoPortIoQueueCreate(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	WDF_OBJECT_ATTRIBUTES attributes;
	WDF_IO_QUEUE_CONFIG queueConfig;
	WDFQUEUE queueDefault;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);

	// 创建默认队列
	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);
	queueConfig.EvtIoWrite = EvtIoWritePdoPort;
	queueConfig.EvtIoRead = EvtIoReadPdoPort;
	queueConfig.EvtIoDeviceControl = EvtIoDeviceControlPdoPort;
	status = WdfIoQueueCreate(Device, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, &queueDefault);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfIoQueueCreate() QueueDefault failed as %08X\n", status));
		return status;
	}

	// 创建IOCTL自旋锁
	status = WdfSpinLockCreate(WDF_NO_OBJECT_ATTRIBUTES, &portContext->SpinLockIoctl);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfSpinLockCreate() IoctlSpinLock failed as %08X\n", status));
		return status;
	}

	HR_PRINT_METHOD_END();

	return status;
}

VOID EvtIoWritePdoPort(
	IN WDFQUEUE Queue,
	IN WDFREQUEST Request,
	IN size_t Length)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	PUCHAR inBuffer;
	ULONG writeCount;
	ULONG writeOffset;
	ULONG txRoom;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	status = WdfRequestRetrieveInputBuffer(Request, Length, (PVOID*)&inBuffer, &writeCount);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfRequestRetrieveInputBuffer() Failed: %08X\n", status));
		WdfRequestComplete(Request, status);
		return;
	}

	portContext = WdfPdoGetPortContext(WdfIoQueueGetDevice(Queue));

	writeOffset = portContext->PortIoInfo.WriteOffset;

#ifdef DEBUG_WITH_REAL_DEVICE

	txRoom = COM422_PPORT_READ_TXROOM(portContext);
	txRoom = min(txRoom, writeCount - writeOffset);

	if (txRoom > 0)
	{
		COM422_PPORT_WRITE_UCHARS(portContext, &inBuffer[writeOffset], txRoom);
	}

#else

	while (writeCount > 0)
	{
		if (writeOffset >= writeCount)
		{
			break;
		}

		if (!CircFifoIsFull(portContext->CircFifo))
		{
			CircFifoWrite(portContext->CircFifo, &inBuffer[writeOffset]);
			HrKdPrint(("WriteOffset: %ld, Data: %02X ('%c')\n", writeOffset, inBuffer[writeOffset], inBuffer[writeOffset]));
			writeOffset++;
		}
		else
		{
			break;
		}
	}

#endif  // DEBUG_WITH_REAL_DEVICE

	WdfRequestCompleteWithInformation(Request, status, (writeOffset - portContext->PortIoInfo.WriteOffset));

	HR_PRINT_METHOD_END();
}

VOID EvtIoReadPdoPort(
	IN WDFQUEUE Queue,
	IN WDFREQUEST Request,
	IN size_t Length)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	PUCHAR outBuffer;
	size_t readCount;
	ULONG readOffset;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(WdfIoQueueGetDevice(Queue));

	status = WdfRequestRetrieveOutputBuffer(Request, Length, (PVOID*)&outBuffer, &readCount);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfRequestRetrieveOutputBuffer() Failed : %08X\n", status));
		WdfRequestComplete(Request, STATUS_UNSUCCESSFUL);
		return;
	}

	readOffset = portContext->PortIoInfo.ReadOffset;

	while (readCount > 0)
	{
		if (readOffset >= readCount)
		{
			break;
		}

		if (CircFifoRead(portContext->CircFifo, &outBuffer[readOffset]))
		{
			HrKdPrint(("ReadOffset: %ld, Data: %02X ('%c')\n", readOffset, outBuffer[readOffset], outBuffer[readOffset]));
			readOffset++;
		}
		else
		{
			break;
		}
	}

	WdfRequestCompleteWithInformation(Request, status, (readOffset - portContext->PortIoInfo.ReadOffset));

	HR_PRINT_METHOD_END();
}

VOID EvtIoQueueContextCleanupPdoPort(
	IN WDFOBJECT Object)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION pdoPortContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();
}

VOID EvtIoDeviceControlPdoPort(
	IN WDFQUEUE Queue,
	IN WDFREQUEST Request,
	IN size_t OutputBufferLength,
	IN size_t InputBufferLength,
	IN ULONG IoControlCode)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	COM422_INTERRUPT_SYNC_CONTEXT syncContext;
	WDFDEVICE device;
	PVOID pInBuffer = NULL, pOutBuffer = NULL;
	size_t length;
	ULONG bytesResolved = 0;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	device = WdfIoQueueGetDevice(Queue);
	portContext = WdfPdoGetPortContext(device);
	busContext = WdfBusGetContext(WdfPdoGetParent(device));
	syncContext.Context = portContext;

	// 如果输入缓冲区存在，则取得输入缓冲区，并判断其有效性
	if (InputBufferLength > 0)
	{
		status = WdfRequestRetrieveInputBuffer(Request, InputBufferLength, &pInBuffer, &length);
		if (!NT_SUCCESS(status))
		{
			HrKdPrint(("WdfRequestRetrieveInputBuffer() failed as %08X\n", status));
			WdfRequestComplete(Request, status);
			return;
		}
	}

	// 如果输出缓冲区存在，则取得输出缓冲区，并判断其有效性
	if (OutputBufferLength > 0)
	{
		status = WdfRequestRetrieveOutputBuffer(Request, OutputBufferLength, &pOutBuffer, &length);
		if (!NT_SUCCESS(status))
		{
			HrKdPrint(("WdfRequestRetrieveOutputBuffer() failed as %08X\n", status));
			WdfRequestComplete(Request, status);
			return;
		}
	}

	// IO请求处理
	switch (IoControlCode)
	{
	case IOCTL_SERIAL_SET_BAUD_RATE:
	{
		HrKdPrint(("IOCTL_SERIAL_SET_BAUD_RATE\n"));

		syncContext.Data = (PVOID)pInBuffer;
		if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialSetBaudRate, &syncContext))
		{
			HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() failed\n"));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		bytesResolved = sizeof(SERIAL_BAUD_RATE);

		break;
	}
	case IOCTL_SERIAL_GET_BAUD_RATE:
	{
		HrKdPrint(("IOCTL_SERIAL_GET_BAUD_RATE\n"));

		syncContext.Data = (PVOID)pOutBuffer;
		if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialGetBaudRate, &syncContext))
		{
			HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() failed\n"));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		bytesResolved = sizeof(SERIAL_BAUD_RATE);

		break;
	}
	case IOCTL_SERIAL_SET_LINE_CONTROL:
	{
		HrKdPrint(("IOCTL_SERIAL_SET_LINE_CONTROL\n"));

		syncContext.Data = (PVOID)pInBuffer;
		if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialSetLineControl, &syncContext))
		{
			HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() failed\n"));
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		
		bytesResolved = sizeof(SERIAL_LINE_CONTROL);

		break;
	}
	case IOCTL_SERIAL_GET_LINE_CONTROL:
	{
		HrKdPrint(("IOCTL_SERIAL_GET_LINE_CONTROL\n"));

		syncContext.Data = (PVOID)pOutBuffer;
		if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialGetLineControl, &syncContext))
		{
			HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() failed\n"));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		bytesResolved = sizeof(SERIAL_LINE_CONTROL);

		break;
	}
	case IOCTL_SERIAL_PURGE:
	{
		HrKdPrint(("IOCTL_SERIAL_PURGE\n"));

		syncContext.Data = (PVOID)pInBuffer;
		if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialPurge, &syncContext))
		{
			HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() failed\n"));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		bytesResolved = sizeof(ULONG);

		break;
	}
	case IOCTL_SERIAL_GET_COMMSTATUS:
	{
		HrKdPrint(("IOCTL_SERIAL_GET_COMMSTATUS\n"));

		syncContext.Data = (PVOID)pOutBuffer;
		if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialGetCommStatus, &syncContext))
		{
			HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() failed\n"));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		bytesResolved = sizeof(SERIAL_STATUS);

		break;
	}
	case IOCTL_SERIAL_GET_MODEMSTATUS:
	{
		HrKdPrint(("IOCTL_SERIAL_GET_MODEMSTATUS\n"));

		syncContext.Data = (PVOID)pOutBuffer;
		if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialGetModemStatus, &syncContext))
		{
			HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() failed\n"));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		bytesResolved = sizeof(ULONG);

		break;
	}
	case IOCTL_SERIAL_SET_DTR:
	{
		ULONG serialDTR = 1;

		HrKdPrint(("IOCTL_SERIAL_SET_DTR\n"));

		syncContext.Data = (PVOID)&serialDTR;
		if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialSetDTR, &syncContext))
		{
			HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() failed\n"));
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		
		break;
	}
	case IOCTL_SERIAL_CLR_DTR:
	{
		ULONG serialDTR = 0;

		HrKdPrint(("IOCTL_SERIAL_CLR_DTR\n"));

		syncContext.Data = (PVOID)&serialDTR;
		if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialSetDTR, &syncContext))
		{
			HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() failed\n"));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		break;
	}
	case IOCTL_SERIAL_SET_RTS:
	{
		ULONG serialRTS = 1;

		HrKdPrint(("IOCTL_SERIAL_SET_RTS\n"));

		syncContext.Data = (PVOID)&serialRTS;
		if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialSetRTS, &syncContext))
		{
			HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() failed\n"));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		break;
	}
	case IOCTL_SERIAL_CLR_RTS:
	{
		ULONG serialRTS = 0;

		HrKdPrint(("IOCTL_SERIAL_CLR_RTS\n"));

		syncContext.Data = (PVOID)&serialRTS;
		if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialSetRTS, &syncContext))
		{
			HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() failed\n"));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		break;
	}
	case IOCTL_SERIAL_GET_DTRRTS:	// 
	{
		ULONG modemStatus;

		HrKdPrint(("IOCTL_SERIAL_GET_DTRRTS\n"));

		syncContext.Data = (PVOID)&modemStatus;
		if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialGetModemStatus, &syncContext))
		{
			HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() failed\n"));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		*(PULONG)pOutBuffer = GET_REG_BITS_VAL(modemStatus,
			COM422_CTRL_DTR_OFFSET, COM422_CTRL_DTR_MASK | COM422_CTRL_RTS_MASK);
		
		bytesResolved = sizeof(ULONG);

		break;
	}
	default:
		HrKdPrint(("(Error) STATUS_NOT_SUPPORTED\n"));
		status = STATUS_NOT_SUPPORTED;
		break;
	}

	WdfRequestCompleteWithInformation(Request, status, bytesResolved);

	HR_PRINT_METHOD_END();
}

BOOLEAN EvtSerialSetBaudRate(
	IN WDFINTERRUPT Interrupt,
	IN WDFCONTEXT Context)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	PSERIAL_BAUD_RATE serialBaudRate;
	ULONG baudRate;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PPDOPORT_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Context;
	serialBaudRate = (PSERIAL_BAUD_RATE)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Data;

	switch (serialBaudRate->BaudRate)
	{
	case 50: baudRate = COM422_CTRL_BAUDRATE_50; break;
	case 75: baudRate = COM422_CTRL_BAUDRATE_75; break;
	case 300: baudRate = COM422_CTRL_BAUDRATE_300; break;
	case 600: baudRate = COM422_CTRL_BAUDRATE_600; break;
	case 1200: baudRate = COM422_CTRL_BAUDRATE_1200; break;
	case 1800: baudRate = COM422_CTRL_BAUDRATE_1800; break;
	case 2400: baudRate = COM422_CTRL_BAUDRATE_2400; break;
	case 4800: baudRate = COM422_CTRL_BAUDRATE_4800; break;
	case 7200: baudRate = COM422_CTRL_BAUDRATE_7200; break;
	case 9600: baudRate = COM422_CTRL_BAUDRATE_9600; break;
	case 14400: baudRate = COM422_CTRL_BAUDRATE_14400; break;
	case 19200: baudRate = COM422_CTRL_BAUDRATE_19200; break;
	case 38400: baudRate = COM422_CTRL_BAUDRATE_38400; break;
	case 57600: baudRate = COM422_CTRL_BAUDRATE_57600; break;
	case 115200: baudRate = COM422_CTRL_BAUDRATE_115200; break;
	case 230400: baudRate = COM422_CTRL_BAUDRATE_230400; break;
	case 460800: baudRate = COM422_CTRL_BAUDRATE_460800; break;
	case 921600: baudRate = COM422_CTRL_BAUDRATE_921600; break;
	case 3125000: baudRate = COM422_CTRL_BAUDRATE_3125000; break;
	default:
		status = STATUS_INVALID_PARAMETER;
	}

	HrKdPrint(("BaudRate : %ld\n", baudRate));

	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("BaudRate -> Error: %08X\n", status));
		return FALSE;
	}

	COM422_PPORT_PRINT_CTRL(portContext);

	COM422_PPORT_WRITE_CTRL_BITS(portContext,
		COM422_CTRL_BUADRATE_OFFSET, COM422_CTRL_BUADRATE_MASK, baudRate);

	COM422_PPORT_PRINT_CTRL(portContext);

	portContext->PortIoInfo.BaudRate = *serialBaudRate;

	HR_PRINT_METHOD_END();

	return TRUE;
}

BOOLEAN EvtSerialGetBaudRate(
	IN WDFINTERRUPT Interrupt,
	IN WDFCONTEXT Context)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	PSERIAL_BAUD_RATE serialBaudRate;
	ULONG baudRate;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PPDOPORT_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Context;
	serialBaudRate = (PSERIAL_BAUD_RATE)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Data;

	COM422_PPORT_PRINT_CTRL(portContext);

	baudRate = COM422_PPORT_READ_CTRL_BITS(portContext,
		COM422_CTRL_BUADRATE_OFFSET, COM422_CTRL_BUADRATE_MASK);

	switch (baudRate)
	{
	case COM422_CTRL_BAUDRATE_50: baudRate = 50; break;
	case COM422_CTRL_BAUDRATE_75: baudRate = 75; break;
	case COM422_CTRL_BAUDRATE_300: baudRate = 300; break;
	case COM422_CTRL_BAUDRATE_600: baudRate = 600; break;
	case COM422_CTRL_BAUDRATE_1200: baudRate = 1200; break;
	case COM422_CTRL_BAUDRATE_1800: baudRate = 1800; break;
	case COM422_CTRL_BAUDRATE_2400: baudRate = 2400; break;
	case COM422_CTRL_BAUDRATE_4800: baudRate = 4800; break;
	case COM422_CTRL_BAUDRATE_7200: baudRate = 7200; break;
	case COM422_CTRL_BAUDRATE_9600: baudRate = 9600; break;
	case COM422_CTRL_BAUDRATE_14400: baudRate = 14400; break;
	case COM422_CTRL_BAUDRATE_19200: baudRate = 19200; break;
	case COM422_CTRL_BAUDRATE_38400: baudRate = 38400; break;
	case COM422_CTRL_BAUDRATE_57600: baudRate = 57600; break;
	case COM422_CTRL_BAUDRATE_115200: baudRate = 115200; break;
	case COM422_CTRL_BAUDRATE_230400: baudRate = 230400; break;
	case COM422_CTRL_BAUDRATE_460800: baudRate = 460800; break;
	case COM422_CTRL_BAUDRATE_921600: baudRate = 921600; break;
	case COM422_CTRL_BAUDRATE_3125000: baudRate = 3125000; break;
	default:
		status = STATUS_INVALID_PARAMETER;
	}

	HrKdPrint(("BaudRate : %ld\n", baudRate));

	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("BaudRate -> Error: %08X\n", status));
		return FALSE;
	}

	portContext->PortIoInfo.BaudRate.BaudRate = baudRate;

	serialBaudRate->BaudRate = baudRate;

	HR_PRINT_METHOD_END();

	return TRUE;
}

BOOLEAN EvtSerialSetLineControl(
	IN WDFINTERRUPT Interrupt,
	IN WDFCONTEXT Context)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	PSERIAL_LINE_CONTROL serialLineControl;
	SERIAL_LINE_CONTROL newLineControl;
	UCHAR value;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PPDOPORT_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Context;
	serialLineControl = (PSERIAL_LINE_CONTROL)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Data;

	// 
	// Config DataBits
	// 
	switch (serialLineControl->WordLength)
	{
	case 5: newLineControl.WordLength = COM422_CTRL_DATABITS_5; break;
	case 6: newLineControl.WordLength = COM422_CTRL_DATABITS_6; break;
	case 7: newLineControl.WordLength = COM422_CTRL_DATABITS_7; break;
	case 8: newLineControl.WordLength = COM422_CTRL_DATABITS_8; break;
	default:
		status = STATUS_INVALID_PARAMETER;
	}

	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("DataBits -> Error: %08X\n", status));
		return FALSE;
	}

	// 
	// Config Parity
	// 
	switch (serialLineControl->Parity)
	{
	case NO_PARITY: newLineControl.Parity = COM422_CTRL_PARITY_NONE; break;
	case ODD_PARITY: newLineControl.Parity = COM422_CTRL_PARITY_ODD; break;
	case EVEN_PARITY: newLineControl.Parity = COM422_CTRL_PARITY_EVEN; break;
	case MARK_PARITY: newLineControl.Parity = COM422_CTRL_PARITY_MARK; break;
	case SPACE_PARITY: newLineControl.Parity = COM422_CTRL_PARITY_SPACE; break;
	default:
		status = STATUS_INVALID_PARAMETER;
	}

	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("Parity -> Error: %08X\n", status));
		return FALSE;
	}

	// 
	// Config StopBits
	// 
	switch (serialLineControl->StopBits)
	{
	case STOP_BIT_1: newLineControl.StopBits = COM422_CTRL_STOPBITS_1; break;
	case STOP_BITS_1_5: newLineControl.StopBits = COM422_CTRL_STOPBITS_1_5; break;
	case STOP_BITS_2: newLineControl.StopBits = COM422_CTRL_STOPBITS_2; break;
	default:
		status = STATUS_INVALID_PARAMETER;
	}

	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("StopBits -> Error: %08X\n", status));
		return FALSE;
	}

	COM422_PPORT_PRINT_CTRL(portContext);

	COM422_PPORT_WRITE_CTRL_BITS(portContext,
		COM422_CTRL_DATABITS_OFFSET, COM422_CTRL_DATABITS_MASK, (ULONG)newLineControl.WordLength);
	COM422_PPORT_WRITE_CTRL_BITS(portContext,
		COM422_CTRL_PARITY_OFFSET, COM422_CTRL_PARITY_MASK, (ULONG)newLineControl.Parity);
	COM422_PPORT_WRITE_CTRL_BITS(portContext,
		COM422_CTRL_STOPBITS_OFFSET, COM422_CTRL_STOPBITS_MASK, (ULONG)newLineControl.StopBits);

	COM422_PPORT_PRINT_CTRL(portContext);

	portContext->PortIoInfo.LineCtrl = *serialLineControl;

	HR_PRINT_METHOD_END();

	return TRUE;
}

BOOLEAN EvtSerialGetLineControl(
	IN WDFINTERRUPT Interrupt,
	IN WDFCONTEXT Context)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	PSERIAL_LINE_CONTROL serialLineControl;
	SERIAL_LINE_CONTROL lineControl;
	UCHAR value;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PPDOPORT_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Context;
	serialLineControl = (PSERIAL_LINE_CONTROL)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Data;

	COM422_PPORT_PRINT_CTRL(portContext);

	// 
	// Get DataBits
	// 
	value = COM422_PPORT_READ_CTRL_BITS(portContext,
		COM422_CTRL_DATABITS_OFFSET, COM422_CTRL_DATABITS_MASK);

	switch (value)
	{
	case COM422_CTRL_DATABITS_5: lineControl.WordLength = 5; break;
	case COM422_CTRL_DATABITS_6: lineControl.WordLength = 6; break;
	case COM422_CTRL_DATABITS_7: lineControl.WordLength = 7; break;
	case COM422_CTRL_DATABITS_8: lineControl.WordLength = 8; break;
	default:
		status = STATUS_INVALID_PARAMETER;
	}

	HrKdPrint(("DataBits : %ld\n", value));

	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("DataBits -> Error: %08X\n", status));
		return FALSE;
	}

	// 
	// Get Parity
	// 
	value = COM422_PPORT_READ_CTRL_BITS(portContext,
		COM422_CTRL_PARITY_OFFSET, COM422_CTRL_PARITY_MASK);

	switch (value)
	{
	case COM422_CTRL_PARITY_NONE: lineControl.Parity = NO_PARITY; break;
	case COM422_CTRL_PARITY_ODD: lineControl.Parity = ODD_PARITY; break;
	case COM422_CTRL_PARITY_EVEN: lineControl.Parity = EVEN_PARITY; break;
	case COM422_CTRL_PARITY_MARK: lineControl.Parity = MARK_PARITY; break;
	case COM422_CTRL_PARITY_SPACE: lineControl.Parity = SPACE_PARITY; break;
	default:
		status = STATUS_INVALID_PARAMETER;
	}

	HrKdPrint(("Parity : %ld\n", value));

	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("Parity -> Error: %08X\n", status));
		return FALSE;
	}

	// 
	// Get StopBits
	// 
	value = COM422_PPORT_READ_CTRL_BITS(portContext,
		COM422_CTRL_STOPBITS_OFFSET, COM422_CTRL_STOPBITS_MASK);

	switch (value)
	{
	case COM422_CTRL_STOPBITS_1: lineControl.StopBits = STOP_BIT_1; break;
	case COM422_CTRL_STOPBITS_1_5: lineControl.StopBits = STOP_BITS_1_5; break;
	case COM422_CTRL_STOPBITS_2: lineControl.StopBits = STOP_BITS_2; break;
	default:
		status = STATUS_INVALID_PARAMETER;
	}

	HrKdPrint(("StopBits : %ld\n", value));

	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("StopBits -> Error: %08X\n", status));
		return FALSE;
	}

	HrKdPrint(("LineCtrl: Parity(%d), StopBits(%d), WordLength(%d)\n",
		lineControl.Parity,
		lineControl.StopBits,
		lineControl.WordLength));

	portContext->PortIoInfo.LineCtrl = lineControl;

	*serialLineControl = lineControl;

	HR_PRINT_METHOD_END();

	return TRUE;
}

BOOLEAN EvtSerialSetDTR(
	IN WDFINTERRUPT Interrupt,
	IN WDFCONTEXT Context)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	PULONG serialDTR;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PPDOPORT_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Context;
	serialDTR = (PULONG)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Data;

	COM422_PPORT_PRINT_CTRL(portContext);
	
	COM422_PPORT_WRITE_CTRL_BITS(portContext,
		COM422_CTRL_DTR_OFFSET, COM422_CTRL_DTR_MASK, *serialDTR);

	COM422_PPORT_PRINT_CTRL(portContext);

	CONFIG_REG_BITS_ULONG(portContext->PortIoInfo.ModemStatus,
		COM422_CTRL_DTR_OFFSET, COM422_CTRL_DTR_MASK, *serialDTR);

	HR_PRINT_METHOD_END();

	return TRUE;
}

BOOLEAN EvtSerialSetRTS(
	IN WDFINTERRUPT Interrupt,
	IN WDFCONTEXT Context)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	PULONG serialRTS;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PPDOPORT_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Context;
	serialRTS = (PULONG)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Data;

	COM422_PPORT_PRINT_CTRL(portContext);

	COM422_PPORT_WRITE_CTRL_BITS(portContext,
		COM422_CTRL_RTS_OFFSET, COM422_CTRL_RTS_MASK, *serialRTS);

	COM422_PPORT_PRINT_CTRL(portContext);

	CONFIG_REG_BITS_ULONG(portContext->PortIoInfo.ModemStatus,
		COM422_CTRL_RTS_OFFSET, COM422_CTRL_RTS_MASK, *serialRTS);

	HR_PRINT_METHOD_END();

	return TRUE;
}

BOOLEAN EvtSerialSetModemStatus(
	IN WDFINTERRUPT Interrupt,
	IN WDFCONTEXT Context)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	PULONG serialModemStatus;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PPDOPORT_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Context;
	serialModemStatus = (PULONG)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Data;

	COM422_PPORT_PRINT_CTRL(portContext);

	COM422_PPORT_WRITE_CTRL_BITS(portContext,
		COM422_CTRL_MODEMSTATUS_OFFSET, COM422_CTRL_MODEMSTATUS_MASK, *serialModemStatus);

	COM422_PPORT_PRINT_CTRL(portContext);

	CONFIG_REG_BITS_ULONG(portContext->PortIoInfo.ModemStatus,
		COM422_CTRL_MODEMSTATUS_OFFSET, COM422_CTRL_MODEMSTATUS_MASK, *serialModemStatus);

	HrKdPrint(("ModemStatus: %08X\n", portContext->PortIoInfo.ModemStatus));

	HR_PRINT_METHOD_END();

	return TRUE;
}

BOOLEAN EvtSerialGetModemStatus(
	IN WDFINTERRUPT Interrupt,
	IN WDFCONTEXT Context)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	PULONG serialModemStatus;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PPDOPORT_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Context;
	serialModemStatus = (PULONG)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Data;

	COM422_PPORT_PRINT_CTRL(portContext);

	*serialModemStatus = COM422_PPORT_READ_CTRL_BITS(portContext,
		COM422_CTRL_MODEMSTATUS_OFFSET, COM422_CTRL_MODEMSTATUS_MASK);

	CONFIG_REG_BITS_ULONG(portContext->PortIoInfo.ModemStatus,
		COM422_CTRL_MODEMSTATUS_OFFSET, COM422_CTRL_MODEMSTATUS_MASK, *serialModemStatus);

	HrKdPrint(("ModemStatus: %08X\n", portContext->PortIoInfo.ModemStatus));

	HR_PRINT_METHOD_END();

	return TRUE;
}

BOOLEAN EvtSerialPurge(
	IN WDFINTERRUPT Interrupt,
	IN WDFCONTEXT Context)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	PULONG purgeFlags;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PPDOPORT_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Context;
	purgeFlags = (PULONG)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Data;

	HrKdPrint(("Purge flags: %08X\n", *purgeFlags));

	if (*purgeFlags & ~(
		SERIAL_PURGE_TXABORT |  // Purges all write (IRP_MJ_WRITE) request.
		SERIAL_PURGE_RXABORT |	// Purges all read (IPR_MJ_READ) request.
		SERIAL_PURGE_TXCLEAR |	// Purges the transmit buffer,if one exists.
		SERIAL_PURGE_RXCLEAR))	// Purges the receive buffer,if one exists.
	{
		HrKdPrint(("Unknown mask: %08X\n", *purgeFlags));
		status = STATUS_INVALID_PARAMETER;
		return FALSE;
	}

	if (*purgeFlags & SERIAL_PURGE_RXCLEAR)
	{
		CircFifoClear(portContext->CircFifo);
	}

	portContext->PortIoInfo.PurgeFlags = *purgeFlags;

	HR_PRINT_METHOD_END();

	return TRUE;
}

BOOLEAN EvtSerialGetCommStatus(
	IN WDFINTERRUPT Interrupt,
	IN WDFCONTEXT Context)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	PSERIAL_STATUS serialStatus;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PPDOPORT_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Context;
	serialStatus = (PSERIAL_STATUS)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Data;
	RtlZeroMemory(serialStatus, sizeof(SERIAL_STATUS));

	serialStatus->AmountInInQueue = CircFifoCount(portContext->CircFifo);
	serialStatus->AmountInOutQueue = portContext->PortIoInfo.WriteCount - portContext->PortIoInfo.WriteOffset;

	HrKdPrint(("serialStatus -> AmountInInQueue: %ld, AmountInOutQueue: %ld\n",
		serialStatus->AmountInInQueue, serialStatus->AmountInOutQueue));

	HR_PRINT_METHOD_END();

	return TRUE;
}
