
#include "precomp.h"
#include "ioctl.h"

DRIVER_CANCEL IoCancelWaitIrp;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, IrpMajorInternalIoControl)
#pragma alloc_text(PAGE, IrpMajorDeviceControl)
#endif

NTSTATUS IrpMajorInternalIoControl(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	PIO_STACK_LOCATION ioStack;
	KIRQL oldIrql;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	ioStack = IoGetCurrentIrpStackLocation(Irp);

	Irp->IoStatus.Information = 0;

	HrKdPrint(("DeviceIoControl (INLEN: %ld, OUTLEN: %ld, CODE: %08X)\n",
		ioStack->Parameters.DeviceIoControl.InputBufferLength,
		ioStack->Parameters.DeviceIoControl.OutputBufferLength,
		ioStack->Parameters.DeviceIoControl.IoControlCode));

	switch (ioStack->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_SERIAL_INTERNAL_DO_WAIT_WAKE:	// 
	{
		status = STATUS_NOT_SUPPORTED;
		break;
	}
	case IOCTL_SERIAL_INTERNAL_CANCEL_WAIT_WAKE:	// 
	{
		status = STATUS_NOT_SUPPORTED;
		break;
	}
	case IOCTL_SERIAL_INTERNAL_BASIC_SETTINGS:	// 
	{
		PSERIAL_BASIC_SETTINGS basicSettings;

		basicSettings = (PSERIAL_BASIC_SETTINGS)Irp->AssociatedIrp.SystemBuffer;

		basicSettings->Timeouts = portContext->Timeouts;
		basicSettings->HandFlow = portContext->HandFlow;
		basicSettings->RxFifo = portContext->RxFifo;
		basicSettings->TxFifo = portContext->TxFifo;

		Irp->IoStatus.Information = sizeof(SERIAL_BASIC_SETTINGS);

		break;
	}
	case IOCTL_SERIAL_INTERNAL_RESTORE_SETTINGS:	// 
	{
		PSERIAL_BASIC_SETTINGS basicSettings;

		basicSettings = (PSERIAL_BASIC_SETTINGS)Irp->AssociatedIrp.SystemBuffer;

		portContext->Timeouts = basicSettings->Timeouts;
		portContext->HandFlow = basicSettings->HandFlow;
		portContext->RxFifo = basicSettings->RxFifo;
		portContext->TxFifo = basicSettings->TxFifo;

		Irp->IoStatus.Information = sizeof(SERIAL_BASIC_SETTINGS);

		break;
	}
	default:
		HrKdPrint(("(Error) STATUS_NOT_SUPPORTED\n"));
		status = STATUS_NOT_SUPPORTED;
		break;
	}

	Irp->IoStatus.Status = status;
	if (status != STATUS_PENDING)
	{
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
	}

	HR_PRINT_METHOD_END();

	return status;
}

VOID IoCancelWaitIrp(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	PFDOPORT_CONTEXT_EXTENSION portContext;
	KIRQL oldIrql;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	IoReleaseCancelSpinLock(Irp->CancelIrql);

	portContext->WaitMaskIrp = NULL;

	Irp->IoStatus.Status = STATUS_CANCELLED;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();
}

