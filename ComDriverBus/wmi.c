
#include "precomp.h"
#include "wmi.h"
#include <wmistr.h>

EVT_WDF_WMI_INSTANCE_QUERY_INSTANCE EvtWmiQueryPortName;
EVT_WDF_WMI_INSTANCE_QUERY_INSTANCE EvtWmiQueryPortCommData;
EVT_WDF_WMI_INSTANCE_QUERY_INSTANCE EvtWmiQueryPortHWData;
EVT_WDF_WMI_INSTANCE_QUERY_INSTANCE EvtWmiQueryPortPerfData;
EVT_WDF_WMI_INSTANCE_QUERY_INSTANCE EvtWmiQueryPortPropData;

NTSTATUS SerialWmiRegisterInstance(
	IN WDFDEVICE Device,
	IN const GUID* Guid,
	IN ULONG MinInstanceBufferSize,
	IN PFN_WDF_WMI_INSTANCE_QUERY_INSTANCE EvtWmiInstanceQueryInstance);

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGESRP0, SerialWmiRegistration)
#pragma alloc_text(PAGESRP0, SerialWmiRegisterInstance)
#pragma alloc_text(PAGESRP0, EvtWmiQueryPortName)
#pragma alloc_text(PAGESRP0, EvtWmiQueryPortCommData)
#pragma alloc_text(PAGESRP0, EvtWmiQueryPortHWData)
#pragma alloc_text(PAGESRP0, EvtWmiQueryPortPerfData)
#pragma alloc_text(PAGESRP0, EvtWmiQueryPortPropData)
#endif

NTSTATUS SerialWmiRegistration(IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);

	// Fill in WMI-PERF data (all zero's)
	RtlZeroMemory(&busContext->WmiPerfData, sizeof(SERIAL_WMI_PERF_DATA));

	status = SerialWmiRegisterInstance(Device, &MSSerial_PortName_GUID, 
		0, EvtWmiQueryPortName);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("SerialWmiRegisterInstance() EvtWmiQueryPortName failure as %08X\n", status));
		return status;
	}

	status = SerialWmiRegisterInstance(Device, &MSSerial_CommInfo_GUID, 
		sizeof(SERIAL_WMI_COMM_DATA), EvtWmiQueryPortCommData);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("SerialWmiRegisterInstance() EvtWmiQueryPortCommData failure as %08X\n", status));
		return status;
	}

	status = SerialWmiRegisterInstance(Device, &MSSerial_HardwareConfiguration_GUID, 
		sizeof(SERIAL_WMI_HW_DATA), EvtWmiQueryPortHWData);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("SerialWmiRegisterInstance() EvtWmiQueryPortHWData failure as %08X\n", status));
		return status;
	}

	status = SerialWmiRegisterInstance(Device, &MSSerial_PerformanceInformation_GUID,
		sizeof(SERIAL_WMI_PERF_DATA), EvtWmiQueryPortPerfData);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("SerialWmiRegisterInstance() EvtWmiQueryPortPerfData failure as %08X\n", status));
		return status;
	}

	status = SerialWmiRegisterInstance(Device, &MSSerial_CommProperties_GUID,
		sizeof(SERIAL_COMMPROP) + sizeof(ULONG), EvtWmiQueryPortPropData);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("SerialWmiRegisterInstance() EvtWmiQueryPortPropData failure as %08X\n", status));
		return status;
	}

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS SerialWmiRegisterInstance(
	IN WDFDEVICE Device,
	IN const GUID* Guid,
	IN ULONG MinInstanceBufferSize,
	IN PFN_WDF_WMI_INSTANCE_QUERY_INSTANCE EvtWmiInstanceQueryInstance)
{
	NTSTATUS status = STATUS_SUCCESS;
    WDF_WMI_PROVIDER_CONFIG providerConfig;
    WDF_WMI_INSTANCE_CONFIG instanceConfig;

    PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

    //
    // Create and register WMI providers and instances  blocks
    //
    WDF_WMI_PROVIDER_CONFIG_INIT(&providerConfig, Guid);
    providerConfig.MinInstanceBufferSize = MinInstanceBufferSize;

    WDF_WMI_INSTANCE_CONFIG_INIT_PROVIDER_CONFIG(&instanceConfig, &providerConfig);
    instanceConfig.Register = TRUE;
    instanceConfig.EvtWmiInstanceQueryInstance = EvtWmiInstanceQueryInstance;
	status = WdfWmiInstanceCreate(Device, &instanceConfig, WDF_NO_OBJECT_ATTRIBUTES, WDF_NO_HANDLE);

	HR_PRINT_METHOD_END();

	return status;
}

