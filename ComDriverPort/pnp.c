
#include "precomp.h"
#include "pnp.h"
#include <InitGuid.h>
#include <devguid.h>

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, IrpMajorPnp)
#pragma alloc_text(PAGE, IrpMajorShutDown)
#pragma alloc_text(PAGE, IrpPnpDefault)
#pragma alloc_text(PAGE, IrpPnpStartDevice)
#pragma alloc_text(PAGE, IrpPnpQueryRemoveDrivice)
#pragma alloc_text(PAGE, IrpPnpRemoveDevice)
#pragma alloc_text(PAGE, IrpPnpCancelRemoveDevice)
#pragma alloc_text(PAGE, IrpPnpStopDevice)
#pragma alloc_text(PAGE, IrpPnpQueryStopDevice)
#pragma alloc_text(PAGE, IrpPnpCancelStopDevice)
#pragma alloc_text(PAGE, IrpPnpQueryDeviceRelations)
#pragma alloc_text(PAGE, IrpPnpQueryInterface)
#pragma alloc_text(PAGE, IrpPnpQueryCapabilities)
#pragma alloc_text(PAGE, IrpPnpQueryResources)
#pragma alloc_text(PAGE, IrpPnpQueryResourceRequirements)
#pragma alloc_text(PAGE, IrpPnpQueryDeviceText)
#pragma alloc_text(PAGE, IrpPnpFilterResourceRequirements)
#pragma alloc_text(PAGE, IrpPnpReadConfig)
#pragma alloc_text(PAGE, IrpPnpWriteConfig)
#pragma alloc_text(PAGE, IrpPnpEject)
#pragma alloc_text(PAGE, IrpPnpSetLock)
#pragma alloc_text(PAGE, IrpPnpQueryId)
#pragma alloc_text(PAGE, IrpPnpQueryPnpDeviceState)
#pragma alloc_text(PAGE, IrpPnpQueryBusInformation)
#pragma alloc_text(PAGE, IrpPnpDeviceUsageNotification)
#pragma alloc_text(PAGE, IrpPnpSupriseRemoval)
#pragma alloc_text(PAGE, IoRequestCompleteRoutine)
#pragma alloc_text(PAGE, ForwardAndWait)
#endif