NTSTATUS IrpMajorDeviceControl(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	PIO_STACK_LOCATION irpStack;
	KIRQL oldIrql;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;
	irpStack = IoGetCurrentIrpStackLocation(Irp);
	
	Irp->IoStatus.Information = 0;

	HrKdPrint(("DeviceIoControl (INLEN: %ld, OUTLEN: %ld, CODE: %08X)\n",
		irpStack->Parameters.DeviceIoControl.InputBufferLength,
		irpStack->Parameters.DeviceIoControl.OutputBufferLength,
		irpStack->Parameters.DeviceIoControl.IoControlCode));

	switch (irpStack->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_SERIAL_SET_BAUD_RATE:	// 
	{
		PSERIAL_BAUD_RATE baudRate;

		HrKdPrint(("IOCTL_SERIAL_SET_BAUD_RATE\n"));

		baudRate = (PSERIAL_BAUD_RATE)Irp->AssociatedIrp.SystemBuffer;

		HrKdPrint(("BaudRate: %ld\n", baudRate->BaudRate));

		switch (baudRate->BaudRate)
		{
		case 50:
		case 75:
//		case 110:		// �ݲ�֧��
//		case 135:		// �ݲ�֧��
		case 300:
		case 600:
		case 1200:
		case 1800:
		case 2400:
		case 4800:
		case 7200:
		case 9600:
		case 14400:
		case 19200:
		case 38400:
//		case 56000:		// �ݲ�֧��
		case 57600:
		case 115200:
//		case 128000:	// �ݲ�֧��
		case 230400:
		case 460800:
		case 921600: break;
		//case 3125000: break;
		default:
			status = STATUS_INVALID_PARAMETER;
		}

		if (!NT_SUCCESS(status))
		{
			HrKdPrint(("BaudRate -> Error: %08X\n", status));
			break;
		}

		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);

		return status;
	}
	case IOCTL_SERIAL_SET_QUEUE_SIZE:	// Ŀǰ��΢��MDD�������û���ṩ֧��
	{
		PSERIAL_QUEUE_SIZE queueSize;

		HrKdPrint(("IOCTL_SERIAL_SET_QUEUE_SIZE\n"));

		queueSize = (PSERIAL_QUEUE_SIZE)Irp->AssociatedIrp.SystemBuffer;

		HrKdPrint(("InSize: %ld, OutSize: %ld\n", queueSize->InSize, queueSize->OutSize));

		KeAcquireSpinLock(&portContext->SpinLockIoctl, &oldIrql);

		if (queueSize->InSize > queueSize->OutSize)
		{
			portContext->QueueSize = *queueSize;
		}

		KeReleaseSpinLock(&portContext->SpinLockIoctl, oldIrql);

		Irp->IoStatus.Information = sizeof(SERIAL_QUEUE_SIZE);

		break;
	}
	case IOCTL_SERIAL_SET_LINE_CONTROL:		// 
	{
		PSERIAL_LINE_CONTROL lineControl;

		HrKdPrint(("IOCTL_SERIAL_SET_LINE_CONTROL\n"));

		lineControl = (PSERIAL_LINE_CONTROL)Irp->AssociatedIrp.SystemBuffer;

		HrKdPrint(("LineCtrl: Parity(%d), StopBits(%d), WordLength(%d)\n",
			lineControl->Parity,
			lineControl->StopBits,
			lineControl->WordLength));

		// DataBits
		switch (lineControl->WordLength)
		{
		case 5:
		case 6:
		case 7:
		case 8: break;
		default:
			status = STATUS_INVALID_PARAMETER;
		}

		// Parity
		switch (lineControl->Parity)
		{
		case NO_PARITY:
		case ODD_PARITY:
		case EVEN_PARITY:
		case MARK_PARITY:
		case SPACE_PARITY: break;
		default:
			status = STATUS_INVALID_PARAMETER;
		}

		// StopBits
		switch (lineControl->StopBits)
		{
		case STOP_BIT_1:
//		case STOP_BITS_1_5:		// ��ʱ��֧��
		case STOP_BITS_2: break;
		default:
			status = STATUS_INVALID_PARAMETER;
		}

		if (!NT_SUCCESS(status))
		{
			HrKdPrint(("LineControl -> Error: %08X\n", status));
			break;
		}

		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);

		return status;
	}
	case IOCTL_SERIAL_SET_BREAK_ON:		// �жϣ���ͣ����ǰ�ķ��ͻ����
	{
		HrKdPrint(("IOCTL_SERIAL_SET_BREAK_ON\n"));

		break;
	}
	case IOCTL_SERIAL_SET_BREAK_OFF:	// ���жϣ���ͣ��״̬�ָ�
	{
		HrKdPrint(("IOCTL_SERIAL_SET_BREAK_OFF\n"));

		break;
	}
	case IOCTL_SERIAL_IMMEDIATE_CHAR:	// Ϊ��չ���ܣ��ڷ�������ǰ����һ����־��
	{
		HrKdPrint(("IOCTL_SERIAL_IMMEDIATE_CHAR\n"));

		break;
	}
	case IOCTL_SERIAL_SET_TIMEOUTS:		// ���ô��ڵĶ�д������ʱ
	{
		PSERIAL_TIMEOUTS timeouts;

		HrKdPrint(("IOCTL_SERIAL_SET_TIMEOUTS\n"));

		timeouts = (PSERIAL_TIMEOUTS)Irp->AssociatedIrp.SystemBuffer;

		HrKdPrint(("RIT: %lu, RTTM: %lu, RTTC: %lu, WTTM: %lu, WTTC: %lu\n",
			timeouts->ReadIntervalTimeout,
			timeouts->ReadTotalTimeoutMultiplier,
			timeouts->ReadTotalTimeoutConstant,
			timeouts->WriteTotalTimeoutMultiplier,
			timeouts->WriteTotalTimeoutConstant));

		if (timeouts->ReadIntervalTimeout == MAXULONG &&
			timeouts->ReadTotalTimeoutMultiplier == MAXULONG &&
			timeouts->ReadTotalTimeoutConstant == MAXULONG)
		{
			HrKdPrint(("invalid input parameter: Timeouts\n"));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		KeAcquireSpinLock(&portContext->SpinLockIoctl, &oldIrql);
		portContext->Timeouts = *timeouts;
		KeReleaseSpinLock(&portContext->SpinLockIoctl, oldIrql);

		Irp->IoStatus.Information = sizeof(SERIAL_TIMEOUTS);

		break;
	}
	case IOCTL_SERIAL_GET_TIMEOUTS:		// ��ȡ���ڵĶ�д������ʱ
	{
		PSERIAL_TIMEOUTS timeouts;
		
		HrKdPrint(("IOCTL_SERIAL_GET_TIMEOUTS\n"));

		timeouts = (PSERIAL_TIMEOUTS)Irp->AssociatedIrp.SystemBuffer;

		KeAcquireSpinLock(&portContext->SpinLockIoctl, &oldIrql);
		*timeouts = portContext->Timeouts;
		KeReleaseSpinLock(&portContext->SpinLockIoctl, oldIrql);

		HrKdPrint(("RIT: %lu, RTTM: %lu, RTTC: %lu, WTTM: %lu, WTTC: %lu\n",
			timeouts->ReadIntervalTimeout,
			timeouts->ReadTotalTimeoutMultiplier,
			timeouts->ReadTotalTimeoutConstant,
			timeouts->WriteTotalTimeoutMultiplier,
			timeouts->WriteTotalTimeoutConstant));

		Irp->IoStatus.Information = sizeof(SERIAL_TIMEOUTS);

		break;
	}
	case IOCTL_SERIAL_SET_DTR:		// ���ߴ��ڵ�DTR�ܽ�
	{
		HrKdPrint(("IOCTL_SERIAL_SET_DTR\n"));

		if ((portContext->HandFlow.ControlHandShake & SERIAL_DTR_MASK) == SERIAL_DTR_HANDSHAKE)
		{
			HrKdPrint(("Error: Current ControlHandShake %08X\n", portContext->HandFlow.ControlHandShake));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);

		return status;
	}
	case IOCTL_SERIAL_CLR_DTR:		// ���ʹ��ڵ�DTR�ܽ�
	{
		HrKdPrint(("IOCTL_SERIAL_CLR_DTR\n"));

		if ((portContext->HandFlow.ControlHandShake & SERIAL_DTR_MASK) == SERIAL_DTR_HANDSHAKE)
		{
			HrKdPrint(("Error: Current ControlHandShake %08X\n", portContext->HandFlow.ControlHandShake));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);

		return status;
	}
	case IOCTL_SERIAL_RESET_DEVICE:
	{
		HrKdPrint(("IOCTL_SERIAL_RESET_DEVICE\n"));

		break;
	}
	case IOCTL_SERIAL_SET_RTS:		// ���ߴ��ڵ�RTS�ܽ�
	{
		HrKdPrint(("IOCTL_SERIAL_SET_RTS\n"));

		if ((portContext->HandFlow.ControlHandShake & SERIAL_RTS_MASK) == SERIAL_RTS_HANDSHAKE)
		{
			HrKdPrint(("Error: Current ControlHandShake %08X\n", portContext->HandFlow.ControlHandShake));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);

		return status;
	}
	case IOCTL_SERIAL_CLR_RTS:		// ���ʹ��ڵ�RTS�ܽ�
	{
		HrKdPrint(("IOCTL_SERIAL_CLR_RTS\n"));
		
		if ((portContext->HandFlow.ControlHandShake & SERIAL_RTS_MASK) == SERIAL_RTS_HANDSHAKE)
		{
			HrKdPrint(("Error: Current ControlHandShake %08X\n", portContext->HandFlow.ControlHandShake));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);

		return status;
	}
	case IOCTL_SERIAL_SET_XOFF:		// �����ģʽ�£���ֹ���ݴ���
	{
		HrKdPrint(("IOCTL_SERIAL_SET_XOFF\n"));

		break;
	}
	case IOCTL_SERIAL_SET_XON:		// �����ģʽ�£��������ݴ���
	{
		HrKdPrint(("IOCTL_SERIAL_SET_XON\n"));

		break;
	}
	case IOCTL_SERIAL_GET_WAIT_MASK:	// ��ȡ��ǰ���¼�����
	{
		HrKdPrint(("IOCTL_SERIAL_GET_WAIT_MASK\n"));

		KeAcquireSpinLock(&portContext->SpinLockIoctl, &oldIrql);
		*(PULONG)Irp->AssociatedIrp.SystemBuffer = portContext->WaitMask;
		KeReleaseSpinLock(&portContext->SpinLockIoctl, oldIrql);

		Irp->IoStatus.Information = sizeof(ULONG);

		break;
	}
	case IOCTL_SERIAL_SET_WAIT_MASK:	// �����¼��������������ԱȽ��鷳��Ҫ����ǰ��ȡ���¼�����mask�������е�Openʵ���У����ǰ���EvaluateEventFlag�������ơ�
	{
		PULONG newMask;
		PIRP oldWaitIrp = NULL;
			
		HrKdPrint(("IOCTL_SERIAL_SET_WAIT_MASK\n"));

		newMask = (PULONG)Irp->AssociatedIrp.SystemBuffer;

		if (*newMask & ~0x1FFF)
		{
			HrKdPrint(("Unknown mask: %08X\n", *newMask));
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		KeAcquireSpinLock(&portContext->SpinLockIoctl, &oldIrql);

		HrKdPrint(("portContext->WaitMask: %08X\n", portContext->WaitMask));

		if (portContext->WaitMaskIrp != NULL)
		{
			oldWaitIrp = portContext->WaitMaskIrp;

			KeCancelTimer(&portContext->WaitMaskTimer);

			if (IoSetCancelRoutine(oldWaitIrp, NULL) != NULL)
			{
				*(PULONG)oldWaitIrp->AssociatedIrp.SystemBuffer = 0;
				oldWaitIrp->IoStatus.Information = sizeof(ULONG);
				oldWaitIrp->IoStatus.Status = STATUS_SUCCESS;
				portContext->WaitMaskIrp = NULL;
			}
			else
			{
				oldWaitIrp = NULL;
			}
		}

		portContext->WaitMask = *newMask;

		KeReleaseSpinLock(&portContext->SpinLockIoctl, oldIrql);

		if (oldWaitIrp != NULL)
		{
			IoCompleteRequest(oldWaitIrp, IO_NO_INCREMENT);
		}

		break;
	}
	case IOCTL_SERIAL_WAIT_ON_MASK:		// �ȴ����ṩ���¼���ͬ���¼�������ʵ��ʵ����WaitCommEvent
	{
		HrKdPrint(("IOCTL_SERIAL_WAIT_ON_MASK\n"));

		//KeAcquireSpinLock(&portContext->SpinLockIoctl, &oldIrql);

		HrKdPrint(("portContext->WaitMask: %08X\n", portContext->WaitMask));

		// �ȴ�IRP���������WaitMask��Ϊ0
		if (portContext->WaitMaskIrp || !portContext->WaitMask)
		{
			status = STATUS_INVALID_PARAMETER;
		}
		else
		{
			IoMarkIrpPending(Irp);

			IoSetCancelRoutine(Irp, IoCancelWaitIrp);

			portContext->WaitMaskIrp = Irp;

			status = STATUS_PENDING;

			KeSetTimer(&portContext->WaitMaskTimer, portContext->WaitMaskDueTime, &portContext->WaitMaskTimerDpc);
		}

		//KeReleaseSpinLock(&portContext->SpinLockIoctl, oldIrql);

		break;
	}
	case IOCTL_SERIAL_PURGE:	// ��������е�����������壬Ҳ������ֹδ���еĶ�д����
	{
		HrKdPrint(("IOCTL_SERIAL_PURGE\n"));

		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);

		// 
		CircFifoClear(&portContext->CircFifo);

		return status;
	}
	case IOCTL_SERIAL_GET_BAUD_RATE:
	{
		HrKdPrint(("IOCTL_SERIAL_GET_BAUD_RATE\n"));

		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);

		return status;
	}
	case IOCTL_SERIAL_GET_LINE_CONTROL:
	{
		HrKdPrint(("IOCTL_SERIAL_GET_LINE_CONTROL\n"));

		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);

		return status;
	}
	case IOCTL_SERIAL_GET_CHARS:
	{
		PSERIAL_CHARS chars;

		HrKdPrint(("IOCTL_SERIAL_GET_CHARS\n"));

		chars = (PSERIAL_CHARS)Irp->AssociatedIrp.SystemBuffer;

		*chars = portContext->Chars;

		HrKdPrint(("SerialChars: EofChar(%02X), ErrorChar(%02X), "
			"BreakChar(%02X), EventChar(%02X), XonChar(%02X), XoffChar(%02X)\n",
			chars->EofChar,
			chars->ErrorChar,
			chars->BreakChar,
			chars->EventChar,
			chars->XonChar,
			chars->XoffChar));

		Irp->IoStatus.Information = sizeof(SERIAL_CHARS);

		break;
	}
	case IOCTL_SERIAL_SET_CHARS:
	{
		PSERIAL_CHARS chars;

		HrKdPrint(("IOCTL_SERIAL_SET_CHARS\n"));

		chars = (PSERIAL_CHARS)Irp->AssociatedIrp.SystemBuffer;

		HrKdPrint(("SerialChars: EofChar(%02X), ErrorChar(%02X), "
			"BreakChar(%02X), EventChar(%02X), XonChar(%02X), XoffChar(%02X)\n",
			chars->EofChar,
			chars->ErrorChar,
			chars->BreakChar,
			chars->EventChar,
			chars->XonChar,
			chars->XoffChar));
		/*
		if (chars->XoffChar == chars->XonChar)
		{
			HrKdPrint(("XoffChar(%02X) can not be equal to XonChar(%02X): \n", chars->XoffChar, chars->XonChar));
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		*/
		portContext->Chars = *chars;

		Irp->IoStatus.Information = sizeof(SERIAL_CHARS);

		break;
	}
	case IOCTL_SERIAL_GET_HANDFLOW:
	{
		PSERIAL_HANDFLOW handFlow;

		HrKdPrint(("IOCTL_SERIAL_GET_HANDFLOW\n"));

		handFlow = (PSERIAL_HANDFLOW)Irp->AssociatedIrp.SystemBuffer;

		*handFlow = portContext->HandFlow;

		HrKdPrint(("HandFlow: ControlHandShake(%08X), FlowReplace(%08X), XonLimit(%ld), XoffLimit(%ld)\n",
			handFlow->ControlHandShake,
			handFlow->FlowReplace,
			handFlow->XonLimit,
			handFlow->XoffLimit));

		Irp->IoStatus.Information = sizeof(SERIAL_HANDFLOW);

		break;
	}
	case IOCTL_SERIAL_SET_HANDFLOW:
	{
		PSERIAL_HANDFLOW handFlow;

		HrKdPrint(("IOCTL_SERIAL_SET_HANDFLOW\n"));

		handFlow = (PSERIAL_HANDFLOW)Irp->AssociatedIrp.SystemBuffer;

		HrKdPrint(("HandFlow: ControlHandShake(%08X), FlowReplace(%08X), XonLimit(%ld), XoffLimit(%ld)\n",
			handFlow->ControlHandShake,
			handFlow->FlowReplace,
			handFlow->XonLimit,
			handFlow->XoffLimit));

		if ((handFlow->ControlHandShake & SERIAL_CONTROL_INVALID) ||
			(handFlow->ControlHandShake & SERIAL_FLOW_INVALID) ||
			((handFlow->ControlHandShake & SERIAL_DTR_MASK) == SERIAL_DTR_MASK))
		{
			HrKdPrint(("Unknown ControlHandShake: %08X\n", handFlow->ControlHandShake));
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		/*
		if (handFlow->XonLimit < 0 || ((ULONG)handFlow->XoffLimit > portContext->QueueSize.InSize))
		{
			HrKdPrint(("Error: XoffLimit(%ld) > InSize(%ld)\n", (ULONG)handFlow->XoffLimit, portContext->QueueSize.InSize));
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		*/
		portContext->HandFlow = *handFlow;

		Irp->IoStatus.Information = sizeof(SERIAL_HANDFLOW);

		break;
	}
	case IOCTL_SERIAL_GET_MODEMSTATUS:
	{
		HrKdPrint(("IOCTL_SERIAL_GET_MODEMSTATUS\n"));

		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);

		return status;
	}
	case IOCTL_SERIAL_GET_COMMSTATUS:		// ����쳣�����ص�ǰ״̬
	{
		PSERIAL_STATUS serialStatus;

		HrKdPrint(("IOCTL_SERIAL_GET_COMMSTATUS\n"));

		serialStatus = (PSERIAL_STATUS)Irp->AssociatedIrp.SystemBuffer;

		RtlZeroMemory(serialStatus, sizeof(SERIAL_STATUS));

		serialStatus->AmountInInQueue = CircFifoCount(&portContext->CircFifo);

		HrKdPrint(("AmountInInQueue: %ld, AmountInOutQueue: %ld\n", serialStatus->AmountInInQueue, serialStatus->AmountInOutQueue));

		Irp->IoStatus.Information = sizeof(SERIAL_STATUS);

		break;
	}
	case IOCTL_SERIAL_XOFF_COUNTER:
	{
		HrKdPrint(("IOCTL_SERIAL_XOFF_COUNTER\n"));

		break;
	}
	case IOCTL_SERIAL_GET_PROPERTIES:		// ��ȡͨѶ...
	{
		HrKdPrint(("IOCTL_SERIAL_GET_PROPERTIES\n"));

		break;
	}
	case IOCTL_SERIAL_GET_DTRRTS:	// 
	{
		HrKdPrint(("IOCTL_SERIAL_GET_DTRRTS\n"));

		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);

		return status;
	}
	default:
		HrKdPrint(("(Error) STATUS_NOT_SUPPORTED\n"));
		status = STATUS_NOT_SUPPORTED;
		break;
	}

	Irp->IoStatus.Status = status;
	if (status != STATUS_PENDING)
	{
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
	}

	HR_PRINT_METHOD_END();

	return status;
}
