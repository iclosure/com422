
#include "precomp.h"
#include "addbus.h"
#include "buspdo.h"
#include "pnp.h"
#include "interrupt.h"
#include <InitGuid.h>
#include <devguid.h>

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, EvtDriverDeviceAdd)
#pragma alloc_text(PAGE, EvtFdoBusContextCleanup)
#pragma alloc_text(PAGE, WdfDevicePropertyString)
#pragma alloc_text(PAGE, EvtDeviceFilterAddResourceRequirements)
#pragma alloc_text(PAGE, EvtDeviceFilterRemoveResourceRequirements)
#pragma alloc_text(PAGE, EvtDeviceRemoveAddedResources)
#endif

NTSTATUS EvtDriverDeviceAdd(
	IN WDFDRIVER Driver,
	IN PWDFDEVICE_INIT DeviceInit)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDRIVER_CONTEXT_EXTENSION driverContext;
	PBUS_CONTEXT_EXTENSION busContext;
	WDFDEVICE newDevice;
	WDF_PNPPOWER_EVENT_CALLBACKS pnpEvtCallbacks;
	WDF_FDO_EVENT_CALLBACKS fdoEvtCallbacks;
	WDF_DEVICE_PNP_CAPABILITIES pnpCaps;
	WDF_DEVICE_POWER_CAPABILITIES powerCaps;
	WDF_OBJECT_ATTRIBUTES attributes;
	PNP_BUS_INFORMATION pnpBusInfo;
	DECLARE_CONST_UNICODE_STRING(cHardwareId, COM422_BUS_HWIDS);
	DECLARE_UNICODE_STRING_SIZE(hardwareId, 256);
	DECLARE_UNICODE_STRING_SIZE(deviceName, 80);

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	///////////////////////////////////////////////////////////////////////////
	// 检测总线硬件ID

	status = WdfDevicePropertyString(&hardwareId, DeviceInit, DevicePropertyHardwareID);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfDevicePropertyString() failed as %08X\n", status));
		return status;
	}

	if (!RtlEqualUnicodeString(&cHardwareId, &hardwareId, TRUE))
	{
		HrKdPrint(("Hardware Id is invalid (%wZ)\n", &hardwareId));
		return STATUS_UNSUCCESSFUL;
	}

	HrKdPrint(("CRD422_BUS_HWID: %wZ\n", &hardwareId));

	///////////////////////////////////////////////////////////////////////////
	// 创建总线设备

	driverContext = WdfDriverGetContext(Driver);
	driverContext->Instance++;  // 设备实例计数
	HrKdPrint(("Instance: %d\n", driverContext->Instance));

	WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoBuffered);
	WdfDeviceInitSetExclusive(DeviceInit, TRUE);
	WdfDeviceInitSetDeviceType(DeviceInit, FILE_DEVICE_BUS_EXTENDER);

	// Set device name
	status = RtlUnicodeStringPrintf(&deviceName, L"\\Device\\CrdSerB%02d", driverContext->Instance);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("RtlUnicodeStringPrintf() DeviceName failed as %08X\n", status));
		return status;
	}

	WdfDeviceInitAssignName(DeviceInit, &deviceName);
	HrKdPrint(("DeviceName: %wZ\n", &deviceName));

	// 初始化即插即用事件
	WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpEvtCallbacks);
	pnpEvtCallbacks.EvtDeviceD0Entry = EvtDeviceD0Entry;
	pnpEvtCallbacks.EvtDeviceD0EntryPostInterruptsEnabled = EvtDeviceD0EntryPostInterruptsEnabled;
	pnpEvtCallbacks.EvtDeviceD0Exit = EvtDeviceD0Exit;
	pnpEvtCallbacks.EvtDeviceD0ExitPreInterruptsDisabled = EvtDeviceD0ExitPreInterruptsDisabled;
	pnpEvtCallbacks.EvtDevicePrepareHardware = EvtDevicePrepareHardware;
	pnpEvtCallbacks.EvtDeviceReleaseHardware = EvtDeviceReleaseHardware;
	pnpEvtCallbacks.EvtDeviceSelfManagedIoCleanup = EvtDeviceSelfManagedIoCleanup;
	pnpEvtCallbacks.EvtDeviceSelfManagedIoFlush = EvtDeviceSelfManagedIoFlush;
	pnpEvtCallbacks.EvtDeviceSelfManagedIoInit = EvtDeviceSelfManagedIoInit;
	pnpEvtCallbacks.EvtDeviceSelfManagedIoSuspend = EvtDeviceSelfManagedIoSuspend;
	pnpEvtCallbacks.EvtDeviceSelfManagedIoRestart = EvtDeviceSelfManagedIoRestart;
	pnpEvtCallbacks.EvtDeviceSurpriseRemoval = EvtDeviceSurpriseRemoval;
	pnpEvtCallbacks.EvtDeviceQueryRemove = EvtDeviceQueryRemove;
	pnpEvtCallbacks.EvtDeviceQueryStop = EvtDeviceQueryStop;
	pnpEvtCallbacks.EvtDeviceUsageNotification = EvtDeviceUsageNotification;
	pnpEvtCallbacks.EvtDeviceRelationsQuery = EvtDeviceRelationsQuery;
	WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpEvtCallbacks);


	// FDO EVENT