NTSTATUS IrpMajorPnp(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	ULONG fnIrpMinorIndex;

	// PNP minor functions.
	static PDRIVER_DISPATCH fnIrpMinorTab[] =
	{
		IrpPnpStartDevice,	// IRP_MN_START_DEVICE                 0x00
		IrpPnpQueryRemoveDrivice,		// IRP_MN_QUERY_REMOVE_DEVICE          0x01
		IrpPnpRemoveDevice,	// IRP_MN_REMOVE_DEVICE                0x02
		IrpPnpCancelRemoveDevice,		// IRP_MN_CANCEL_REMOVE_DEVICE         0x03
		IrpPnpStopDevice,//IrpPnpStopDevice,	// IRP_MN_STOP_DEVICE                  0x04
		IrpPnpQueryStopDevice,		// IRP_MN_QUERY_STOP_DEVICE            0x05
		IrpPnpCancelStopDevice,		// IRP_MN_CANCEL_STOP_DEVICE           0x06
		IrpPnpQueryDeviceRelations,		// IRP_MN_QUERY_DEVICE_RELATIONS       0x07
		IrpPnpQueryInterface,		// IRP_MN_QUERY_INTERFACE              0x08
		IrpPnpQueryCapabilities,		// IRP_MN_QUERY_CAPABILITIES           0x09
		IrpPnpQueryResources,		// IRP_MN_QUERY_RESOURCES              0x0A
		IrpPnpQueryResourceRequirements,		// IRP_MN_QUERY_RESOURCE_REQUIREMENTS  0x0B
		IrpPnpQueryDeviceText,		// IRP_MN_QUERY_DEVICE_TEXT            0x0C
		IrpPnpFilterResourceRequirements,		// IRP_MN_FILTER_RESOURCE_REQUIREMENTS 0x0D
		IrpPnpDefault,		// --                                  0x0E
		IrpPnpReadConfig,		// IRP_MN_READ_CONFIG                  0x0F
		IrpPnpWriteConfig,		// IRP_MN_WRITE_CONFIG                 0x10
		IrpPnpEject,		// IRP_MN_EJECT                        0x11
		IrpPnpSetLock,		// IRP_MN_SET_LOCK                     0x12
		IrpPnpQueryId,		// IRP_MN_QUERY_ID                     0x13
		IrpPnpQueryPnpDeviceState,		// IRP_MN_QUERY_PNP_DEVICE_STATE       0x14
		IrpPnpQueryBusInformation,		// IRP_MN_QUERY_BUS_INFORMATION        0x15
		IrpPnpDeviceUsageNotification,		// IRP_MN_DEVICE_USAGE_NOTIFICATION    0x16
		IrpPnpSupriseRemoval		// IRP_MN_SURPRISE_REMOVAL             0x17
	};

#if DBG
	static PCCHAR fnIrpMinorTabName[] =
	{
		"IRP_MN_START_DEVICE",
		"IRP_MN_QUERY_REMOVE_DEVICE",
		"IRP_MN_REMOVE_DEVICE",
		"IRP_MN_CANCEL_REMOVE_DEVICE",
		"IRP_MN_STOP_DEVICE",
		"IRP_MN_QUERY_STOP_DEVICE",
		"IRP_MN_CANCEL_STOP_DEVICE",
		"IRP_MN_QUERY_DEVICE_RELATIONS",
		"IRP_MN_QUERY_INTERFACE",
		"IRP_MN_QUERY_CAPABILITIES",
		"IRP_MN_QUERY_RESOURCES",
		"IRP_MN_QUERY_RESOURCE_REQUIREMENTS",
		"IRP_MN_QUERY_DEVICE_TEXT",
		"IRP_MN_FILTER_RESOURCE_REQUIREMENTS",
		"unused",
		"IRP_MN_READ_CONFIG",
		"IRP_MN_WRITE_CONFIG",
		"IRP_MN_EJECT",
		"IRP_MN_SET_LOCK",
		"IRP_MN_QUERY_ID",
		"IRP_MN_QUERY_PNP_DEVICE_STATE",
		"IRP_MN_QUERY_BUS_INFORMATION",
		"IRP_MN_DEVICE_USAGE_NOTIFICATION",
		"IRP_MN_SURPRISE_REMOVAL",
	};
#endif

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	fnIrpMinorIndex = IoGetCurrentIrpStackLocation(Irp)->MinorFunction;
	if (fnIrpMinorIndex >= sizeof(fnIrpMinorTab) / sizeof(fnIrpMinorTab[0]))
	{
		HrKdPrint(("Unkown MinorFunction (%d). Call IrpPnpDefault\n", fnIrpMinorIndex));

		// 未知子功能代码
		return IrpPnpDefault(DeviceObject, Irp);
	}

#if DBG
	HrKdPrint(("PNP Request (%s)\n", fnIrpMinorTabName[fnIrpMinorIndex]));
#endif  // DBG

	status = (*fnIrpMinorTab[fnIrpMinorIndex])(DeviceObject, Irp);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpMajorShutDown(
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

NTSTATUS IrpPnpDefault(
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

NTSTATUS IrpPnpStartDevice(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION pdoPortContext;
	PFDOPORT_CONTEXT_EXTENSION fdoPortContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	fdoPortContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;
	pdoPortContext = (PPDOPORT_CONTEXT_EXTENSION)fdoPortContext->LowDevice->DeviceExtension;

//	CircFifoCreate(&fdoPortContext->SendFifo, COM422_SEND_FIFO_SIZE, sizeof(UCHAR));
	status = CircFifoCreate(&fdoPortContext->CircFifo, COM422_BUFF_MAX_COUNT, sizeof(UCHAR));
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("CircFifoCreate() failed as %08X\n", status));
		Irp->IoStatus.Status = status;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	}

	// 初始化虚拟物理端口设备上下文
	pdoPortContext->CircFifo = &fdoPortContext->CircFifo;
	pdoPortContext->EvtWriteInt = &fdoPortContext->EvtWriteInt;
	pdoPortContext->EvtReadInt = &fdoPortContext->EvtReadInt;
//	pdoPortContext->SendFifo = &fdoPortContext->SendFifo;
	

	// 转发IRP并等待返回
	status = ForwardAndWait(DeviceObject, Irp);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("ForwardAndWait() failed as %d\n", status));
		Irp->IoStatus.Status = status;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	}

	// 完成IRP
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpPnpQueryRemoveDrivice(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	if (portContext->IsOpened)
	{
		return STATUS_UNSUCCESSFUL;
	}

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpPnpRemoveDevice(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	// 对缺省PNP IRP进行处理
	Irp->IoStatus.Status = STATUS_SUCCESS;
	status = IrpPnpDefault(DeviceObject, Irp);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("IrpPnpDefault() failed as %d\n", status));
		return status;
	}

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	HrKdPrint(("DeviceName: %wZ, SymbolicLink: %wZ\n", &portContext->DeviceName, &portContext->SymLinkName));

	// 删除符号链接
	status = IoDeleteSymbolicLink(&portContext->SymLinkName);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("IoDeleteSymbolicLink() failed as %d\n", status));
	}

	// 脱离设备栈
	if (portContext->LowDevice != NULL)
	{
		IoDetachDevice(portContext->LowDevice);
	}

	// 删除功能设备对象
	IoDeleteDevice(portContext->TheDevice);

	///////////////////////////////////////////////////////////////////////////

	// 清理注册表
	status = RtlDeleteRegistryValue(RTL_REGISTRY_DEVICEMAP, COM422_SERIAL_DEVICEMAP,
		portContext->DeviceName.Buffer);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("RtlDeleteRegistryValue() DeviceName failed as %08X\n", status));
		return status;
	}

	///////////////////////////////////////////////////////////////////////////

	// 清除字符串
	RtlFreeUnicodeString(&portContext->PortName);
	RtlFreeUnicodeString(&portContext->DeviceName);
	RtlFreeUnicodeString(&portContext->SymLinkName);

	///////////////////////////////////////////////////////////////////////////

	HR_PRINT_METHOD_END();

	return STATUS_SUCCESS;
}

