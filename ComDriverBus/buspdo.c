
#include "precomp.h"
#include "buspdo.h"
#include "pnppdo.h"
#include "filepdo.h"
#include "ioctlpdo.h"
#include <devguid.h>

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, BusChildListInitialze)
#pragma alloc_text(PAGE, BusChildrenPlugin)
#pragma alloc_text(PAGE, BusChildrenUnplug)
#pragma alloc_text(PAGE, BusChildrenEject)
#pragma alloc_text(PAGE, BusPluginDevice)
#pragma alloc_text(PAGE, BusUnplugDevice)
#pragma alloc_text(PAGE, BusEjectDevice)
#pragma alloc_text(PAGE, EvtChildListCreatePdoPort)
#pragma alloc_text(PAGE, CreateChildListPdoPort)
#pragma alloc_text(PAGE, EvtPdoPortContextCleanup)
#endif

NTSTATUS BusChildListInitialze(
	IN PWDFDEVICE_INIT DeviceInit)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDF_CHILD_LIST_CONFIG childListConfig;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	WDF_CHILD_LIST_CONFIG_INIT(&childListConfig, sizeof(PDO_IDENTIFICATION_DESCRIPTION), EvtChildListCreatePdoPort);
	childListConfig.EvtChildListIdentificationDescriptionDuplicate = EvtChildListIdentificationDescriptionDuplicate;
	childListConfig.EvtChildListIdentificationDescriptionCleanup = EvtChildListIdentificationDescriptionCleanup;
	childListConfig.EvtChildListIdentificationDescriptionCompare = EvtChildListIdentificationDescriptionCompare;
	WdfFdoInitSetDefaultChildListConfig(DeviceInit, &childListConfig, WDF_NO_OBJECT_ATTRIBUTES);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS BusChildrenPlugin(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	LONG i;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);

	for (i = 1; i <= busContext->PortNum; i++)
	{
		status = BusPluginDevice(Device, sizeof(CRD422_PORT_HWIDS) / sizeof(WCHAR), CRD422_PORT_HWIDS, i);
	}

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS BusChildrenUnplug(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	LONG i;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);

	// 
	for (i = 1; i <= busContext->PortNum; i++)
	{
		status = BusUnplugDevice(Device, i);
		if (!NT_SUCCESS(status))
		{
			HrKdPrint(("BusUnplugDevice() failed as %08X\n", status));
		}
	}

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS BusChildrenEject(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	LONG i;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);

	// 
	for (i = 1; i <= busContext->PortNum; i++)
	{
		status = BusEjectDevice(Device, i);
		if (!NT_SUCCESS(status))
		{
			HrKdPrint(("BusEjectDevice() failed as %08X\n", status));
		}
	}

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS BusPluginDevice(
	IN WDFDEVICE Device,
	IN size_t CchHardwareIds,
	IN __field_bcount(CchHardwareIds) PWCHAR HardwareIds,
	IN ULONG SerialNo)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	PDO_IDENTIFICATION_DESCRIPTION description;
	WDFCHILDLIST childList;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);
	childList = WdfFdoGetDefaultChildList(Device);

	HrKdPrint(("Enumerating %ws%03d serial port devices\n", HardwareIds, SerialNo - 1));

	WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));
	description.SerialNo = SerialNo;
	description.CchHardwareIds = CchHardwareIds;
	description.HardwareIds = HardwareIds;
	status = WdfChildListAddOrUpdateChildDescriptionAsPresent(childList, &description.Header, NULL);
	if (status == STATUS_OBJECT_NAME_EXISTS)
	{
		//
		// The description is already present in the list, the serial number is
		// not unique, return error.
		//
		status = STATUS_INVALID_PARAMETER;
	}

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS BusUnplugDevice(
	IN WDFDEVICE Device,
	IN ULONG SerialNo)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDFDEVICE childDevice;
	WDFCHILDLIST childList;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	childList = WdfFdoGetDefaultChildList(Device);

	HrKdPrint(("SerialNo: %d\n", SerialNo));

	if (0 == SerialNo)
	{
		WDF_CHILD_LIST_ITERATOR childListIterator;

		WDF_CHILD_LIST_ITERATOR_INIT(&childListIterator, WdfRetrievePresentChildren);

		WdfChildListBeginIteration(childList, &childListIterator);

		do {
			WDF_CHILD_RETRIEVE_INFO childRetrievInfo;
			PDO_IDENTIFICATION_DESCRIPTION description;
			BOOLEAN result;

			//
			// Init the structures.
			//
			WDF_CHILD_RETRIEVE_INFO_INIT(&childRetrievInfo, &description.Header);
			WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));

			//
			// Get the device identification description
			//
			status = WdfChildListRetrieveNextDevice(childList,
				&childListIterator, &childDevice, &childRetrievInfo);
			if (!NT_SUCCESS(status) || status == STATUS_NO_MORE_ENTRIES)
			{
				break;
			}

			//ASSERT(childRetrievInfo.Status == WdfChildListRetrieveDeviceSuccess);

			//
			// Use that description to request an eject.
			//
			result = WdfChildListRequestChildEject(childList, &description.Header);
			if (!result)
			{
				//WDFVERIFY(result);
				HrKdPrint(("WDFVERIFY(FALSE)\n"));
			}
		} while (TRUE);

		WdfChildListEndIteration(childList, &childListIterator);

		if (status == STATUS_NO_MORE_ENTRIES)
		{
			status = STATUS_SUCCESS;
		}
	}
	else
	{
		PDO_IDENTIFICATION_DESCRIPTION description;

		WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));

		description.SerialNo = SerialNo;

		if (WdfChildListRequestChildEject(childList, &description.Header))
		{
			status = STATUS_SUCCESS;
		}
	}

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS BusEjectDevice(
	IN WDFDEVICE Device,
	IN ULONG SerialNo)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDFCHILDLIST childList;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	childList = WdfFdoGetDefaultChildList(Device);

	HrKdPrint(("SerialNo: %d\n", SerialNo));

	if (0 == SerialNo)
	{
		WdfChildListBeginScan(childList);

		// 

		WdfChildListEndScan(childList);
	}
	else
	{
		PDO_IDENTIFICATION_DESCRIPTION description;

		WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));
		description.SerialNo = SerialNo;

		status = WdfChildListUpdateChildDescriptionAsMissing(childList, &description.Header);

		if (status == STATUS_OBJECT_NAME_EXISTS)
		{
			//
			// The description is already present in the list, the serial number is
			// not unique, return error.
			//
			status = STATUS_INVALID_PARAMETER;
		}
	}

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtChildListCreatePdoPort(
	IN WDFCHILDLIST ChildList,
	IN PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER IdentificationDescription,
	IN PWDFDEVICE_INIT ChildInit)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDO_IDENTIFICATION_DESCRIPTION idDesc;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	idDesc = CONTAINING_RECORD(IdentificationDescription, PDO_IDENTIFICATION_DESCRIPTION, Header);

	status = CreateChildListPdoPort(WdfChildListGetDevice(ChildList), ChildInit, idDesc);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("CreateChildListPdoPort() failed as %08X\n", status));
		return status;
	}

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS CreateChildListPdoPort(
	IN WDFDEVICE Device,
	IN PWDFDEVICE_INIT DeviceInit,
	IN PPDO_IDENTIFICATION_DESCRIPTION idDesc)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDRIVER_CONTEXT_EXTENSION driverContext;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	WDFDEVICE newDevice;
	WDF_PNPPOWER_EVENT_CALLBACKS pnpEvtCallbacks;
	WDF_PDO_EVENT_CALLBACKS pdoEvtCallbacks;
	WDF_DEVICE_PNP_CAPABILITIES pnpCaps;
	WDF_DEVICE_POWER_CAPABILITIES powerCaps;
	WDF_OBJECT_ATTRIBUTES attributes;
	WDF_FILEOBJECT_CONFIG fileConfig;
	WDF_IO_QUEUE_CONFIG queueConfig;
	DECLARE_CONST_UNICODE_STRING(deviceLocation, COM422_PORT_DEVLOCAT);
	DECLARE_CONST_UNICODE_STRING(compatId, COM422_PORT_COMPATIBLE_IDS);
	DECLARE_UNICODE_STRING_SIZE(buffer, COM422_MAX_INSTANCE_ID_LEN);

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoBuffered);
	WdfDeviceInitSetExclusive(DeviceInit, TRUE);
	WdfDeviceInitSetDeviceType(DeviceInit, FILE_DEVICE_SERIAL_PORT);
	WdfDeviceInitSetCharacteristics(DeviceInit, FILE_DEVICE_SECURE_OPEN, TRUE);

	////////////////////////////////////////////////////////////////////

	driverContext = WdfDriverGetContext(WdfDeviceGetDriver(Device));

	HrKdPrint(("Instance = %d\n", idDesc->SerialNo));

	// Provide HardwareIDs
	status = RtlUnicodeStringPrintf(&buffer, L"%ws%03d", idDesc->HardwareIds, idDesc->SerialNo - 1);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("RtlUnicodeStringPrintf() HardwareIds failed as %08X\n", status));
		return status;
	}

	HrKdPrint(("HardwareID: %wZ\n", &buffer));

	// NOTE: same string  is used to initialize hardware id too
	status = WdfPdoInitAddHardwareID(DeviceInit, &buffer);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfPdoInitAddHardwareID() failed as %08X\n", status));
		return status;
	}

	// Provide DeviceIDs
	status = RtlUnicodeStringPrintf(&buffer, L"%wsB%02dP%03d", CRD422_PORT_DEVIDS, 
		driverContext->Instance, idDesc->SerialNo - 1);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("RtlUnicodeStringPrintf() DeviceIDs failed as %08X\n", status));
		return status;
	}

	status = WdfPdoInitAssignDeviceID(DeviceInit, &buffer);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfPdoInitAssignDeviceID() failed as %08X\n", status));
		return status;
	}

	// Provide CompatibleIDs
	status = WdfPdoInitAddCompatibleID(DeviceInit, &compatId);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfPdoInitAddCompatibleID() failed as %08X\n", status));
		return status;
	}

	// Provide InstanceId
	status = RtlUnicodeStringPrintf(&buffer, L"%02d", idDesc->SerialNo);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("RtlUnicodeStringPrintf() InstanceId failed as %08X\n", status));
		return status;
	}

	status = WdfPdoInitAssignInstanceID(DeviceInit, &buffer);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfPdoInitAssignInstanceID() failed as %08X\n", status));
		return status;
	}

	//
	// Provide a description about the device. This text is usually read from
	// the device. In the case of USB device, this text comes from the string
	// descriptor. This text is displayed momentarily by the PnP manager while
	// it's looking for a matching INF. If it finds one, it uses the Device
	// Description from the INF file or the friendly name created by
	// coinstallers to display in the device manager. FriendlyName takes
	// precedence over the DeviceDesc from the INF file.
	//
	status = RtlUnicodeStringPrintf(&buffer, L"%ws %d", COM422_PORT_DEVDESCS, idDesc->SerialNo);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("RtlUnicodeStringPrintf() DeviceDesc as %08X\n", status));
		return status;
	}
	//
	// You can call WdfPdoInitAddDeviceText multiple times, adding device
	// text for multiple locales. When the system displays the text, it
	// chooses the text that matches the current locale, if available.
	// Otherwise it will use the string for the default locale.
	// The driver can specify the driver's default locale by calling
	// WdfPdoInitSetDefaultLocale.
	//
	status = WdfPdoInitAddDeviceText(DeviceInit, &buffer, &deviceLocation, 0x0409);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfPdoInitAddDeviceText() DeviceDesc as %08X\n", status));
		return status;
	}

	WdfPdoInitSetDefaultLocale(DeviceInit, 0x0409);

	////////////////////////////////////////////////////////////////////////

	//WdfPdoInitAllowForwardingRequestToParent(DeviceInit);		// 

	////////////////////////////////////////////////////////////////////////

	// 初始化即插即用事件
	WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpEvtCallbacks);
	pnpEvtCallbacks.EvtDeviceD0Entry = EvtDeviceD0EntryPdoPort;
	pnpEvtCallbacks.EvtDeviceD0EntryPostInterruptsEnabled = EvtDeviceD0EntryPostInterruptsEnabledPdoPort;
	pnpEvtCallbacks.EvtDeviceD0Exit = EvtDeviceD0ExitPdoPort;
	pnpEvtCallbacks.EvtDeviceD0ExitPreInterruptsDisabled = EvtDeviceD0ExitPreInterruptsDisabledPdoPort;
	pnpEvtCallbacks.EvtDevicePrepareHardware = EvtDevicePrepareHardwarePdoPort;
	pnpEvtCallbacks.EvtDeviceReleaseHardware = EvtDeviceReleaseHardwarePdoPort;
	pnpEvtCallbacks.EvtDeviceSelfManagedIoCleanup = EvtDeviceSelfManagedIoCleanupPdoPort;
	pnpEvtCallbacks.EvtDeviceSelfManagedIoFlush = EvtDeviceSelfManagedIoFlushPdoPort;
	pnpEvtCallbacks.EvtDeviceSelfManagedIoInit = EvtDeviceSelfManagedIoInitPdoPort;
	pnpEvtCallbacks.EvtDeviceSelfManagedIoSuspend = EvtDeviceSelfManagedIoSuspendPdoPort;
	pnpEvtCallbacks.EvtDeviceSelfManagedIoRestart = EvtDeviceSelfManagedIoRestartPdoPort;
	pnpEvtCallbacks.EvtDeviceSurpriseRemoval = EvtDeviceSurpriseRemovalPdoPort;
	pnpEvtCallbacks.EvtDeviceQueryRemove = EvtDeviceQueryRemovePdoPort;
	pnpEvtCallbacks.EvtDeviceQueryStop = EvtDeviceQueryStopPdoPort;
	pnpEvtCallbacks.EvtDeviceUsageNotification = EvtDeviceUsageNotificationPdoPort;
	pnpEvtCallbacks.EvtDeviceRelationsQuery = EvtDeviceRelationsQueryPdoPort;
	WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpEvtCallbacks);

	// PDO EVENT
	WDF_PDO_EVENT_CALLBACKS_INIT(&pdoEvtCallbacks);
	pdoEvtCallbacks.EvtDeviceResourcesQuery = EvtDeviceResourcesQueryPdo;
	pdoEvtCallbacks.EvtDeviceResourceRequirementsQuery = EvtDeviceResourceRequirementsQueryPdo;
	pdoEvtCallbacks.EvtDeviceEject = EvtDeviceEjectPdo;
	pdoEvtCallbacks.EvtDeviceSetLock = EvtDeviceSetLockPdo;
	pdoEvtCallbacks.EvtDeviceEnableWakeAtBus = EvtDeviceEnableWakeAtBusPdo;
	pdoEvtCallbacks.EvtDeviceDisableWakeAtBus = EvtDeviceDisableWakeAtBusPdo;
	WdfPdoInitSetEventCallbacks(DeviceInit, &pdoEvtCallbacks);
	
	// 初始化文件对象事件
	WDF_FILEOBJECT_CONFIG_INIT(&fileConfig, EvtDeviceFileCreatePdoPort, EvtFileClosePdoPort, EvtFileCleanupPdoPort);
	WdfDeviceInitSetFileObjectConfig(DeviceInit, &fileConfig, WDF_NO_OBJECT_ATTRIBUTES);

	// 初始化设备扩展对象
	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, PDOPORT_CONTEXT_EXTENSION);
	attributes.EvtCleanupCallback = EvtPdoPortContextCleanup;

	// 创建设备对象
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
	pnpCaps.EjectSupported = WdfTrue;
	pnpCaps.SurpriseRemovalOK = WdfTrue;
	pnpCaps.Address = idDesc->SerialNo;
	pnpCaps.UINumber = idDesc->SerialNo;
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

	// 获取设备扩展对象句柄
	portContext = WdfPdoGetPortContext(newDevice);
	RtlZeroMemory(portContext, sizeof(PDOPORT_CONTEXT_EXTENSION));

	// 初始化设备扩展对象的参数
	portContext->InstanceBus = driverContext->Instance;
	portContext->InstancePort = idDesc->SerialNo - 1;
	portContext->PortMask = (1UL << portContext->InstancePort);

	// Bus Information
	{
		PNP_BUS_INFORMATION busInfo;
		busInfo.BusTypeGuid = GUID_DEVCLASS_PORTS;
		busInfo.LegacyBusType = PNPBus;
		busInfo.BusNumber = driverContext->Instance;
		WdfDeviceSetBusInformationForChildren(newDevice, &busInfo);
	}

	// 创建IO队列
	status = PdoPortIoQueueCreate(newDevice);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("PdoPortIoQueueCreate() failed as %08X\n", status));
		return status;
	}

	HR_PRINT_METHOD_END();

	return status;
}

