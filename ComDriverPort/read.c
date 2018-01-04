
#include "precomp.h"
#include "ioctl.h"
#include "timeout.h"

DRIVER_CANCEL          IoCancelRead;
DRIVER_STARTIO         StartIoRead;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, QueueReadCreate)
#pragma alloc_text(PAGE, IrpMajorRead)
#pragma alloc_text(PAGE, StartIoRead)
#endif

NTSTATUS QueueReadCreate(
	IN PDEVICE_OBJECT DeviceObject)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	KeInitializeDeviceQueue(&portContext->QueueRead);

	KeInitializeTimerEx(&portContext->TimerReadTotal, NotificationEvent);
	KeInitializeDpc(&portContext->TimerReadTotalDpc, TimerReadDpc, (PVOID)portContext);

	KeInitializeTimerEx(&portContext->TimerReadInterval, NotificationEvent);
	KeInitializeDpc(&portContext->TimerReadIntervalDpc, TimerReadDpc, (PVOID)portContext);

	HR_PRINT_METHOD_END();

	return status;
}

VOID IoCancelRead(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	PFDOPORT_CONTEXT_EXTENSION portContext;
	KIRQL oldIrql;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	KeCancelTimer(&portContext->TimerReadTotal);
	KeCancelTimer(&portContext->TimerReadInterval);

	if (Irp == DeviceObject->CurrentIrp)
	{
		KIRQL oldIrql = Irp->CancelIrql;

		IoReleaseCancelSpinLock(Irp->CancelIrql);

		IoStartNextPacket(DeviceObject, TRUE);

		KeLowerIrql(oldIrql);
	}
	else
	{
		KeRemoveEntryDeviceQueue(&DeviceObject->DeviceQueue, &Irp->Tail.Overlay.DeviceQueueEntry);

		IoReleaseCancelSpinLock(Irp->CancelIrql);
	}

	KeAcquireSpinLock(&portContext->SpinLockRead, &oldIrql);
	portContext->ReadIrp = NULL;
	KeReleaseSpinLock(&portContext->SpinLockRead, oldIrql);

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_CANCELLED;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();
}

NTSTATUS IrpMajorRead(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	PFDOPORT_CONTEXT_EXTENSION portContext;
	KIRQL oldIrql;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	IoMarkIrpPending(Irp);

	IoSetCancelRoutine(Irp, IoCancelRead);

	KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);

	if (!KeInsertDeviceQueue(&portContext->QueueRead, &Irp->Tail.Overlay.DeviceQueueEntry))
	{
		StartIoRead(DeviceObject, Irp);
	}

	KeLowerIrql(oldIrql);

	HR_PRINT_METHOD_END();

	return STATUS_PENDING;
}

#pragma LOCKEDCODE
VOID StartIoRead(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	PPDOPORT_CONTEXT_EXTENSION pdoPortContext;
	PKDEVICE_QUEUE_ENTRY deviceQueueEntry;
	LARGE_INTEGER timeout;
	KIRQL oldIrql;
	PUCHAR outBuffer;
	ULONG readCount, readOffset, recvCount;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;
	pdoPortContext = (PPDOPORT_CONTEXT_EXTENSION)portContext->LowDevice->DeviceExtension;

	do {
		readCount = IoGetCurrentIrpStackLocation(Irp)->Parameters.Read.Length;

		if (readCount == 0)
		{
			KeCancelTimer(&portContext->TimerReadTotal);

			KeAcquireSpinLock(&portContext->SpinLockRead, &oldIrql);

			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = STATUS_SUCCESS;
			portContext->ReadIrp = NULL;

			KeReleaseSpinLock(&portContext->SpinLockRead, oldIrql);

			IoCompleteRequest(Irp, IO_NO_INCREMENT);
		}
		else
		{
			KeAcquireSpinLock(&portContext->SpinLockRead, &oldIrql);
			portContext->ReadIrp = Irp;
			KeReleaseSpinLock(&portContext->SpinLockRead, oldIrql);
			
			outBuffer = (PUCHAR)Irp->AssociatedIrp.SystemBuffer;
			readOffset = 0;
			Irp->IoStatus.Information = 0;

			SetReadTimeout(portContext, Irp);

			timeout.QuadPart = HR_REL_TIMEOUT_IN_SEC(1);
			
			while (TRUE)
			{
				SetReadTimeout(portContext, Irp);

				recvCount = CircFifoReadBuffer(&portContext->CircFifo, &outBuffer[readOffset], readCount - readOffset);

				if (recvCount > 0)
				{
					readOffset += recvCount;

					if (readOffset == readCount)
					{
						Irp->IoStatus.Information = readOffset;
						break;
					}
				}
				else
				{
					status = KeWaitForSingleObject(&portContext->EvtRxChars, Executive, KernelMode, FALSE, &timeout);
					if (portContext->ReadIrp == NULL)
					{
						DbgPrint("Read timeout\n");
						Irp->IoStatus.Status = STATUS_TIMEOUT;
						break;
					}
				}

				KeCancelTimer(&portContext->TimerReadInterval);

				if (portContext->ReadIrp == NULL)
				{
					break;
				}
			}

			KeCancelTimer(&portContext->TimerReadTotal);

			if (portContext->ReadIrp != NULL)
			{
				KeAcquireSpinLock(&portContext->SpinLockRead, &oldIrql);
				portContext->ReadIrp = NULL;
				KeReleaseSpinLock(&portContext->SpinLockRead, oldIrql);

				IoCompleteRequest(Irp, IO_NO_INCREMENT);
			}
		}

		// next queue

		deviceQueueEntry = KeRemoveDeviceQueue(&portContext->QueueRead);

		if (deviceQueueEntry == NULL)
		{
			break;
		}

		Irp = CONTAINING_RECORD(deviceQueueEntry, IRP, Tail.Overlay.DeviceQueueEntry);

	} while (TRUE);

	HR_PRINT_METHOD_END();
}
