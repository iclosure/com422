
#include "precomp.h"
#include "waitmask.h"

KDEFERRED_ROUTINE      WaitMaskTimerDpc;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, WaitMaskCreate)
#pragma alloc_text(PAGE, WaitMaskClose)
#endif

NTSTATUS WaitMaskCreate(
	IN PDEVICE_OBJECT DeviceObject)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	KeInitializeEvent(&portContext->EvtRxChars, SynchronizationEvent, FALSE);

	portContext->WaitMaskDueTime.QuadPart = HR_REL_TIMEOUT_IN_US(5);	// ? ms
	KeInitializeTimerEx(&portContext->WaitMaskTimer, NotificationEvent);
	KeInitializeDpc(&portContext->WaitMaskTimerDpc, WaitMaskTimerDpc, (PVOID)portContext);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS WaitMaskClose(
	IN PDEVICE_OBJECT DeviceObject)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	KeCancelTimer(&portContext->WaitMaskTimer);

	HR_PRINT_METHOD_END();

	return status;
}

#pragma LOCKEDCODE
VOID WaitMaskTimerDpc(
	IN PKDPC Dpc,
	IN OPTIONAL PVOID DeferredContext,
	IN OPTIONAL PVOID SystemArgument1,
	IN OPTIONAL PVOID SystemArgument2)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	KIRQL oldIrql;
	PIRP oldWaitIrp = NULL;
	ULONG events = 0;

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeferredContext;
	
	CircFifoGetSerialEvent(&portContext->CircFifo, &events);

	KeAcquireSpinLock(&portContext->SpinLockIoctl, &oldIrql);

	events &= portContext->WaitMask;

	if (portContext->WaitMaskIrp && events)
	{
		oldWaitIrp = portContext->WaitMaskIrp;

		if (IoSetCancelRoutine(oldWaitIrp, NULL) != NULL)
		{
			*(PULONG)oldWaitIrp->AssociatedIrp.SystemBuffer = events;
			oldWaitIrp->IoStatus.Information = sizeof(ULONG);
			oldWaitIrp->IoStatus.Status = STATUS_SUCCESS;
			portContext->WaitMaskIrp = NULL;
		}
		else
		{
			oldWaitIrp = NULL;
		}
	}

	KeReleaseSpinLock(&portContext->SpinLockIoctl, oldIrql);

	if (oldWaitIrp != NULL)
	{
		IoCompleteRequest(oldWaitIrp, IO_NO_INCREMENT);
	}
	else
	{
		KeSetTimer(&portContext->WaitMaskTimer, portContext->WaitMaskDueTime, &portContext->WaitMaskTimerDpc);
	}
}