VOID EvtPdoPortContextCleanup(
	IN WDFOBJECT Object)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Object);

	HR_PRINT_METHOD_END();
}

VOID EvtChildListScanForChildren(
	IN WDFCHILDLIST ChildList)
{
	NTSTATUS status = STATUS_SUCCESS;

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();
}

VOID EvtChildListIdentificationDescriptionCopy(
	IN WDFCHILDLIST ChildList,
	IN PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER SourceIdentificationDescription,
	IN PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER DestinationIdentificationDescription)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDO_IDENTIFICATION_DESCRIPTION srcIdDesc, destIdDesc;

	HR_PRINT_METHOD_BEGIN();

	srcIdDesc = CONTAINING_RECORD(SourceIdentificationDescription,
		PDO_IDENTIFICATION_DESCRIPTION, Header);
	destIdDesc = CONTAINING_RECORD(DestinationIdentificationDescription,
		PDO_IDENTIFICATION_DESCRIPTION, Header);

	//RtlCopyMemory(destIdDesc, srcIdDesc, sizeof(PDO_IDENTIFICATION_DESCRIPTION));

	HR_PRINT_METHOD_END();
}

NTSTATUS EvtChildListIdentificationDescriptionDuplicate(
	IN WDFCHILDLIST ChildList,
	IN PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER SourceIdentificationDescription,
	IN PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER DestinationIdentificationDescription)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDO_IDENTIFICATION_DESCRIPTION srcIdDesc, destIdDesc;
	size_t safeMultResult;

	HR_PRINT_METHOD_BEGIN();

	srcIdDesc = CONTAINING_RECORD(SourceIdentificationDescription,
		PDO_IDENTIFICATION_DESCRIPTION, Header);
	destIdDesc = CONTAINING_RECORD(DestinationIdentificationDescription,
		PDO_IDENTIFICATION_DESCRIPTION, Header);

	RtlCopyMemory(destIdDesc, srcIdDesc, sizeof(PDO_IDENTIFICATION_DESCRIPTION));

	status = RtlSizeTMult(destIdDesc->CchHardwareIds, sizeof(WCHAR), &safeMultResult);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("RtlSizeTMult() failed as %08X\n", status));
		return status;
	}

	destIdDesc->HardwareIds = (PWCHAR)HR_ALLOCATE_POOL(NonPagedPool, safeMultResult);
	if (destIdDesc->HardwareIds == NULL) {
		HrKdPrint(("HR_ALLOCATE_POOL() failed as %08X\n", STATUS_INSUFFICIENT_RESOURCES));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlCopyMemory(destIdDesc->HardwareIds, srcIdDesc->HardwareIds, destIdDesc->CchHardwareIds * sizeof(WCHAR));

	HR_PRINT_METHOD_END();

	return status;
}

