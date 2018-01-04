
#include "precomp.h"
#include "wmi.h"
#include <wmistr.h>

// DRIVER-CONTEXT
extern DRIVER_CONTEXT_EXTENSION _v_globalDrvContext;

/*
 * WMI - SERIALPORT - Global variables
 */

//---GUID

GUID _v_serialPortWmiNameGuid = SERIAL_PORT_WMI_NAME_GUID;
GUID _v_serialPortWmiCommGuid = SERIAL_PORT_WMI_COMM_GUID;
#ifdef COM422_ALLOW_WMI_HW_GUID
GUID _v_serialPortWmiHwGuid = SERIAL_PORT_WMI_HW_GUID;
#endif
GUID _v_serialPortWmiPerfGuid = SERIAL_PORT_WMI_PERF_GUID;
GUID _v_serialPortWmiPropertiesGuid = SERIAL_PORT_WMI_PROPERTIES_GUID;

//---ENUM: GUID_INDEX
enum COM422_FPORT_GUID_INDEX
{
	COM422_FPORT_GUID_INDEX_NAME,
	COM422_FPORT_GUID_INDEX_COMM,
#ifdef COM422_ALLOW_WMI_HW_GUID
	COM422_FPORT_GUID_INDEX_HW,
#endif
	COM422_FPORT_GUID_INDEX_PERF,
	COM422_FPORT_GUID_INDEX_PROPERTIES,

	COM422_FPORT_GUID_COUNT
};

//---WMIGUIDREGINFO

WMIGUIDREGINFO _v_wmiGuidRegInfo[COM422_FPORT_GUID_COUNT] =
{
	{ &_v_serialPortWmiNameGuid, 1, 0 },
	{ &_v_serialPortWmiCommGuid, 1, 0 },
#ifdef COM422_ALLOW_WMI_HW_GUID
	{ &_v_serialPortWmiHwGuid, 1, 0 },
#endif
	{ &_v_serialPortWmiPerfGuid, 1, 0 },
	{ &_v_serialPortWmiPropertiesGuid, 1, 0 },
};

//---WMILIB_CONTEXT

WMILIB_CONTEXT _v_wmiLibContext =
{
	COM422_FPORT_GUID_COUNT,

	_v_wmiGuidRegInfo,

	QueryWmiRegInfo,
	QueryWmiDataBlock,
	SetWmiDataBlock,
	SetWmiDataItem,
	ExecuteWmiMethod,
	WmiFunctionControl
};

/*
 * ---
 */

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, WmiSystemControlCreate)
#pragma alloc_text(PAGE, IrpMajorSystemControl)
#pragma alloc_text(PAGE, IrpQueryAllData)
#pragma alloc_text(PAGE, IrpQuerySingleInstance)
#pragma alloc_text(PAGE, IrpChangeSingleInstance)
#pragma alloc_text(PAGE, IrpChangeSingleItem)
#pragma alloc_text(PAGE, IrpEnableEvents)
#pragma alloc_text(PAGE, IrpDisableEvents)
#pragma alloc_text(PAGE, IrpEnableCollection)
#pragma alloc_text(PAGE, IrpDisableCollection)
#pragma alloc_text(PAGE, IrpReginfo)
#pragma alloc_text(PAGE, IrpExecuteMethod)
#pragma alloc_text(PAGE, IrpSystemControlDefault)
#endif

