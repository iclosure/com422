
#include "precomp.h"
#include "adddev.h"
#include "file.h"
#include "ioctl.h"
#include "pnp.h"
#include "power.h"
#include "timeout.h"
#include "wmi.h"

DRIVER_INITIALIZE     DriverEntry;
DRIVER_UNLOAD		  DriverUnload;
DRIVER_STARTIO		  WriteReadStartIo;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, DriverUnload)
#endif	// ALLOC_PRAGMA

DRIVER_CONTEXT_EXTENSION _v_globalDrvContext;

EXTERN_C NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;

	HR_PRINT_SET_PREFIX("#> (PORT) ");

	HR_PRINT_METHOD_BEGIN();

	HrKdPrint(("Compiled time: %s (%s)\n", __DATE__, __TIME__));

	_v_globalDrvContext.RegistryPath = *RegistryPath;

	DriverObject->DriverExtension->AddDevice = AddDevice;
	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = IrpMajorCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = IrpMajorClose;
	DriverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] = IrpMajorQueryInformation;
	DriverObject->MajorFunction[IRP_MJ_SET_INFORMATION] = IrpMajorSetInformation;
	DriverObject->MajorFunction[IRP_MJ_READ] = IrpMajorRead;
	DriverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS] = IrpMajorWrite;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = IrpMajorWrite;
	DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = IrpMajorInternalIoControl;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IrpMajorDeviceControl;
	DriverObject->MajorFunction[IRP_MJ_CLEANUP] = IrpMajorCleanup;
	DriverObject->MajorFunction[IRP_MJ_PNP] = IrpMajorPnp;
	DriverObject->MajorFunction[IRP_MJ_POWER] = IrpMajorPower;
	DriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = IrpMajorShutDown;
	DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = IrpMajorSystemControl;
	
	HR_PRINT_METHOD_END();
	
	return status;
}

VOID DriverUnload(
	IN PDRIVER_OBJECT DriverObject)
{
	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();
}
