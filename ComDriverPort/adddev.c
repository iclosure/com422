
#include "precomp.h"
#include "adddev.h"
#include "file.h"
#include "ioctl.h"
#include "pnp.h"
#include "waitmask.h"
#include "wmi.h"
#include <devguid.h>

NTSTATUS SetDeviceSymbolicLink(
	IN PDEVICE_OBJECT DeviceObject,
	IN PDEVICE_OBJECT PhyDevObject);

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, AddDevice)
#pragma alloc_text(PAGE, SetDeviceSymbolicLink)
#endif

NTSTATUS AddDevice(
	IN PDRIVER_OBJECT DriverObject,
	IN PDEVICE_OBJECT PhyDevObject)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION pdoPortContext;
	PFDOPORT_CONTEXT_EXTENSION fdoPortContext;
	PDEVICE_OBJECT newDevice = NULL;
	DECLARE_UNICODE_STRING_SIZE(deviceName, 80);

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	// 
	pdoPortContext = (PPDOPORT_CONTEXT_EXTENSION)PhyDevObject->DeviceExtension;

	HrKdPrint(("InstancePort: %d\n", pdoPortContext->InstancePort));

	// 生成设备名
	status = RtlUnicodeStringPrintf(&deviceName, L"\\Device\\CrdSerB%02dP%03d",
		pdoPortContext->InstanceBus, pdoPortContext->InstancePort);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("RtlUnicodeStringPrintf() DeviceName failed as %08X\n", status));
		return status;
	}

	HrKdPrint(("DeviceName: %wZ\n", &deviceName));

	// 创建设备对象
	status = IoCreateDevice(DriverObject, sizeof(FDOPORT_CONTEXT_EXTENSION),
		&deviceName, FILE_DEVICE_SERIAL_PORT, 0, TRUE, &newDevice);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("IoCreateDevice() failed as %08X\n", status));
		return status;
	}

	// 获取设备扩展对象句柄
	fdoPortContext = newDevice->DeviceExtension;
	RtlZeroMemory(fdoPortContext, sizeof(FDOPORT_CONTEXT_EXTENSION));
	fdoPortContext->TheDevice = newDevice;
	fdoPortContext->PortIndex = pdoPortContext->InstancePort;

	// 保存设备名到扩展设备上下文
	UnicodeStringAppendUnicodeString(&status, &fdoPortContext->DeviceName, &deviceName);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("UnicodeStringAppendUnicodeString() DeviceName failed as %08X\n", status));
		return status;
	}

	// 将FDO挂载在PDO上层，并且将DeviceExtension中的LowDevice指向FDO的下层设备。
	// 如果PDO上面有过滤驱动的话，LowDevice就是过滤驱动，如果没有就是PDO。
	fdoPortContext->LowDevice = IoAttachDeviceToDeviceStack(newDevice, PhyDevObject);

	// 设置符号链接
	status = SetDeviceSymbolicLink(newDevice, PhyDevObject);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("SetDeviceSymbolicLink() failed as %08X\n", status));
		return status;
	}

	// 
	KeInitializeSpinLock(&fdoPortContext->SpinLockInt);
	KeInitializeSpinLock(&fdoPortContext->SpinLockIoctl);
	KeInitializeSpinLock(&fdoPortContext->SpinLockRead);
	KeInitializeSpinLock(&fdoPortContext->SpinLockWrite);

	// 创建WMI系统控制器
	status = WmiSystemControlCreate(newDevice);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WmiSystemControlCreate() failed as %08X\n", status));
		return status;
	}

	// 创建写例程
	status = QueueWriteCreate(newDevice);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("QueueWriteCreate() failed as %08X\n", status));
		return status;
	}

	// 创建读例程
	status = QueueReadCreate(newDevice);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("QueueReadCreate() failed as %08X\n", status));
		return status;
	}

	fdoPortContext->TheDevice->Flags |= DO_BUFFERED_IO | DO_POWER_PAGABLE;	// DO_BUFFERED_IO，定义为“缓冲内存设备”
	fdoPortContext->TheDevice->Flags &= ~(DO_DEVICE_INITIALIZING);			// 保证设备初始化完毕。

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS SetDeviceSymbolicLink(
	IN PDEVICE_OBJECT DeviceObject,
	IN PDEVICE_OBJECT PhyDevObject)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	HANDLE hRegKey;
	PKEY_VALUE_PARTIAL_INFORMATION keyValueInfo;
	ULONG resultLength = 0;
	DECLARE_UNICODE_STRING_SIZE(keyName, 80);
	DECLARE_UNICODE_STRING_SIZE(portName, 80);
	DECLARE_UNICODE_STRING_SIZE(symLinkName, 80);

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	// 打开设备注册表键
	status = IoOpenDeviceRegistryKey(PhyDevObject, PLUGPLAY_REGKEY_DEVICE, STANDARD_RIGHTS_READ, &hRegKey);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("IoOpenDeviceRegistryKey() failed as %08X\n", status));
		return status;
	}

	// Query PortName information
	RtlUnicodeStringInit(&keyName, L"PortName");

	// Query ResultLength
	status = ZwQueryValueKey(hRegKey, &keyName, KeyValuePartialInformation,
		NULL, 0, &resultLength);
	if ((STATUS_OBJECT_NAME_NOT_FOUND == status) || (0 == resultLength))
	{
		HrKdPrint(("registry PortName KeyName not found\n"));
		return status;
	}

	// Allocate memory space
	keyValueInfo = (PKEY_VALUE_PARTIAL_INFORMATION)HR_ALLOCATE_POOL(PagedPool, resultLength);
	if (keyValueInfo == NULL)
	{
		HrKdPrint(("keyValueInfo is null\n"));
		ZwClose(hRegKey);
		return status;
	}

	// Query KeyValue
	status = ZwQueryValueKey(hRegKey, &keyName, KeyValuePartialInformation,
		keyValueInfo, resultLength, &resultLength);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("ZwQueryValueKey() PortName failed as %08X\n", status));
		ZwClose(hRegKey);
		return status;
	}

	// Initialize PortName
	if (keyValueInfo->Type == REG_SZ && keyValueInfo->DataLength < portName.MaximumLength)
	{
		RtlUnicodeStringCopyString(&portName, (PWSTR)keyValueInfo->Data);
		HrKdPrint(("PortName: %wZ\n", &portName));
		HR_FREE_POOL(keyValueInfo);
	}
	else
	{
		ZwClose(hRegKey);
		HR_FREE_POOL(keyValueInfo);
		return status;
	}

	// Initialize symLinkLink
	status = RtlUnicodeStringPrintf(&symLinkName, L"%ws%wZ", COM422_PREF_WIN32_DEVNAME, &portName);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("RtlUnicodeStringPrintf() SymLinkName failed as %08X\n", status));
		return status;
	}

	HrKdPrint(("SymLinkName: %wZ\n", &symLinkName));

	// 创建链接符号
	status = IoCreateSymbolicLink(&symLinkName, &portContext->DeviceName);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("first： IoCreateSymbolicLink() failed as %08X\n", status));

		// 创建失败，删除符号链接
		IoDeleteSymbolicLink(&portContext->SymLinkName);

		// 再次创建符号链接
		status = IoCreateSymbolicLink(&symLinkName, &portContext->DeviceName);
		if (!NT_SUCCESS(status))
		{
			HrKdPrint(("second： IoCreateSymbolicLink() failed as %08X\n", status));
			return status;
		}
	}

	// 添加注册表SERIALCOMM项
	status = RtlWriteRegistryValue(RTL_REGISTRY_DEVICEMAP, COM422_SERIAL_DEVICEMAP,
		portContext->DeviceName.Buffer, REG_SZ, portName.Buffer, portName.Length);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("RtlWriteRegistryValue() PortName failed as %08X\n", status));
		return status;
	}

	// 保存串口名到扩展设备上下文
	UnicodeStringAppendUnicodeString(&status, &portContext->PortName, &portName);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("UnicodeStringAppendUnicodeString() PortName failed as %08X\n", status));
		return status;
	}
	
	// 保存符号链接名到扩展设备上下文
	UnicodeStringAppendUnicodeString(&status, &portContext->SymLinkName, &symLinkName);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("UnicodeStringAppendUnicodeString() SymLinkName failed as %08X\n", status));
		return status;
	}

	HR_PRINT_METHOD_END();

	return status;
}
