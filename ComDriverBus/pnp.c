
#include "precomp.h"
#include "pnp.h"
#include "buspdo.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, EvtDeviceD0Entry)
#pragma alloc_text(PAGE, EvtDeviceD0EntryPostInterruptsEnabled)
#pragma alloc_text(PAGE, EvtDeviceD0Exit)
#pragma alloc_text(PAGE, EvtDeviceD0ExitPreInterruptsDisabled)
#pragma alloc_text(PAGE, EvtDevicePrepareHardware)
#pragma alloc_text(PAGE, EvtDeviceReleaseHardware)
#pragma alloc_text(PAGE, EvtDeviceSelfManagedIoCleanup)
#pragma alloc_text(PAGE, EvtDeviceSelfManagedIoFlush)
#pragma alloc_text(PAGE, EvtDeviceSelfManagedIoInit)
#pragma alloc_text(PAGE, EvtDeviceSelfManagedIoSuspend)
#pragma alloc_text(PAGE, EvtDeviceSelfManagedIoRestart)
#pragma alloc_text(PAGE, EvtDeviceSurpriseRemoval)
#pragma alloc_text(PAGE, EvtDeviceQueryRemove)
#pragma alloc_text(PAGE, EvtDeviceQueryStop)
#pragma alloc_text(PAGE, EvtDeviceUsageNotification)
#pragma alloc_text(PAGE, EvtDeviceRelationsQuery)
#endif

