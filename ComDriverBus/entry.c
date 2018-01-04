
#include "precomp.h"
#include "addbus.h"

DRIVER_INITIALIZE		       DriverEntry;
EVT_WDF_DRIVER_UNLOAD	       EvtDriverUnload;
EVT_WDF_OBJECT_CONTEXT_CLEANUP EvtDriverContextCleanup;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, EvtDriverUnload)
#pragma alloc_text(PAGE, EvtDriverContextCleanup)
#endif	// ALLOC_PRAGMA

EXTERN_C NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDRIVER_CONTEXT_EXTENSION driverContext;
	WDFDRIVER driver;
	WDF_DRIVER_CONFIG driverConfig;
	WDF_OBJECT_ATTRIBUTES driverAttributes;

	HR_PRINT_SET_PREFIX("#> (BUS) ");

	HR_PRINT_METHOD_BEGIN();

	HrKdPrint(("Compiled time: %s (%s)\n", __DATE__, __TIME__));

	// ��ʼ��������չ����
	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&driverAttributes, DRIVER_CONTEXT_EXTENSION);
	driverAttributes.EvtCleanupCallback = EvtDriverContextCleanup;

	// ��ʼ�������¼�
	WDF_DRIVER_CONFIG_INIT(&driverConfig, EvtDriverDeviceAdd);
	driverConfig.EvtDriverUnload = EvtDriverUnload;
	driverConfig.DriverPoolTag = HR_MEM_TAG;

	// ������������
	status = WdfDriverCreate(DriverObject, RegistryPath, &driverAttributes, &driverConfig, &driver);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfDriverCreate() failed with status %08X\n", status));
		return status;
	}

	// ��ʼ��������չ��������
	driverContext = WdfDriverGetContext(driver);
	driverContext->Instance = 0;	// 
	driverContext->RegistryPath = *RegistryPath;

	HR_PRINT_METHOD_END();

	return status;
}

VOID EvtDriverUnload(
	IN WDFDRIVER Driver)
{
	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();
}

VOID EvtDriverContextCleanup(
	IN WDFOBJECT Object)
{
	PDRIVER_CONTEXT_EXTENSION driverContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	driverContext = WdfDriverGetContext(Object);

	HR_PRINT_METHOD_END();
}