NTSTATUS IrpPnpCancelRemoveDevice(
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

NTSTATUS IrpPnpStopDevice(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION fportContext;
	PPDOPORT_CONTEXT_EXTENSION pportContext;
	ULONG regWMask;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	fportContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;
	pportContext = (PPDOPORT_CONTEXT_EXTENSION)fportContext->LowDevice->DeviceExtension;

	//////////////////////////////////////////////////////////////////////////

	// Stop WaitMask Timer
	KeCancelTimer(&fportContext->WaitMaskTimer);

	// Complete write-Irp
	if (fportContext->WriteIrp != NULL)
	{
		// 屏蔽写端口中断
		WRITE_REGISTER_ULONG(pportContext->RegCtrl, READ_REGISTER_ULONG(pportContext->RegCtrl) & ~XXX_WRITE_MASK_BIT);

		fportContext->WriteIrp->IoStatus.Information = 0;
		fportContext->WriteIrp->IoStatus.Status = status;
		IoCompleteRequest(fportContext->WriteIrp, IO_NO_INCREMENT);

		fportContext->WriteIrp = NULL;
		KeSetEvent(&fportContext->EvtWriteInt, IO_NO_INCREMENT, FALSE);
	}

	// Complete Read-Irp
	if (fportContext->ReadIrp != NULL)
	{
		fportContext->ReadIrp->IoStatus.Information = 0;
		fportContext->ReadIrp->IoStatus.Status = status;
		IoCompleteRequest(fportContext->ReadIrp, IO_NO_INCREMENT);

		fportContext->ReadIrp = NULL;
		KeSetEvent(&fportContext->EvtRxChars, IO_NO_INCREMENT, FALSE);
	}

	// Complete WaitMask Irp
	if (fportContext->WaitMaskIrp != NULL)
	{
		fportContext->WaitMaskIrp->IoStatus.Information = 0;
		fportContext->WaitMaskIrp->IoStatus.Status = status;
		IoCompleteRequest(fportContext->WaitMaskIrp, IO_NO_INCREMENT);

		fportContext->WaitMaskIrp = NULL;
		KeSetEvent(&fportContext->EvtRxChars, IO_NO_INCREMENT, FALSE);
	}

	//////////////////////////////////////////////////////////////////////////

	// 转发IRP并等待返回
	status = ForwardAndWait(DeviceObject, Irp);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("ForwardAndWait() failed as %d\n", status));

		Irp->IoStatus.Status = status;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	}

	// 释放FIFO
	CircFifoFree(&fportContext->CircFifo);

	// 完成IRP
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return STATUS_SUCCESS;
}