#if 0
	WDF_FDO_EVENT_CALLBACKS_INIT(&fdoEvtCallbacks);
	fdoEvtCallbacks.EvtDeviceFilterAddResourceRequirements = EvtDeviceFilterAddResourceRequirements;
	fdoEvtCallbacks.EvtDeviceFilterRemoveResourceRequirements = EvtDeviceFilterRemoveResourceRequirements;
	fdoEvtCallbacks.EvtDeviceRemoveAddedResources = EvtDeviceRemoveAddedResources;
	WdfFdoInitSetEventCallbacks(DeviceInit, &fdoEvtCallbacks);
#endif


	// 初始化物理设端口设备
	BusChildListInitialze(DeviceInit);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("BusChildListInitialze() failed as %08X\n", status));
		return status;
	}

	// 创建设备对象
	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, BUS_CONTEXT_EXTENSION);
	attributes.EvtCleanupCallback = EvtFdoBusContextCleanup;
	attributes.SynchronizationScope = WdfSynchronizationScopeDevice;
	status = WdfDeviceCreate(&DeviceInit, &attributes, &newDevice);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfDeviceCreate() failed as %08X\n", status));
		return status;
	}

	//
	// Set some properties for the child device.
	//
	WDF_DEVICE_PNP_CAPABILITIES_INIT(&pnpCaps);
	pnpCaps.Removable = WdfTrue;
	pnpCaps.LockSupported = WdfTrue;
	pnpCaps.EjectSupported = WdfTrue;
	pnpCaps.SurpriseRemovalOK = WdfTrue;
	pnpCaps.Address = driverContext->Instance;
	pnpCaps.UINumber = driverContext->Instance;
	WdfDeviceSetPnpCapabilities(newDevice, &pnpCaps);

	// power
	powerCaps.DeviceD1 = WdfTrue;
	powerCaps.WakeFromD1 = WdfTrue;
	powerCaps.DeviceWake = PowerDeviceD1;
	powerCaps.DeviceState[PowerSystemWorking] = PowerDeviceD1;
	powerCaps.DeviceState[PowerSystemSleeping1] = PowerDeviceD1;
	powerCaps.DeviceState[PowerSystemSleeping2] = PowerDeviceD2;
	powerCaps.DeviceState[PowerSystemSleeping3] = PowerDeviceD2;
	powerCaps.DeviceState[PowerSystemHibernate] = PowerDeviceD3;
	powerCaps.DeviceState[PowerSystemShutdown] = PowerDeviceD3;
	WdfDeviceSetPowerCapabilities(newDevice, &powerCaps);

	// Bus Information
	{
		PNP_BUS_INFORMATION busInfo;
		busInfo.BusTypeGuid = GUID_DEVCLASS_MULTIPORTSERIAL;
		busInfo.LegacyBusType = PCIBus;
		busInfo.BusNumber = driverContext->Instance;
		WdfDeviceSetBusInformationForChildren(newDevice, &busInfo);
	}

	// 获取设备扩展对象句柄
	busContext = WdfBusGetContext(newDevice);
	RtlZeroMemory(busContext, sizeof(BUS_CONTEXT_EXTENSION));

	// 初始化设备扩展对象的参数
	busContext->Instance = driverContext->Instance;

	// 初始化中断模块
	status = InterruptCreate(newDevice);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("InterruptCreate() failed as %08X\n", status));
		return status;
	}

	HR_PRINT_METHOD_END();

	return status;
}

