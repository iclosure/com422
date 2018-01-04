
#include "precomp.h"
#include "timeout.h"

#pragma LOCKEDCODE
VOID TimerWriteDpc(
	IN PKDPC Dpc,
	IN OPTIONAL PVOID DeferredContext,
	IN OPTIONAL PVOID SystemArgument1,
	IN OPTIONAL PVOID SystemArgument2)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION fportContext;
	PPDOPORT_CONTEXT_EXTENSION pportContext;
	PIRP oldWriteIrp = NULL;
	KIRQL oldIrql;
	ULONG regWMask;

	HR_PRINT_METHOD_BEGIN();

	fportContext = (PFDOPORT_CONTEXT_EXTENSION)DeferredContext;
	pportContext = (PPDOPORT_CONTEXT_EXTENSION)fportContext->LowDevice->DeviceExtension;

	KeAcquireSpinLock(&fportContext->SpinLockWrite, &oldIrql);

	if (fportContext->WriteIrp != NULL)
	{
		oldWriteIrp = fportContext->WriteIrp;

		if (IoSetCancelRoutine(oldWriteIrp, NULL) != NULL)
		{
			oldWriteIrp->IoStatus.Status = STATUS_TIMEOUT;
			fportContext->WriteIrp = NULL;
		}
		else
		{
			oldWriteIrp = NULL;
		}
	}

	KeReleaseSpinLock(&fportContext->SpinLockWrite, oldIrql);

	//
	DbgPrint("## -> TmrIntEvent: port=%02d\n", fportContext->PortIndex);
	KeSetEvent(&fportContext->EvtWriteInt, IO_NO_INCREMENT, FALSE);

	if (oldWriteIrp != NULL)
	{
		// TODO: ½ûÖ¹Ð´¶Ë¿ÚÖÐ¶Ï
		WRITE_REGISTER_ULONG(pportContext->RegCtrl, READ_REGISTER_ULONG(pportContext->RegCtrl) & ~XXX_WRITE_MASK_BIT);
		IoCompleteRequest(oldWriteIrp, IO_NO_INCREMENT);
	}

	HR_PRINT_METHOD_END();
}

#pragma LOCKEDCODE
VOID TimerReadDpc(
	IN PKDPC Dpc,
	IN OPTIONAL PVOID DeferredContext,
	IN OPTIONAL PVOID SystemArgument1,
	IN OPTIONAL PVOID SystemArgument2)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	PIRP oldReadIrp = NULL;
	KIRQL oldIrql;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeferredContext;

	KeAcquireSpinLock(&portContext->SpinLockRead, &oldIrql);

	KeCancelTimer(&portContext->TimerReadTotal);
	KeCancelTimer(&portContext->TimerReadInterval);

	if (portContext->ReadIrp != NULL)
	{
		oldReadIrp = portContext->ReadIrp;

		if (IoSetCancelRoutine(oldReadIrp, NULL) != NULL)
		{
			oldReadIrp->IoStatus.Status = STATUS_TIMEOUT;
			portContext->ReadIrp = NULL;
		}
		else
		{
			oldReadIrp = NULL;
		}
	}

	KeReleaseSpinLock(&portContext->SpinLockRead, oldIrql);

	//
	KeSetEvent(&portContext->EvtRxChars, IO_NO_INCREMENT, FALSE);

	if (oldReadIrp != NULL)
	{
		IoCompleteRequest(oldReadIrp, IO_NO_INCREMENT);
	}

	HR_PRINT_METHOD_END();
}

NTSTATUS SetReadTimeout(
	IN PFDOPORT_CONTEXT_EXTENSION PortContext,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	SERIAL_TIMEOUTS timeouts;
	BOOLEAN setTotal;
	ULONG multiplier;
	ULONG constant;

	HR_PRINT_METHOD_BEGIN();

	KeCancelTimer(&PortContext->TimerReadTotal);
	KeCancelTimer(&PortContext->TimerReadInterval);

	timeouts = PortContext->Timeouts;

	if (timeouts.ReadIntervalTimeout == MAXULONG &&
		!timeouts.ReadTotalTimeoutMultiplier &&
		!timeouts.ReadTotalTimeoutConstant)
	{
		return STATUS_SUCCESS;
	}

	setTotal = FALSE;
	multiplier = 0;
	constant = 0;

	if (timeouts.ReadIntervalTimeout == MAXULONG &&
		timeouts.ReadTotalTimeoutMultiplier == MAXULONG &&
		timeouts.ReadTotalTimeoutConstant < MAXULONG &&
		timeouts.ReadTotalTimeoutConstant > 0)
	{
		if (Irp->IoStatus.Information)
		{
			return STATUS_SUCCESS;
		}

		setTotal = TRUE;
		multiplier = 0;
		constant = timeouts.ReadTotalTimeoutConstant;
	}
	else
	{
		if (timeouts.ReadTotalTimeoutMultiplier || timeouts.ReadTotalTimeoutConstant)
		{
			setTotal = TRUE;
			multiplier = timeouts.ReadTotalTimeoutMultiplier;
			constant = timeouts.ReadTotalTimeoutConstant;
		}

		if (timeouts.ReadIntervalTimeout)
		{
			PortContext->TimeoutInterval.QuadPart =
				((LONGLONG)timeouts.ReadIntervalTimeout) * -10000;

			if (Irp->IoStatus.Information)
			{
				KeSetTimer(&PortContext->TimerReadInterval, PortContext->TimeoutInterval, &PortContext->TimerReadIntervalDpc);
			}
		}
	}

	if (setTotal) {
		LARGE_INTEGER total;
		ULONG length;

		length = IoGetCurrentIrpStackLocation(Irp)->Parameters.Read.Length;

		total.QuadPart = ((LONGLONG)(
			UInt32x32To64(length, multiplier) + constant)) * -10000;

		KeSetTimer(
			&PortContext->TimerReadTotal,
			total,
			&PortContext->TimerReadTotalDpc);
	}

	return STATUS_PENDING;
}

NTSTATUS SetWriteTimeout(
	IN PFDOPORT_CONTEXT_EXTENSION PortContext,
	IN PIRP Irp)
{
	SERIAL_TIMEOUTS timeouts;
	BOOLEAN setTotal;
	ULONG multiplier;
	ULONG constant;

	KeCancelTimer(&PortContext->TimerWriteTotal);

	timeouts = PortContext->Timeouts;

	setTotal = FALSE;
	multiplier = 0;
	constant = 0;

	if (timeouts.WriteTotalTimeoutMultiplier || timeouts.WriteTotalTimeoutConstant) {
		setTotal = TRUE;
		multiplier = timeouts.WriteTotalTimeoutMultiplier;
		constant = timeouts.WriteTotalTimeoutConstant;
	}

	if (setTotal) {
		LARGE_INTEGER total;
		ULONG length;

		length = IoGetCurrentIrpStackLocation(Irp)->Parameters.Read.Length;

		total.QuadPart = ((LONGLONG)(UInt32x32To64(length, multiplier) + constant)) * -10000;

		KeSetTimer(
			&PortContext->TimerWriteTotal,
			total,
			&PortContext->TimerWriteTotalDpc);
	}

	return STATUS_PENDING;
}