VOID EvtChildListIdentificationDescriptionCleanup(
	IN WDFCHILDLIST ChildList,
	IN PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER IdentificationDescription)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDO_IDENTIFICATION_DESCRIPTION idDesc;

	HR_PRINT_METHOD_BEGIN();

	idDesc = CONTAINING_RECORD(IdentificationDescription, PDO_IDENTIFICATION_DESCRIPTION, Header);

	if (idDesc->HardwareIds != NULL)
	{
		HR_FREE_POOL(idDesc->HardwareIds);
		idDesc->HardwareIds = NULL;
	}

	HR_PRINT_METHOD_END();
}

BOOLEAN EvtChildListIdentificationDescriptionCompare(
	IN WDFCHILDLIST ChildList,
	IN PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER FirstIdentificationDescription,
	IN PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER SecondIdentificationDescription)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDO_IDENTIFICATION_DESCRIPTION lhs, rhs;
	BOOLEAN result;

	HR_PRINT_METHOD_BEGIN();

	lhs = CONTAINING_RECORD(FirstIdentificationDescription,
		PDO_IDENTIFICATION_DESCRIPTION, Header);
	rhs = CONTAINING_RECORD(SecondIdentificationDescription,
		PDO_IDENTIFICATION_DESCRIPTION, Header);

	result = (lhs->SerialNo == rhs->SerialNo) ? TRUE : FALSE;

	HR_PRINT_METHOD_END();

	return result;
}