NTSTATUS EvtDeviceD0Entry(
	IN  WDFDEVICE Device,
	IN WDF_POWER_DEVICE_STATE PreviousState)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);

	HrKdPrint(("PreviousState is %s\n",
		(PreviousState == WdfPowerDeviceD0) ? "WdfPowerDeviceD0" :
		(PreviousState == WdfPowerDeviceD1) ? "WdfPowerDeviceD1" :
		(PreviousState == WdfPowerDeviceD2) ? "WdfPowerDeviceD2" :
		(PreviousState == WdfPowerDeviceD3) ? "WdfPowerDeviceD3" :
		(PreviousState == WdfPowerDeviceD3Final) ? "WdfPowerDeviceD3Final" :
		(PreviousState == WdfPowerDevicePrepareForHibernation) ? "WdfPowerDevicePrepareForHibernation" :
		(PreviousState == WdfPowerDeviceMaximum) ? "WdfPowerDeviceMaximum" : "Invalid"));

	if (PowerDeviceD2 == PreviousState)
	{
		//...
	}

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceD0EntryPostInterruptsEnabled(
	IN WDFDEVICE Device,
	IN WDF_POWER_DEVICE_STATE PreviousState)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);

	HrKdPrint(("PreviousState is %s\n",
		(PreviousState == WdfPowerDeviceD0) ? "WdfPowerDeviceD0" :
		(PreviousState == WdfPowerDeviceD1) ? "WdfPowerDeviceD1" :
		(PreviousState == WdfPowerDeviceD2) ? "WdfPowerDeviceD2" :
		(PreviousState == WdfPowerDeviceD3) ? "WdfPowerDeviceD3" :
		(PreviousState == WdfPowerDeviceD3Final) ? "WdfPowerDeviceD3Final" :
		(PreviousState == WdfPowerDevicePrepareForHibernation) ? "WdfPowerDevicePrepareForHibernation" :
		(PreviousState == WdfPowerDeviceMaximum) ? "WdfPowerDeviceMaximum" : "Invalid"));

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceD0Exit(
	IN WDFDEVICE Device,
	IN WDF_POWER_DEVICE_STATE TargetState)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);

	HrKdPrint(("TargetState is %s\n",
		(TargetState == WdfPowerDeviceD0) ? "WdfPowerDeviceD0" :
		(TargetState == WdfPowerDeviceD1) ? "WdfPowerDeviceD1" :
		(TargetState == WdfPowerDeviceD2) ? "WdfPowerDeviceD2" :
		(TargetState == WdfPowerDeviceD3) ? "WdfPowerDeviceD3" :
		(TargetState == WdfPowerDeviceD3Final) ? "WdfPowerDeviceD3Final" :
		(TargetState == WdfPowerDevicePrepareForHibernation) ? "WdfPowerDevicePrepareForHibernation" :
		(TargetState == WdfPowerDeviceMaximum) ? "WdfPowerDeviceMaximum" : "Invalid"));

	if (PowerDeviceD2 == TargetState)
	{
		//...
	}

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceD0ExitPreInterruptsDisabled(
	IN WDFDEVICE Device,
	IN WDF_POWER_DEVICE_STATE TargetState)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HrKdPrint(("TargetState is %s\n",
		(TargetState == WdfPowerDeviceD0) ? "WdfPowerDeviceD0" :
		(TargetState == WdfPowerDeviceD1) ? "WdfPowerDeviceD1" :
		(TargetState == WdfPowerDeviceD2) ? "WdfPowerDeviceD2" :
		(TargetState == WdfPowerDeviceD3) ? "WdfPowerDeviceD3" :
		(TargetState == WdfPowerDeviceD3Final) ? "WdfPowerDeviceD3Final" :
		(TargetState == WdfPowerDevicePrepareForHibernation) ? "WdfPowerDevicePrepareForHibernation" :
		(TargetState == WdfPowerDeviceMaximum) ? "WdfPowerDeviceMaximum" : "Invalid"));

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDevicePrepareHardware(
	IN WDFDEVICE Device,
	IN WDFCMRESLIST ResourcesRaw,
	IN WDFCMRESLIST ResourcesTranslated)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor;
	LONG numDesc;
	LONG i, j = 0;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);

	numDesc = WdfCmResourceListGetCount(ResourcesTranslated);

	HrKdPrint(("numDesc: %ld\n", numDesc));

	for (i = 0; i < numDesc; i++)
	{
		descriptor = WdfCmResourceListGetDescriptor(ResourcesTranslated, i);
		if (NULL == descriptor)
		{
			HrKdPrint(("WdfCmResourceListGetDescriptor() failure\n"));
			return STATUS_DEVICE_CONFIGURATION_ERROR;
		}

		switch (descriptor->Type)
		{
		case CmResourceTypePort:
		{
			HrKdPrint(("I/O Port: (%08I64X) Length: (%ld)\n",
				descriptor->u.Port.Start,
				descriptor->u.Port.Length));
		}
		case CmResourceTypeMemory:
		{
			HrKdPrint(("Memory: (%08I64X) Length: (%ld)\n",
				descriptor->u.Memory.Start,
				descriptor->u.Memory.Length));

			busContext->BAR[j].Addr = (PUCHAR)MmMapIoSpace(descriptor->u.Memory.Start,
				descriptor->u.Memory.Length, MmNonCached);
			if (NULL == busContext->BAR[j].Addr)
			{
				HrKdPrint(("MmMapIoSpace(%08I64X, %08X) Failed\n",
					descriptor->u.Memory.Start, descriptor->u.Memory.Length));
				return STATUS_UNSUCCESSFUL;
			}
			else
			{
				busContext->BAR[j].Length = descriptor->u.Memory.Length;
				HrKdPrint(("BAR%ld -> MAP: [(%08I64X, %08I64X) --> (%08X, %08X)], size: %ld bytes\n", j,
					descriptor->u.Memory.Start.QuadPart, descriptor->u.Memory.Start.QuadPart + descriptor->u.Memory.Length,
					(ULONG)busContext->BAR[j].Addr, ((ULONG)busContext->BAR[j].Addr)
					+ busContext->BAR[j].Length, busContext->BAR[j].Length));
				j++;
			}

			break;
		}
		case CmResourceTypeInterrupt:
		{
			HrKdPrint(("Interrupt level: %08X, Vector: %08X, Affinity: %08X\n",
				descriptor->u.Interrupt.Level,
				descriptor->u.Interrupt.Vector,
				descriptor->u.Interrupt.Affinity));
			break;
		}
		case CmResourceTypeDma:
		{
			HrKdPrint(("DMA Channel: %ld, Port: %ld\n",
				descriptor->u.Dma.Channel,
				descriptor->u.Dma.Port));
			break;
		}
		default:
			HrKdPrint(("Unexpected I/O resource type %d\n", descriptor->Type));
			break;
		}
	}

	//
	// 初始化通道数
	//
	busContext->PortNum = COM422_BUS_READ_CHNUM(busContext);

	HrKdPrint(("Channel number: %ld\n", busContext->PortNum));

	if (busContext->PortNum <= 0 || busContext->PortNum > COM422_MAX_PORT_COUNT)
	{
		HrKdPrint(("Get channel number failed, (channel number: %d)\n", busContext->PortNum));

		for (i = 0; (i < PCI_TYPE0_ADDRESSES) && (busContext->BAR[i].Addr != NULL); i++)
		{
			HrKdPrint(("BAR%ld: MmUnmapIoSpace(%08X, %08X), size: %ld bytes\n", i,
				(ULONG)busContext->BAR[i].Addr, ((ULONG)busContext->BAR[i].Addr)
				+ busContext->BAR[i].Length, busContext->BAR[i].Length));

			MmUnmapIoSpace(busContext->BAR[i].Addr, busContext->BAR[i].Length);
			busContext->BAR[i].Addr = NULL;
			busContext->BAR[i].Length = 0;
		}

		status = STATUS_UNSUCCESSFUL;
	}
	else
	{
		PHYSICAL_ADDRESS maxAddress;

//		busContext->PortMask = 0;
//		busContext->WriteMask = 0;

#ifdef DEBUG_WITH_REAL_DEVICE

		// 
		busContext->RegPci = (PPCI9054_REGS)busContext->BAR[COM422_PCI_BAR_REGS].Addr;
		busContext->RegIntCsr = (PULONG)&busContext->RegPci->Int_Csr;

		busContext->RegDma0Csr = (PUCHAR)&busContext->RegPci->Dma0_Csr;
		busContext->RegDma0Mode = (PULONG)&busContext->RegPci->Dma0_Mode;
		busContext->RegDma0Count = (PULONG)&busContext->RegPci->Dma0_Count;

		// 
		// 初始化DMA
		// 
		busContext->ReadCommonBufferLength = 8192;
		maxAddress.QuadPart = 0xFFFFFFFF;
		busContext->ReadCommonBuffer = (PUSHORT)MmAllocateContiguousMemory(busContext->ReadCommonBufferLength, maxAddress);
		if (busContext->ReadCommonBuffer == NULL)
		{
			HrKdPrint(("busContext->ReadCommonBuffer == NULL\n"));
			return STATUS_INSUFFICIENT_RESOURCES;
		}
		else
		{
			DMA_MODE dmaMode;
			DESC_PTR descPtr;

			//
			// DMA0配置（用作读数据操作）
			//

			// 获取存储映射的物理地址
			busContext->ReadCommonBufferLA = MmGetPhysicalAddress(busContext->ReadCommonBuffer);

			// 配置DMA0模式：BLOCK模式
			dmaMode.all = READ_REGISTER_ULONG(busContext->RegDma0Mode);			// 获取DMA 控制寄存器当前值
			dmaMode.bits.LocalBusDataWidth = 1;									// 字对齐
			dmaMode.bits.SgModeEnable = FALSE;
			dmaMode.bits.DoneIntEnable = TRUE;
			dmaMode.bits.IntToPci = TRUE;
			dmaMode.bits.ClearCountMode = TRUE;
			dmaMode.bits.LocalAddressMode = TRUE;
			WRITE_REGISTER_ULONG(busContext->RegDma0Mode, dmaMode.all);

			// 配置DMA0地址、描述符
			descPtr.bits.LastElement = TRUE;
			descPtr.bits.DirOfTransfer = DESC_PTR_DIRECTION__FROM_DEVICE;
			WRITE_REGISTER_ULONG((PULONG)&busContext->RegPci->Dma0_PCI_Addr, busContext->ReadCommonBufferLA.u.LowPart);	// 配置PCI侧地址
			WRITE_REGISTER_ULONG((PULONG)&busContext->RegPci->Dma0_Local_Addr, COM422_REG_RFIFO);						// 配置LocalBus侧地址
			WRITE_REGISTER_ULONG((PULONG)&busContext->RegPci->Dma0_Desc_Ptr, descPtr.all);								// 配置DMA0描述符

			//
			// DMA1配置（用作写数据操作）
			//
			/*
			// 配置DMA1模式：BLOCK模式
			dmaMode.all = READ_REGISTER_ULONG(busContext->RegDma1Mode);
			dmaMode.bits.LocalBusDataWidth = 0;		// 字节对齐
			WRITE_REGISTER_ULONG(busContext->RegDma1Mode, dmaMode.all);

			// 配置DMA1地址、描述符
			descPtr.bits.DirOfTransfer = DESC_PTR_DIRECTION__TO_DEVICE;
			WRITE_REGISTER_ULONG((PULONG)&busContext->RegPci->Dma1_Desc_Ptr, descPtr.all);	 // 配置DMA1描述符
			*/
		}




		//busContext->RegIntStat = COM422_BUS_INTSTAT_ADDR(busContext);
		//busContext->RegIntMask = COM422_BUS_INTMASK_ADDR(busContext);
		//busContext->RegIntCtrl = COM422_BUS_INTCTRL_ADDR(busContext);
		//busContext->RegRxBuff = COM422_BUS_RXBUFF_ADDR_BASE(busContext, 0);
		
		// Disable the transmit of all channel.
		//COM422_BUS_WRITE_INTMASK(busContext, 0UL);


		
#endif  // DEBUG_WITH_REAL_DEVICE
	}

	// 总线枚举
	status = BusChildrenPlugin(Device);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("BusChildrenPlugin() failed as %08X\n", status));
		return status;
	}

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceReleaseHardware(
	IN WDFDEVICE Device,
	IN WDFCMRESLIST ResourcesTranslated)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	LONG i = 0;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);

	// Cancel all DPC
	WdfDpcCancel(busContext->IntWriteDpc, TRUE);
	WdfDpcCancel(busContext->IntReadDpc, TRUE);

	// 释放PCI相关资源
	for (i = 0; (i < PCI_TYPE0_ADDRESSES) && (busContext->BAR[i].Addr != NULL); i++)
	{
		HrKdPrint(("BAR%ld: MmUnmapIoSpace(%08X, %08X), size: %ld bytes\n", i,
			(ULONG)busContext->BAR[i].Addr, ((ULONG)busContext->BAR[i].Addr)
			+ busContext->BAR[i].Length, busContext->BAR[i].Length));

		MmUnmapIoSpace(busContext->BAR[i].Addr, busContext->BAR[i].Length);
		busContext->BAR[i].Addr = NULL;
		busContext->BAR[i].Length = 0;
	}
	
	// 释放DMA相关资源
	if (busContext->ReadCommonBuffer != NULL)
	{
		MmFreeContiguousMemory(busContext->ReadCommonBuffer);
		busContext->ReadCommonBuffer = NULL;
	}
	HR_PRINT_METHOD_END();

	return status;
}

