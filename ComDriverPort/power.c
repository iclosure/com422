
#include "precomp.h"
#include "power.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, IrpMajorPower)
#pragma alloc_text(PAGE, IrpWaitWake)
#pragma alloc_text(PAGE, IrpPowerSequence)
#pragma alloc_text(PAGE, IrpSetPower)
#pragma alloc_text(PAGE, IrpQueryPower)
#pragma alloc_text(PAGE, IrpPowerDefault)
#endif

NTSTATUS IrpMajorPower(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	switch (IoGetCurrentIrpStackLocation(Irp)->MinorFunction)
	{
	case IRP_MN_WAIT_WAKE:
	{
		status = IrpWaitWake(DeviceObject, Irp);
		break;
	}
	case IRP_MN_POWER_SEQUENCE:
	{
		status = IrpPowerSequence(DeviceObject, Irp);
		break;
	}
	case IRP_MN_SET_POWER:
	{
		status = IrpSetPower(DeviceObject, Irp);
		break;
	}
	case IRP_MN_QUERY_POWER:
	{
		status = IrpQueryPower(DeviceObject, Irp);
		break;
	}
	default:
		status = STATUS_NOT_SUPPORTED;
		break;
	}

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpPowerDefault(
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

NTSTATUS IrpWaitWake(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpPowerSequence(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpSetPower(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpQueryPower(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}