NTSTATUS WmiSystemControlCreate(
	IN PDEVICE_OBJECT DeviceObject)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpMajorSystemControl(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_NO_SUCH_DEVICE;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	SYSCTL_IRP_DISPOSITION disposition;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	status = WmiSystemControl(&_v_wmiLibContext, DeviceObject, Irp, &disposition);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WmiSystemControl() failed as %08X\n", status));
		Irp->IoStatus.Status = status;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	}

	switch (disposition)
	{
	case IrpProcessed:
	{
		break;
	}
	case IrpNotCompleted:
	{
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		break;
	}
	case IrpNotWmi:
	{
		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);
		break;
	}
	case IrpForward:
	{
		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);
		break;
	}
	default:
		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(portContext->LowDevice, Irp);
		break;
	}
	/*
	switch (IoGetCurrentIrpStackLocation(Irp)->MinorFunction)
	{
	case IRP_MN_QUERY_ALL_DATA:
	{
		status = IrpQueryAllData(DeviceObject, Irp);
		break;
	}
	case IRP_MN_QUERY_SINGLE_INSTANCE:
	{
		status = IrpQuerySingleInstance(DeviceObject, Irp);
		break;
	}
	case IRP_MN_CHANGE_SINGLE_INSTANCE:
	{
		status = IrpChangeSingleInstance(DeviceObject, Irp);
		break;
	}
	case IRP_MN_CHANGE_SINGLE_ITEM:
	{
		status = IrpChangeSingleItem(DeviceObject, Irp);
		break;
	}
	case IRP_MN_ENABLE_EVENTS:
	{
		status = IrpEnableEvents(DeviceObject, Irp);
		break;
	}
	case IRP_MN_DISABLE_EVENTS:
	{
		status = IrpDisableEvents(DeviceObject, Irp);
		break;
	}
	case IRP_MN_ENABLE_COLLECTION:
	{
		status = IrpEnableCollection(DeviceObject, Irp);
		break;
	}
	case IRP_MN_DISABLE_COLLECTION:
	{
		status = IrpDisableCollection(DeviceObject, Irp);
		break;
	}
	case IRP_MN_REGINFO:
	{
		status = IrpReginfo(DeviceObject, Irp);
		break;
	}
	case IRP_MN_EXECUTE_METHOD:
	{
		status = IrpExecuteMethod(DeviceObject, Irp);
		break;
	}
	case IRP_MN_REGINFO_EX:
	{
		status = STATUS_NOT_SUPPORTED;
		break;
	}
	default:
		status = STATUS_NOT_SUPPORTED;
		break;
	}
	*/
	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpSystemControlDefault(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	IoSkipCurrentIrpStackLocation(Irp);
	status = IoCallDriver(portContext->LowDevice, Irp);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpQueryAllData(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpQuerySingleInstance(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpChangeSingleInstance(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpChangeSingleItem(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpEnableEvents(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpDisableEvents(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpEnableCollection(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpDisableCollection(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpReginfo(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpExecuteMethod(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	HR_PRINT_METHOD_END();

	return status;
}

//---

NTSTATUS QueryWmiRegInfo(
	IN PDEVICE_OBJECT DeviceObject,
	IN OUT PULONG RegFlags,
	IN OUT PUNICODE_STRING InstanceName,
	OUT OPTIONAL PUNICODE_STRING *RegistryPath,
	IN OUT PUNICODE_STRING MofResourceName,
	OUT OPTIONAL PDEVICE_OBJECT *Pdo)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	*RegFlags = WMIREG_FLAG_INSTANCE_PDO;

	*RegistryPath = &_v_globalDrvContext.RegistryPath;

	*Pdo = portContext->LowDevice;

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS QueryWmiDataBlock(
	IN OUT PDEVICE_OBJECT DeviceObject,
	IN OUT PIRP Irp,
	IN ULONG GuidIndex,
	IN ULONG InstanceIndex,
	IN ULONG InstanceCount,
	OUT PULONG InstanceLengthArray,
	IN ULONG BufferAvail,
	OUT PUCHAR Buffer)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION fportContext;
	PPDOPORT_CONTEXT_EXTENSION pportContext;
	KIRQL oldIrql;
	ULONG bufferUsed = 0;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	fportContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;
	pportContext = (PPDOPORT_CONTEXT_EXTENSION)fportContext->LowDevice->DeviceExtension;

	switch (GuidIndex)
	{
	case COM422_FPORT_GUID_INDEX_NAME:
	{
		bufferUsed = fportContext->PortName.Length + sizeof(USHORT);

		if (BufferAvail < bufferUsed)
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		*(PUSHORT)Buffer = fportContext->PortName.Length;

		RtlCopyMemory(Buffer + sizeof(USHORT), fportContext->PortName.Buffer, fportContext->PortName.Length);

		status = STATUS_SUCCESS;

		break;
	}
	case COM422_FPORT_GUID_INDEX_COMM:
	{
		PSERIAL_WMI_COMM_DATA commData;

		bufferUsed = sizeof(SERIAL_WMI_COMM_DATA);

		if (BufferAvail < bufferUsed)
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		commData = (PSERIAL_WMI_COMM_DATA)Buffer;

		KeAcquireSpinLock(&fportContext->SpinLockIoctl, &oldIrql);

		// BaudRate

		commData->BaudRate = pportContext->PortIoInfo.BaudRate.BaudRate;

		// DataBits

		commData->BitsPerByte = pportContext->PortIoInfo.LineCtrl.WordLength;

		// Parity

		commData->ParityCheckEnable = TRUE;
		switch (pportContext->PortIoInfo.LineCtrl.Parity)
		{
		case NO_PARITY:
		{
			commData->Parity = SERIAL_WMI_PARITY_NONE;
			commData->ParityCheckEnable = FALSE;
			break;
		}
		case ODD_PARITY:
		{
			commData->Parity = SERIAL_WMI_PARITY_ODD;
			break;
		}
		case EVEN_PARITY:
		{
			commData->Parity = SERIAL_WMI_PARITY_EVEN;
			break;
		}
		case MARK_PARITY:
		{
			commData->Parity = SERIAL_WMI_PARITY_MARK;
			break;
		}
		case SPACE_PARITY:
		{
			commData->Parity = SERIAL_WMI_PARITY_SPACE;
			break;
		}
		default:
			ASSERTMSG("Invalid parity", FALSE);
			break;
		}

		// StopBits

		switch (pportContext->PortIoInfo.LineCtrl.StopBits)
		{
		case STOP_BIT_1:
		{
			commData->StopBits = SERIAL_WMI_STOP_1;
			break;
		}
		case STOP_BITS_1_5:
		{
			commData->StopBits = SERIAL_WMI_STOP_1_5;
			break;
		}
		case STOP_BITS_2:
		{
			commData->StopBits = SERIAL_WMI_STOP_2;
			break;
		}
		default:
			ASSERTMSG("Invalid stopBits", FALSE);
			break;
		}

		// Others

		commData->XoffCharacter = pportContext->PortIoInfo.specialChars.XoffChar;
		commData->XoffXmitThreshold = pportContext->PortIoInfo.handFlow.XoffLimit;
		commData->XonCharacter = pportContext->PortIoInfo.specialChars.XonChar;
		commData->XonXmitThreshold = pportContext->PortIoInfo.handFlow.XonLimit;

		KeReleaseSpinLock(&fportContext->SpinLockIoctl, oldIrql);

		// 

		commData->MaximumBaudRate = 3125000UL;
		commData->MaximumInputBufferSize = (ULONG)-1L;
		commData->MaximumOutputBufferSize = (ULONG)-1L;
		commData->Support16BitMode = FALSE;
		commData->SupportDTRDSR = FALSE;
		commData->SupportRTSCTS = FALSE;
		commData->SupportIntervalTimeouts = TRUE;
		commData->SupportParityCheck = TRUE;
		commData->SupportXonXoff = FALSE;
		commData->SettableBaudRate = TRUE;
		commData->SettableDataBits = TRUE;
		commData->SettableParityCheck = TRUE;
		commData->SettableParity = TRUE;
		commData->SettableStopBits = TRUE;
		commData->SettableFlowControl = FALSE;

		commData->IsBusy = fportContext->IsOpened;

		status = STATUS_SUCCESS;

		break;
	}
#ifdef COM422_ALLOW_WMI_HW_GUID
	case COM422_FPORT_GUID_INDEX_HW:
	{
		bufferUsed = sizeof(SERIAL_WMI_HW_DATA);

		if (BufferAvail < bufferUsed)
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		RtlZeroMemory(Buffer, bufferUsed);

		status = STATUS_SUCCESS;

		break;
	}
#endif
	case COM422_FPORT_GUID_INDEX_PERF:
	{
		PSERIAL_WMI_PERF_DATA perfData;

		bufferUsed = sizeof(SERIAL_WMI_PERF_DATA);

		if (BufferAvail < bufferUsed)
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		perfData = (PSERIAL_WMI_PERF_DATA)Buffer;

		KeAcquireSpinLock(&fportContext->SpinLockIoctl, &oldIrql);

		perfData->ReceivedCount = pportContext->PortIoInfo.perfStats.ReceivedCount;
		perfData->TransmittedCount = pportContext->PortIoInfo.perfStats.TransmittedCount;
		perfData->FrameErrorCount = pportContext->PortIoInfo.perfStats.FrameErrorCount;
		perfData->SerialOverrunErrorCount = pportContext->PortIoInfo.perfStats.SerialOverrunErrorCount;
		perfData->BufferOverrunErrorCount = pportContext->PortIoInfo.perfStats.BufferOverrunErrorCount;
		perfData->ParityErrorCount = pportContext->PortIoInfo.perfStats.ParityErrorCount;

		KeReleaseSpinLock(&fportContext->SpinLockIoctl, oldIrql);

		status = STATUS_SUCCESS;

		break;
	}
	case COM422_FPORT_GUID_INDEX_PROPERTIES:
	{
		PSERIAL_COMMPROP commProp;

		bufferUsed = sizeof(SERIAL_COMMPROP);

		if (BufferAvail < bufferUsed)
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		commProp = (PSERIAL_COMMPROP)Buffer;

		RtlZeroMemory(Buffer, bufferUsed);

		commProp->PacketLength = sizeof(SERIAL_COMMPROP);
		commProp->PacketVersion = 2;
		commProp->ServiceMask = SERIAL_SP_SERIALCOMM;
		commProp->MaxTxQueue = 0;
		commProp->MaxRxQueue = 0;

		commProp->MaxBaud = SERIAL_BAUD_USER;
		commProp->SettableBaud =
			SERIAL_BAUD_075 |
//			SERIAL_BAUD_110 |
//			SERIAL_BAUD_134_5 |
//			SERIAL_BAUD_150 |
			SERIAL_BAUD_300 |
			SERIAL_BAUD_600 |
			SERIAL_BAUD_1200 |
			SERIAL_BAUD_1800 |
			SERIAL_BAUD_2400 |
			SERIAL_BAUD_4800 |
			SERIAL_BAUD_7200 |
			SERIAL_BAUD_9600 |
			SERIAL_BAUD_14400 |
			SERIAL_BAUD_19200 |
			SERIAL_BAUD_38400 |
//			SERIAL_BAUD_56K |
//			SERIAL_BAUD_128K |
			SERIAL_BAUD_115200 |
			SERIAL_BAUD_57600 |
			SERIAL_BAUD_USER;

		commProp->ProvSubType = SERIAL_SP_RS422;

		commProp->ProvCapabilities =
//			SERIAL_PCF_DTRDSR |
//			SERIAL_PCF_RTSCTS |
//			SERIAL_PCF_CD |
			SERIAL_PCF_PARITY_CHECK |
//			SERIAL_PCF_XONXOFF |
			SERIAL_PCF_SETXCHAR |
			SERIAL_PCF_TOTALTIMEOUTS |
			SERIAL_PCF_INTTIMEOUTS;

		commProp->SettableParams =
			SERIAL_SP_BAUD |
			SERIAL_SP_DATABITS |
			SERIAL_SP_PARITY_CHECK |
			SERIAL_SP_PARITY |
			SERIAL_SP_STOPBITS |
			SERIAL_SP_HANDSHAKING |
			SERIAL_SP_CARRIER_DETECT;

		commProp->SettableStopParity =
			SERIAL_STOPBITS_10 |
			SERIAL_STOPBITS_15 |
			SERIAL_STOPBITS_20 |
			SERIAL_PARITY_NONE |
			SERIAL_PARITY_ODD |
			SERIAL_PARITY_EVEN |
			SERIAL_PARITY_MARK |
			SERIAL_PARITY_SPACE;

		commProp->CurrentTxQueue = 0;
		commProp->CurrentRxQueue = (ULONG)CircFifoCount(&fportContext->CircFifo);

		status = STATUS_SUCCESS;

		break;
	}
	default:
		bufferUsed = 0;
		status = STATUS_WMI_GUID_NOT_FOUND;
		break;
	}

	status = WmiCompleteRequest(DeviceObject, Irp, status, bufferUsed, IO_NO_INCREMENT);
	
	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS SetWmiDataBlock(
	IN OUT PDEVICE_OBJECT DeviceObject,
	IN OUT PIRP Irp,
	IN ULONG GuidIndex,
	IN ULONG InstanceIndex,
	IN ULONG BufferSize,
	IN PUCHAR Buffer)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	KIRQL oldIrql;
	ULONG bufferUsed = 0;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	status = STATUS_WMI_GUID_NOT_FOUND;

	status = WmiCompleteRequest(DeviceObject, Irp, status, bufferUsed, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS SetWmiDataItem(
	IN OUT PDEVICE_OBJECT DeviceObject,
	IN OUT PIRP Irp,
	IN ULONG GuidIndex,
	IN ULONG InstanceIndex,
	IN ULONG DataItemId,
	IN ULONG BufferSize,
	IN PUCHAR Buffer)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	KIRQL oldIrql;
	ULONG bufferUsed = 0;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	status = STATUS_WMI_GUID_NOT_FOUND;

	status = WmiCompleteRequest(DeviceObject, Irp, status, bufferUsed, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS ExecuteWmiMethod(
	IN OUT PDEVICE_OBJECT DeviceObject,
	IN OUT PIRP Irp,
	IN ULONG GuidIndex,
	IN ULONG InstanceIndex,
	IN ULONG MethodId,
	IN ULONG InBufferSize,
	IN ULONG OutBufferSize,
	IN OUT PUCHAR Buffer)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	ULONG bufferUsed = 0;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	status = STATUS_WMI_GUID_NOT_FOUND;

	status = WmiCompleteRequest(DeviceObject, Irp, status, bufferUsed, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS WmiFunctionControl(
	IN OUT PDEVICE_OBJECT DeviceObject,
	IN OUT PIRP Irp,
	IN ULONG GuidIndex,
	IN WMIENABLEDISABLECONTROL Function,
	IN BOOLEAN Enable)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	ULONG bufferUsed = 0;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	status = STATUS_WMI_GUID_NOT_FOUND;

	status = WmiCompleteRequest(DeviceObject, Irp, status, bufferUsed, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}