//
// WMI Call back functions
//
NTSTATUS EvtWmiQueryPortName(
    IN  WDFWMIINSTANCE WmiInstance,
    IN  ULONG OutBufferSize,
    IN  PVOID OutBuffer,
    OUT PULONG BufferUsed)
{
    WDFDEVICE device;
	WCHAR pRegName[COM422_SYMBOLIC_NAME_LEN];
    UNICODE_STRING string;
    USHORT nameSize = sizeof(pRegName);
    NTSTATUS status;

    PAGED_CODE();

    device = WdfWmiInstanceGetDevice(WmiInstance);
	/*
    status = SerialReadSymName(device, pRegName, &nameSize);
    if (!NT_SUCCESS(status))
	{
        return status;
    }
	*/
    RtlInitUnicodeString(&string, pRegName);

    return WDF_WMI_BUFFER_APPEND_STRING(OutBuffer, OutBufferSize, &string, BufferUsed);
}

NTSTATUS EvtWmiQueryPortCommData(
    IN  WDFWMIINSTANCE WmiInstance,
    IN  ULONG  OutBufferSize,
    IN  PVOID  OutBuffer,
    OUT PULONG BufferUsed)
{
	PBUS_CONTEXT_EXTENSION busContext;

    PAGED_CODE();

	busContext = WdfBusGetContext(WdfWmiInstanceGetDevice(WmiInstance));

    *BufferUsed = sizeof(SERIAL_WMI_COMM_DATA);

    if (OutBufferSize < *BufferUsed)
	{
        return STATUS_INSUFFICIENT_RESOURCES;
    }

	*(PSERIAL_WMI_COMM_DATA)OutBuffer = busContext->WmiCommData;

    return STATUS_SUCCESS;
}

NTSTATUS EvtWmiQueryPortHWData(
    IN  WDFWMIINSTANCE WmiInstance,
    IN  ULONG  OutBufferSize,
    IN  PVOID  OutBuffer,
    OUT PULONG BufferUsed)
{
	PBUS_CONTEXT_EXTENSION busContext;

    UNREFERENCED_PARAMETER(OutBufferSize);

    PAGED_CODE();

	busContext = WdfBusGetContext(WdfWmiInstanceGetDevice(WmiInstance));

    *BufferUsed = sizeof(SERIAL_WMI_HW_DATA);

    if (OutBufferSize < *BufferUsed)
	{
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    
	*(PSERIAL_WMI_HW_DATA)OutBuffer = busContext->WmiHwData;

    return STATUS_SUCCESS;
}

NTSTATUS EvtWmiQueryPortPerfData(
    IN  WDFWMIINSTANCE WmiInstance,
    IN  ULONG OutBufferSize,
    IN  PVOID OutBuffer,
    OUT PULONG BufferUsed)
{
	PBUS_CONTEXT_EXTENSION busContext;

    PAGED_CODE();

	busContext = WdfBusGetContext(WdfWmiInstanceGetDevice(WmiInstance));

    *BufferUsed = sizeof(SERIAL_WMI_PERF_DATA);

    if (OutBufferSize < *BufferUsed)
	{
        return STATUS_INSUFFICIENT_RESOURCES;
    }

	*(PSERIAL_WMI_PERF_DATA)OutBuffer = busContext->WmiPerfData;

    return STATUS_SUCCESS;
}

VOID SerialGetProperties(
	IN PBUS_CONTEXT_EXTENSION BusContext,
	IN PSERIAL_COMMPROP Properties)
{

}

NTSTATUS EvtWmiQueryPortPropData(
    IN  WDFWMIINSTANCE WmiInstance,
    IN  ULONG OutBufferSize,
    IN  PVOID OutBuffer,
    OUT PULONG BufferUsed)
{
	PBUS_CONTEXT_EXTENSION busContext;

    PAGED_CODE();

	busContext = WdfBusGetContext(WdfWmiInstanceGetDevice(WmiInstance));

    *BufferUsed = sizeof(SERIAL_COMMPROP) + sizeof(ULONG);

    if (OutBufferSize < *BufferUsed)
	{
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    SerialGetProperties(busContext, (PSERIAL_COMMPROP)OutBuffer);

    *((PULONG)(((PSERIAL_COMMPROP)OutBuffer)->ProvChar)) = 0;

    return STATUS_SUCCESS;
}
