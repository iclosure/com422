
#include "precomp.h"
#include "ioctl.h"
#include "../ComDriverBus/regdef.h"
#include "timeout.h"
#include "waitmask.h"

DRIVER_CANCEL          IoCancelWrite;
DRIVER_STARTIO         StartIoWrite;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, QueueWriteCreate)
#pragma alloc_text(PAGE, IrpMajorWrite)
#endif

NTSTATUS QueueWriteCreate(
	IN PDEVICE_OBJECT DeviceObject)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	KeInitializeEvent(&portContext->EvtWriteInt, SynchronizationEvent, FALSE);

	KeInitializeDeviceQueue(&portContext->QueueWrite);

	KeInitializeTimerEx(&portContext->TimerWriteTotal, NotificationEvent);
	KeInitializeDpc(&portContext->TimerWriteTotalDpc, TimerWriteDpc, (PVOID)portContext);

	HR_PRINT_METHOD_END();

	return status;
}

VOID IoCancelWrite(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	PFDOPORT_CONTEXT_EXTENSION portContext;
	KIRQL oldIrql;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	KeCancelTimer(&portContext->TimerWriteTotal);

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

	KeAcquireSpinLock(&portContext->SpinLockWrite, &oldIrql);
	portContext->WriteIrp = NULL;
	KeReleaseSpinLock(&portContext->SpinLockWrite, oldIrql);

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_CANCELLED;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();
}

NTSTATUS IrpMajorWrite(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	PFDOPORT_CONTEXT_EXTENSION portContext;
	//KIRQL oldIrql;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	IoMarkIrpPending(Irp);
	
	IoSetCancelRoutine(Irp, IoCancelWrite);

	//KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);

	if (!KeInsertDeviceQueue(&portContext->QueueWrite, &Irp->Tail.Overlay.DeviceQueueEntry))
	{
		StartIoWrite(DeviceObject, Irp);
	}

	//KeLowerIrql(oldIrql);

	HR_PRINT_METHOD_END();

	return STATUS_PENDING;
}

#pragma LOCKEDCODE
VOID StartIoWrite(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION fportContext;
	PPDOPORT_CONTEXT_EXTENSION pportContext;
	PKDEVICE_QUEUE_ENTRY deviceQueueEntry;
	ULONG writeCount, writeOffset, txRoom;
	PUCHAR inBuffer;
	KIRQL oldIrql;
	LARGE_INTEGER timeout;

	ULONG  regWMask;

	HR_PRINT_METHOD_BEGIN();

	fportContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;
	pportContext = (PPDOPORT_CONTEXT_EXTENSION)fportContext->LowDevice->DeviceExtension;
	timeout.QuadPart = HR_REL_TIMEOUT_IN_SEC(1);
	do {
		writeCount = IoGetCurrentIrpStackLocation(Irp)->Parameters.Write.Length;
		
		if (writeCount == 0)
		{
			KeAcquireSpinLock(&fportContext->SpinLockWrite, &oldIrql);
			fportContext->WriteIrp = NULL;
			KeReleaseSpinLock(&fportContext->SpinLockWrite, oldIrql);

			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = STATUS_SUCCESS;
			IoCompleteRequest(Irp, IO_NO_INCREMENT);
		}
		else
		{
			KeAcquireSpinLock(&fportContext->SpinLockWrite, &oldIrql);
			fportContext->WriteIrp = Irp;
			KeReleaseSpinLock(&fportContext->SpinLockWrite, oldIrql);

			inBuffer = (PUCHAR)Irp->AssociatedIrp.SystemBuffer;
			writeOffset = 0;

			SetWriteTimeout(fportContext, Irp);

			do {
#ifdef DEBUG_WITH_REAL_DEVICE
				txRoom = COM422_PPORT_READ_TXROOM(pportContext);
				txRoom = min(txRoom, writeCount - writeOffset);

				if (txRoom > 0)
				{
					COM422_PPORT_WRITE_UCHARS(pportContext, &inBuffer[writeOffset], txRoom);
					writeOffset += txRoom;

					if (writeOffset == writeCount)
					{
						break;
					}
				}

				DbgPrint("## -> WaitIntEvent: Port=%02d\n", fportContext->PortIndex);
				// 开启中断等待上报
				WRITE_REGISTER_ULONG(pportContext->RegCtrl, READ_REGISTER_ULONG(pportContext->RegCtrl) | XXX_WRITE_MASK_BIT);
				KeWaitForSingleObject(&fportContext->EvtWriteInt, Executive, KernelMode, FALSE, NULL);
				DbgPrint("## <- WaitIntEvent: Port=%02d\n", fportContext->PortIndex);
				if (fportContext->WriteIrp == NULL)
				{
					DbgPrint("Write timeout\n");
					//COM422_PPORT_WRITE_INTMASK_BIT(pportContext, (pportContext->InstancePort + 16), 0UL);
					WRITE_REGISTER_ULONG(pportContext->RegCtrl, READ_REGISTER_ULONG(pportContext->RegCtrl) & ~XXX_WRITE_MASK_BIT);
					Irp->IoStatus.Status = STATUS_TIMEOUT;
					break;
				}
#else
				while (TRUE)
				{
					if (CircFifoIsFull(&fportContext->CircFifo))
					{
						Irp->IoStatus.Information = writeOffset;
						break;
					}
					else
					{
						CircFifoWrite(&fportContext->CircFifo, &inBuffer[writeOffset]);
						HrKdPrint(("WriteOffset: %ld, Data: %02X ('%c')\n", writeOffset, inBuffer[writeOffset], inBuffer[writeOffset]));

						if (++writeOffset == writeCount)
						{
							Irp->IoStatus.Information = writeOffset;
							break;
						}
					}
				}

				if (Irp->IoStatus.Information > 0)
				{
					//WaitMaskNotify(DeviceObject);
				}
				
#endif  // DEBUG_WITH_REAL_DEVICE
			} while (TRUE);

			KeCancelTimer(&fportContext->TimerWriteTotal);

			if (fportContext->WriteIrp != NULL)
			{
				KeAcquireSpinLock(&fportContext->SpinLockWrite, &oldIrql);
				fportContext->WriteIrp = NULL;
				KeReleaseSpinLock(&fportContext->SpinLockWrite, oldIrql);

				Irp->IoStatus.Information = writeOffset;
				Irp->IoStatus.Status = STATUS_SUCCESS;
				IoCompleteRequest(Irp, IO_NO_INCREMENT);
			}
		}

		// next queue

		deviceQueueEntry = KeRemoveDeviceQueue(&fportContext->QueueWrite);

		if (deviceQueueEntry == NULL)
		{
			break;
		}

		Irp = CONTAINING_RECORD(deviceQueueEntry, IRP, Tail.Overlay.DeviceQueueEntry);

	} while (TRUE);

	HR_PRINT_METHOD_END();
}
