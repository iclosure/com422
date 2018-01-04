
#include "precomp.h"
#include "pnppdo.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, EvtDeviceD0EntryPdoPort)
#pragma alloc_text(PAGE, EvtDeviceD0EntryPostInterruptsEnabledPdoPort)
#pragma alloc_text(PAGE, EvtDeviceD0ExitPdoPort)
#pragma alloc_text(PAGE, EvtDeviceD0ExitPreInterruptsDisabledPdoPort)
#pragma alloc_text(PAGE, EvtDevicePrepareHardwarePdoPort)
#pragma alloc_text(PAGE, EvtDeviceReleaseHardwarePdoPort)
#pragma alloc_text(PAGE, EvtDeviceSelfManagedIoCleanupPdoPort)
#pragma alloc_text(PAGE, EvtDeviceSelfManagedIoFlushPdoPort)
#pragma alloc_text(PAGE, EvtDeviceSelfManagedIoInitPdoPort)
#pragma alloc_text(PAGE, EvtDeviceSelfManagedIoSuspendPdoPort)
#pragma alloc_text(PAGE, EvtDeviceSelfManagedIoRestartPdoPort)
#pragma alloc_text(PAGE, EvtDeviceSurpriseRemovalPdoPort)
#pragma alloc_text(PAGE, EvtDeviceQueryRemovePdoPort)
#pragma alloc_text(PAGE, EvtDeviceQueryStopPdoPort)
#pragma alloc_text(PAGE, EvtDeviceUsageNotificationPdoPort)
#pragma alloc_text(PAGE, EvtDeviceRelationsQueryPdoPort)
#endif

NTSTATUS EvtDeviceD0EntryPdoPort(
	IN WDFDEVICE Device,
	IN WDF_POWER_DEVICE_STATE PreviousState)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);

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

NTSTATUS EvtDeviceD0EntryPostInterruptsEnabledPdoPort(
	IN WDFDEVICE Device,
	IN WDF_POWER_DEVICE_STATE PreviousState)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);

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

NTSTATUS EvtDeviceD0ExitPdoPort(
	IN WDFDEVICE Device,
	IN WDF_POWER_DEVICE_STATE TargetState)
{
	NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceD0ExitPreInterruptsDisabledPdoPort(
	IN WDFDEVICE Device,
	IN WDF_POWER_DEVICE_STATE TargetState)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);

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

NTSTATUS EvtDevicePrepareHardwarePdoPort(
	IN WDFDEVICE Device,
	IN WDFCMRESLIST ResourcesRaw,
	IN WDFCMRESLIST ResourcesTranslated)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();
	
	portContext = WdfPdoGetPortContext(Device);
	busContext = WdfBusGetContext(WdfPdoGetParent(Device));

#ifdef DEBUG_WITH_REAL_DEVICE

	//portContext->RegIntMask = busContext->RegIntMask;
	//portContext->RegIntCtrl = busContext->RegIntCtrl;
	
	//portContext->RegIntRStat = (PULONG)(busContext->BAR[COM422_PCI_BAR_SRAM].Addr + XXX_READ_INT_STAT);
	//portContext->RegIntRMask = (PULONG)(busContext->BAR[COM422_PCI_BAR_SRAM].Addr + XXX_READ_INT_MASK);
	portContext->RegIntWStat = (PULONG)(busContext->BAR[COM422_PCI_BAR_SRAM].Addr + XXX_WRITE_INT_STAT);
	//portContext->RegIntWMask = (PULONG)(busContext->BAR[COM422_PCI_BAR_SRAM].Addr + XXX_WRITE_INT_MASK);

	portContext->RegCtrl = COM422_BUS_CTRL_ADDR(busContext, portContext->InstancePort);
	portContext->RegStat = COM422_BUS_STAT_ADDR(busContext, portContext->InstancePort);
	portContext->RegTxRoom = COM422_BUS_TXROOM_ADDR(busContext, portContext->InstancePort);
	portContext->RegRxMaxDep = COM422_BUS_RXMAXDEP_ADDR(busContext, portContext->InstancePort);
	portContext->RegRxCurDep = COM422_BUS_RXCURDEP_ADDR(busContext, portContext->InstancePort);
	portContext->RegTxMaxDep = COM422_BUS_TXMAXDEP_ADDR(busContext, portContext->InstancePort);
	portContext->RegTxCurDep = COM422_BUS_TXCURDEP_ADDR(busContext, portContext->InstancePort);
	portContext->RegTxBuff = (PUCHAR)COM422_BUS_TXBUFF_ADDR_BASE(busContext, portContext->InstancePort);
	portContext->RegRxBuff = (PUCHAR)COM422_BUS_RXBUFF_ADDR_BASE(busContext, portContext->InstancePort);
	
	HrKdPrint(("PPORT >> CTRL: %p, STAT: %p, TXROOM: %p, RXSIZE: %p, TXBUFF: %p\n", 
		portContext->RegCtrl, portContext->RegStat, portContext->RegTxRoom,
		portContext->RegRxSize, portContext->RegTxBuff));

	// Disable transmit.
	//COM422_PPORT_WRITE_INTMASK_BIT(portContext, (portContext->InstancePort + 16), 0UL);

#else

	COM422_PPORT_PRINT_CTRL(portContext);
	COM422_PPORT_PRINT_STAT(portContext);

#endif  // DEBUG_WITH_REAL_DEVICE

	busContext->CircFifo[portContext->InstancePort] = portContext->CircFifo;
	busContext->CircFifo[portContext->InstancePort] = portContext->CircFifo;
	busContext->EvtWriteInt[portContext->InstancePort] = portContext->EvtWriteInt;
	busContext->EvtReadInt[portContext->InstancePort] = portContext->EvtReadInt;
	
	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceReleaseHardwarePdoPort(
	IN WDFDEVICE Device,
	IN WDFCMRESLIST ResourcesTranslated)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);
	busContext = WdfBusGetContext(WdfPdoGetParent(Device));

	busContext->CircFifo[portContext->InstancePort] = NULL;

	HR_PRINT_METHOD_END();

	return status;
}

VOID EvtDeviceSelfManagedIoCleanupPdoPort(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);

	HR_PRINT_METHOD_END();
}

VOID EvtDeviceSelfManagedIoFlushPdoPort(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();
}

NTSTATUS EvtDeviceSelfManagedIoInitPdoPort(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceSelfManagedIoSuspendPdoPort(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceSelfManagedIoRestartPdoPort(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);

	HR_PRINT_METHOD_END();

	return status;
}

VOID EvtDeviceSurpriseRemovalPdoPort(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);

	HR_PRINT_METHOD_END();
}

NTSTATUS EvtDeviceQueryRemovePdoPort(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);

	HR_PRINT_METHOD_END();

	return status;
}

NTSTATUS EvtDeviceQueryStopPdoPort(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);

	HR_PRINT_METHOD_END();

	return status;
}

VOID EvtDeviceUsageNotificationPdoPort(
	IN WDFDEVICE Device,
	IN WDF_SPECIAL_FILE_TYPE NotificationType,
	IN BOOLEAN IsInNotificationPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);

	HR_PRINT_METHOD_END();
}

VOID EvtDeviceRelationsQueryPdoPort(
	IN WDFDEVICE Device,
	IN DEVICE_RELATION_TYPE RelationType)
{
	NTSTATUS status = STATUS_SUCCESS;
	PPDOPORT_CONTEXT_EXTENSION portContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);

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