VOID EvtFdoBusContextCleanup(
	IN WDFOBJECT Object)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDRIVER_CONTEXT_EXTENSION driverContext;
	PBUS_CONTEXT_EXTENSION busContext;
	LONG i;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	driverContext = WdfDriverGetContext(WdfGetDriver());
	driverContext->Instance--;

	busContext = WdfBusGetContext(Object);

	// Cancel all DPC
	WdfDpcCancel(busContext->IntWriteDpc, TRUE);
	WdfDpcCancel(busContext->IntReadDpc, TRUE);

	// Delete WorkItem
	//WdfWorkItemFlush(busContext->WorkItem);

	// Unplug all sub devices
	status = BusChildrenUnplug(Object);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("BusChildrenUnplug() failed as %08X\n", status));
	}

	// 删除子设备
	status = BusChildrenEject(Object);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("BusChildrenEject() failed as %08X\n", status));
	}

	// 
	WdfTimerStop(busContext->TimerCounter, TRUE);

	HR_PRINT_METHOD_END();
}

NTSTATUS WdfDevicePropertyString(
	IN OUT PUNICODE_STRING PropertyString,
	IN PWDFDEVICE_INIT DeviceInit,
	IN DEVICE_REGISTRY_PROPERTY Property)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG resultLength = 0;
	PWCHAR buffer;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	status = RtlUnicodeStringValidate(PropertyString);
	if (status == STATUS_INVALID_PARAMETER)
	{
		HrKdPrint(("RtlUnicodeStringValidate() failed as %08X\n", status));
		return status;
	}

	status = WdfFdoInitQueryProperty(DeviceInit, Property, 0, NULL, &resultLength);
	if (status != STATUS_BUFFER_TOO_SMALL || resultLength <= 0)
	{
		HrKdPrint(("status(%08X) != STATUS_BUFFER_TOO_SMALL || resultLength(%d) <= 0\n", status, resultLength));
		return STATUS_BUFFER_TOO_SMALL;
	}

	if (resultLength > PropertyString->MaximumLength)
	{
		HrKdPrint(("resultLength(%d) > PropertyString->MaximumLength(%d)\n", resultLength, PropertyString->MaximumLength));
		return STATUS_BUFFER_TOO_SMALL;
	}

	buffer = (PWCHAR)HR_ALLOCATE_POOL(NonPagedPool, resultLength + sizeof(WCHAR));
	if (buffer == NULL)
	{
		HrKdPrint(("HR_ALLOCATE_POOL() failed\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	status = WdfFdoInitQueryProperty(DeviceInit, Property, resultLength, buffer, &resultLength);
	if (NT_SUCCESS(status))
	{
		status = RtlAppendUnicodeToString(PropertyString, buffer);
	}

	HR_FREE_POOL(buffer);

	HR_PRINT_METHOD_END();

	return status;
}

// FDO event

NTSTATUS EvtDeviceFilterAddResourceRequirements(
	IN WDFDEVICE Device,
	IN WDFIORESREQLIST IoResourceRequirementsList)
{
	NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceFilterRemoveResourceRequirements(
	IN WDFDEVICE Device,
	IN WDFIORESREQLIST IoResourceRequirementsList)
{
	NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceRemoveAddedResources(
	IN WDFDEVICE Device,
	IN WDFCMRESLIST ResourcesRaw,
	IN WDFCMRESLIST ResourcesTranslated)
{
	NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}