VOID EvtDeviceSelfManagedIoCleanup(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);

	HR_PRINT_METHOD_END();
}

VOID EvtDeviceSelfManagedIoFlush(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();
}

NTSTATUS EvtDeviceSelfManagedIoInit(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceSelfManagedIoSuspend(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceSelfManagedIoRestart(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}

VOID EvtDeviceSurpriseRemoval(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();
}

NTSTATUS EvtDeviceQueryRemove(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceQueryStop(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}

VOID EvtDeviceUsageNotification(
	IN WDFDEVICE Device,
	IN WDF_SPECIAL_FILE_TYPE NotificationType,
	IN BOOLEAN IsInNotificationPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();
}

VOID EvtDeviceRelationsQuery(
	IN WDFDEVICE Device,
	IN DEVICE_RELATION_TYPE RelationType)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);

	switch (RelationType)
	{
	case BusRelations:
	{
		break;
	}
	case EjectionRelations:
	{
		break;
	}
	case PowerRelations:
	{
		break;
	}
	case RemovalRelations:
	{
		break;
	}
	case TargetDeviceRelation:
	{
		break;
	}
	case SingleBusRelations:
	{
		break;
	}
	case TransportRelations:
	{
		break;
	}
	default:
		break;
	}

	HR_PRINT_METHOD_END();
}