VOID EvtChildListAddressDescriptionCopy(
	IN WDFCHILDLIST ChildList,
	IN PWDF_CHILD_ADDRESS_DESCRIPTION_HEADER SourceAddressDescription,
	IN PWDF_CHILD_ADDRESS_DESCRIPTION_HEADER DestinationAddressDescription)
{
	NTSTATUS status = STATUS_SUCCESS;

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();
}

NTSTATUS EvtChildListAddressDescriptionDuplicate(
	IN WDFCHILDLIST ChildList,
	IN PWDF_CHILD_ADDRESS_DESCRIPTION_HEADER SourceAddressDescription,
	IN PWDF_CHILD_ADDRESS_DESCRIPTION_HEADER DestinationAddressDescription)
{
	NTSTATUS status = STATUS_SUCCESS;

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}

VOID EvtChildListAddressDescriptionCleanup(
	IN WDFCHILDLIST ChildList,
	IN PWDF_CHILD_ADDRESS_DESCRIPTION_HEADER AddressDescription)
{
	NTSTATUS status = STATUS_SUCCESS;

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();
}

BOOLEAN EvtChildListDeviceReenumerated(
	IN WDFCHILDLIST ChildList,
	IN WDFDEVICE OldDevice,
	IN PWDF_CHILD_ADDRESS_DESCRIPTION_HEADER OldAddressDescription,
	IN PWDF_CHILD_ADDRESS_DESCRIPTION_HEADER NewAddressDescription)
{
	NTSTATUS status = STATUS_SUCCESS;

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return TRUE;
}

// 
// PDO event
// 

NTSTATUS EvtDeviceResourcesQueryPdo(
	IN WDFDEVICE Device,
	IN WDFCMRESLIST Resources)
{
	NTSTATUS status = STATUS_SUCCESS;

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceResourceRequirementsQueryPdo(
	IN WDFDEVICE Device,
	IN WDFIORESREQLIST IoResourceRequirementsList)
{
	NTSTATUS status = STATUS_SUCCESS;

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceEjectPdo(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceSetLockPdo(
	IN WDFDEVICE Device,
	IN BOOLEAN IsLocked)
{
	NTSTATUS status = STATUS_SUCCESS;

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceEnableWakeAtBusPdo(
	IN WDFDEVICE Device,
	IN SYSTEM_POWER_STATE PowerState)
{
	NTSTATUS status = STATUS_SUCCESS;

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}

VOID EvtDeviceDisableWakeAtBusPdo(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();
}