NTSTATUS IrpPnpQueryStopDevice(
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

NTSTATUS IrpPnpCancelStopDevice(
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

NTSTATUS IrpPnpQueryDeviceRelations(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	switch (IoGetCurrentIrpStackLocation(Irp)->Parameters.QueryDeviceRelations.Type)
	{
	case BusRelations:
	{
		status = Irp->IoStatus.Status;
		break;
	}
	case EjectionRelations:
	{
		status = Irp->IoStatus.Status;
		break;
	}
	case PowerRelations:
	{
		status = Irp->IoStatus.Status;
		break;
	}
	case RemovalRelations:
	{
		status = STATUS_SUCCESS;
		//Irp->IoStatus.Information = 1;
		break;
	}
	case TargetDeviceRelation:
	{
		status = Irp->IoStatus.Status;
		break;
	}
	case SingleBusRelations:
	{
		status = Irp->IoStatus.Status;
		break;
	}
	case TransportRelations:
	{
		status = Irp->IoStatus.Status;
		break;
	}
	default:
		status = Irp->IoStatus.Status;
		break;
	}

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpPnpQueryInterface(
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

NTSTATUS IrpPnpQueryCapabilities(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	PDEVICE_CAPABILITIES pCaps;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	pCaps = IoGetCurrentIrpStackLocation(Irp)->Parameters.DeviceCapabilities.Capabilities;

	if (pCaps->Version != 1 || pCaps->Size < sizeof(DEVICE_CAPABILITIES))
	{
		return STATUS_UNSUCCESSFUL;
	}

	pCaps->UniqueID = TRUE;

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpPnpQueryResources(
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

NTSTATUS IrpPnpQueryResourceRequirements(
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

NTSTATUS IrpPnpQueryDeviceText(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	switch (IoGetCurrentIrpStackLocation(Irp)->Parameters.QueryDeviceText.DeviceTextType)
	{
	case DeviceTextDescription:
	{
		if (!Irp->IoStatus.Information)
		{
			UNICODE_STRING portText;

			RtlInitUnicodeString(&portText, NULL);

			UnicodeStringAppendString0(&status, &portText, portContext->PortName.Buffer);

			if (NT_SUCCESS(status))
			{
				Irp->IoStatus.Information = (ULONG_PTR)portText.Buffer;
			}
		}

		break;
	}
	case DeviceTextLocationInformation:
	{
		if (!Irp->IoStatus.Information)
		{
			UNICODE_STRING portText;

			RtlInitUnicodeString(&portText, NULL);

			UnicodeStringAppendString0(&status, &portText, portContext->PortName.Buffer);

			if (NT_SUCCESS(status))
			{
				Irp->IoStatus.Information = (ULONG_PTR)portText.Buffer;
			}
		}

		break;
	}
	default:
		break;
	}

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpPnpFilterResourceRequirements(
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

NTSTATUS IrpPnpReadConfig(
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

NTSTATUS IrpPnpWriteConfig(
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

NTSTATUS IrpPnpEject(
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

NTSTATUS IrpPnpSetLock(
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

NTSTATUS IrpPnpQueryId(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	PWCHAR pIDs = NULL;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	switch (IoGetCurrentIrpStackLocation(Irp)->Parameters.QueryId.IdType)
	{
	case BusQueryDeviceID:
	{
		status = UnicodeStringDupString(&pIDs, CRD422_PORT_DEVIDS, PagedPool, FALSE);
		break;
	}
	case BusQueryHardwareIDs:
	{
		status = UnicodeStringDupString(&pIDs, CRD422_PORT_HWIDS, PagedPool, FALSE);
		break;
	}
	case BusQueryCompatibleIDs:
	{
		status = UnicodeStringDupString(&pIDs, COM422_PORT_COMPATIBLE_IDS, PagedPool, TRUE);
		break;
	}
	case BusQueryInstanceID:
	{
		status = UnicodeStringDupString(&pIDs, portContext->PortName.Buffer, PagedPool, FALSE);
		break;
	}
	case BusQueryDeviceSerialNumber:
	{
		status = Irp->IoStatus.Status;
		break;
	}
	case BusQueryContainerID:
	{
		status = Irp->IoStatus.Status;
		break;
	}
	default:
		status = Irp->IoStatus.Status;
		break;
	}

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpPnpQueryPnpDeviceState(
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

NTSTATUS IrpPnpQueryBusInformation(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	PPNP_BUS_INFORMATION busInfo;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	busInfo = (PPNP_BUS_INFORMATION)HR_ALLOCATE_POOL(PagedPool, sizeof(PNP_BUS_INFORMATION));

	if (!busInfo)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	busInfo->BusTypeGuid = GUID_DEVCLASS_PORTS;
	busInfo->LegacyBusType = PNPBus;
	busInfo->BusNumber = portContext->PortIndex;

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS IrpPnpDeviceUsageNotification(
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

NTSTATUS IrpPnpSupriseRemoval(
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

NTSTATUS IoRequestCompleteRoutine(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp,
	IN PKEVENT KEvent)
{
	NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	// 在完成例程中设置等待事件
	KeSetEvent(KEvent, 0, FALSE);

	// 标志本IRP还需要再次被完成
	status = STATUS_MORE_PROCESSING_REQUIRED;

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS ForwardAndWait(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFDOPORT_CONTEXT_EXTENSION portContext;
	KEVENT event;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = (PFDOPORT_CONTEXT_EXTENSION)DeviceObject->DeviceExtension;

	// 初始化事件
	KeInitializeEvent(&event, NotificationEvent, FALSE);

	// 将本层堆栈拷贝到下一层堆栈
	IoCopyCurrentIrpStackLocationToNext(Irp);

	// 设置完成例程
	IoSetCompletionRoutine(Irp, (PIO_COMPLETION_ROUTINE)IoRequestCompleteRoutine,
		(PVOID)&event, TRUE, TRUE, TRUE);

	// 调用底层驱动，即PDO
	status = IoCallDriver(portContext->LowDevice, Irp);
	if (!NT_SUCCESS(status))
	{
		Irp->IoStatus.Status = status;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return Irp->IoStatus.Status;
	}
	else if (STATUS_PENDING == status)
	{
		// 等待PDO完成
		KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
	}

	HR_PRINT_METHOD_END();

	return Irp->IoStatus.Status;
}
