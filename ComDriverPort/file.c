
#include "precomp.h"
#include "file.h"
#include "waitmask.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, IrpMajorCreate)
#pragma alloc_text(PAGE, IrpMajorClose)
#pragma alloc_text(PAGE, IrpMajorQueryInformation)
#pragma alloc_text(PAGE, IrpMajorSetInformation)
#endif

NTSTATUS IrpMajorCreate(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	HrKdPrint(("PortIndex: %ld\n", portContext->PortIndex));

	if (portContext->IsOpened)
	{
		return STATUS_UNSUCCESSFUL;
	}

	// Initialize another member parameters

	portContext->Timeouts.ReadIntervalTimeout = MAXULONG;
	portContext->Timeouts.ReadTotalTimeoutConstant = 0;
	portContext->Timeouts.ReadTotalTimeoutMultiplier = 0;
	portContext->Timeouts.WriteTotalTimeoutConstant = 0;
	portContext->Timeouts.WriteTotalTimeoutMultiplier = 0;

	portContext->TxFifo = 4096;
	portContext->RxFifo = 4096;

	portContext->WaitMask = 0;
	portContext->WaitMaskIrp = NULL;
	portContext->WriteIrp = NULL;
	portContext->ReadIrp = NULL;

	// 
	CircFifoClear(&portContext->CircFifo);

	// 创建掩码等待例程
	status = WaitMaskCreate(DeviceObject);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WaitMaskCreate() failed as %08X\n", status));
		return status;
	}

	// 
	IoSkipCurrentIrpStackLocation(Irp);
	status = IoCallDriver(portContext->LowDevice, Irp);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("IoCallDriver() failed as %08X\n", status));
		return status;
	}

	portContext->IsOpened = TRUE;

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpMajorClose(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	KIRQL oldIrql;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	if (!portContext->IsOpened)
	{
		return STATUS_UNSUCCESSFUL;
	}

	// 
	IoSkipCurrentIrpStackLocation(Irp);
	status = IoCallDriver(portContext->LowDevice, Irp);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("IoCallDriver() failed as %08X\n", status));
		return status;
	}

	KeAcquireSpinLock(&portContext->SpinLockIoctl, &oldIrql);

	portContext->IsOpened = FALSE;

	KeReleaseSpinLock(&portContext->SpinLockIoctl, oldIrql);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpMajorCleanup(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	PKDEVICE_QUEUE_ENTRY deviceQueueEntry;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;
	
	// 关闭掩码等待例程
	status = WaitMaskClose(DeviceObject);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WaitMaskClose() failed as %08X\n", status));
		return status;
	}

	// Write Queue
	do {
		deviceQueueEntry = KeRemoveDeviceQueue(&portContext->QueueWrite);
		if (deviceQueueEntry == NULL)
		{
			break;
		}
		IoCancelIrp(CONTAINING_RECORD(deviceQueueEntry, IRP, Tail.Overlay.DeviceQueueEntry));
	} while (TRUE);

	// Read Queue
	do {
		deviceQueueEntry = KeRemoveDeviceQueue(&portContext->QueueRead);
		if (deviceQueueEntry == NULL)
		{
			break;
		}
		IoCancelIrp(CONTAINING_RECORD(deviceQueueEntry, IRP, Tail.Overlay.DeviceQueueEntry));
	} while (TRUE);

	if (portContext->WaitMaskIrp != NULL)
	{
		IoCancelIrp(portContext->WaitMaskIrp);
	}

	if (portContext->WriteIrp != NULL)
	{
		IoCancelIrp(portContext->WriteIrp);
	}

	if (portContext->ReadIrp != NULL)
	{
		IoCancelIrp(portContext->ReadIrp);
	}

	// 
	CircFifoClear(&portContext->CircFifo);

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	
	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpMajorQueryInformation(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	PIO_STACK_LOCATION ioStack;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	ioStack = IoGetCurrentIrpStackLocation(Irp);

	switch (ioStack->Parameters.QueryFile.FileInformationClass)
	{
	case FileStandardInformation:
	{
		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(FILE_STANDARD_INFORMATION))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		RtlZeroMemory(Irp->AssociatedIrp.SystemBuffer, sizeof(FILE_STANDARD_INFORMATION));
		Irp->IoStatus.Information = sizeof(FILE_STANDARD_INFORMATION);
		status = STATUS_SUCCESS;
		break;
	}
	case FilePositionInformation:
	{
		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(FILE_POSITION_INFORMATION))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		RtlZeroMemory(Irp->AssociatedIrp.SystemBuffer, sizeof(FILE_POSITION_INFORMATION));
		Irp->IoStatus.Information = sizeof(FILE_POSITION_INFORMATION);
		status = STATUS_SUCCESS;
		break;
	}
	default:
		Irp->IoStatus.Information = 0;
		status = STATUS_INVALID_PARAMETER;
		break;
	}

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpMajorSetInformation(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	PIO_STACK_LOCATION ioStack;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	ioStack = IoGetCurrentIrpStackLocation(Irp);

	switch (ioStack->Parameters.QueryFile.FileInformationClass)
	{
	case FileEndOfFileInformation:
	{
		status = STATUS_SUCCESS;
		break;
	}
	case FileAllInformation:
	{
		status = STATUS_SUCCESS;
		break;
	}
	default:
		status = STATUS_INVALID_PARAMETER;
		break;
	}

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}
