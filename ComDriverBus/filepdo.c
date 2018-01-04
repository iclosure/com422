
#include "precomp.h"
#include "filepdo.h"
#include "ioctlpdo.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, EvtDeviceFileCreatePdoPort)
#pragma alloc_text(PAGE, EvtFileClosePdoPort)
#pragma alloc_text(PAGE, EvtFileCleanupPdoPort)
#endif

VOID EvtDeviceFileCreatePdoPort(
	IN WDFDEVICE Device,
	IN WDFREQUEST Request,
	IN WDFFILEOBJECT FileObject)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	COM422_INTERRUPT_SYNC_CONTEXT syncContext;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	portContext = WdfPdoGetPortContext(Device);
	busContext = WdfBusGetContext(WdfPdoGetParent(Device));
	syncContext.Context = portContext;

	//////////////////////////////////////////////////////////////////////////
	
	portContext->PortIoInfo.BaudRate.BaudRate = 9600;
	portContext->PortIoInfo.LineCtrl.WordLength = 8;
	portContext->PortIoInfo.LineCtrl.Parity = NO_PARITY;
	portContext->PortIoInfo.LineCtrl.StopBits = STOP_BIT_1;
	portContext->PortIoInfo.ModemStatus = COM422_CTRL_MODEMSTATUS_MASK;

	syncContext.Data = &portContext->PortIoInfo.BaudRate;
	if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialSetBaudRate, &syncContext))
	{
		HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() EvtSerialSetBaudRate failed\n"));
		return;
	}
	syncContext.Data = &portContext->PortIoInfo.LineCtrl;
	if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialSetLineControl, &syncContext))
	{
		HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() EvtSerialSetLineControl failed\n"));
		return;
	}
	syncContext.Data = &portContext->PortIoInfo.ModemStatus;
	if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialSetModemStatus, &syncContext))
	{
		HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() EvtSerialSetModemStatus failed\n"));
		return;
	}
	syncContext.Data = busContext;
	if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialBusCreate, &syncContext))
	{
		HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() EvtSerialBusCreate failed\n"));
		return;
	}
	
	WdfRequestComplete(Request, status);

	HR_PRINT_METHOD_END();
}

VOID EvtFileClosePdoPort(
	IN WDFFILEOBJECT FileObject)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	COM422_INTERRUPT_SYNC_CONTEXT syncContext;
	WDFDEVICE device;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	device = WdfFileObjectGetDevice(FileObject);
	portContext = WdfPdoGetPortContext(device);
	busContext = WdfBusGetContext(WdfPdoGetParent(device));
	syncContext.Context = portContext;
	
	syncContext.Data = busContext;
	if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtSerialBusClose, &syncContext))
	{
		HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() EvtSerialBusClose failed\n"));
		return;
	}
	
	HR_PRINT_METHOD_END();
}

VOID EvtFileCleanupPdoPort(
	IN WDFFILEOBJECT FileObject)
{
	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	HR_PRINT_METHOD_END();
}

BOOLEAN EvtSerialBusCreate(
	IN WDFINTERRUPT Interrupt,
	IN WDFCONTEXT Context)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	ULONG baudRate;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PPDOPORT_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Context;
	busContext = (PBUS_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Data;

//	HrKdPrint(("PortMask: %08X, WriteMask: %08X, IntMask: %08X\n", busContext->PortMask, busContext->WriteMask, busContext->IntMask));

	// TODO: 禁止端口发送中断
	//SET_REG_BIT(busContext->WriteMask, portContext->InstancePort);
	//HrKdPrint(("Before -> RegIntMask: ptr(%p), val(%08X)\n", portContext->RegIntMask,
	//	COM422_PPORT_READ_INTMASK(portContext)));
	//COM422_PPORT_WRITE_INTMASK_BIT(portContext, (portContext->InstancePort + 16), 0UL);
	//COM422_PPORT_WRITE_INTMASK_BIT(portContext, portContext->InstancePort, 1UL);
	//HrKdPrint(("After -> RegIntMask: ptr(%p), val(%08X)\n", portContext->RegIntMask,
	//	COM422_PPORT_READ_INTMASK(portContext)));

	//HrKdPrint(("PortMask: %08X, WriteMask: %08X, IntMask: %08X\n", busContext->PortMask, busContext->WriteMask, busContext->IntMask));

	SET_REG_BIT(busContext->PortMask, portContext->InstancePort);
	COM422_PPORT_WRITE_CTRL_BITS(portContext, COM422_CTRL_ENCH_OFFSET, COM422_CTRL_ENCH_MASK, 1UL);

	// 设置触发深度
	//COM422_PPORT_WRITE_TXMAXDEP(portContext, portContext->InstancePort, 32);
	//COM422_PPORT_WRITE_TXCURDEP(portContext, portContext->InstancePort, 4);

	if (!busContext->TransmitOpen)
	{
		busContext->FnEnableTransmit(WdfInterruptGetDevice(Interrupt), TRUE);
	}

	HR_PRINT_METHOD_END();

	return TRUE;
}

BOOLEAN EvtSerialBusClose(
	IN WDFINTERRUPT Interrupt,
	IN WDFCONTEXT Context)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	PPDOPORT_CONTEXT_EXTENSION portContext;
	ULONG baudRate;

	HR_PRINT_METHOD_BEGIN();

	portContext = (PPDOPORT_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Context;
	busContext = (PBUS_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Data;

	//HrKdPrint(("PortMask: %08X, WriteMask: %08X, IntMask: %08X\n", busContext->PortMask, busContext->WriteMask, busContext->IntMask));

	//TODO: 使能端口发送中断
	//CLR_REG_BIT(busContext->WriteMask, portContext->InstancePort);
	//HrKdPrint(("Before -> RegIntMask: ptr(%p), val(%08X)\n", portContext->RegIntMask,
	//	COM422_PPORT_READ_INTMASK(portContext)));
	//COM422_PPORT_WRITE_INTMASK_BIT(portContext, (portContext->InstancePort + 16), 0UL);
	//COM422_PPORT_WRITE_INTMASK_BIT(portContext, portContext->InstancePort, 0UL);
	//HrKdPrint(("After -> RegIntMask: ptr(%p), val(%08X)\n", portContext->RegIntMask,
	//	COM422_PPORT_READ_INTMASK(portContext)));

	//HrKdPrint(("PortMask: %08X, WriteMask: %08X, IntMask: %08X\n", busContext->PortMask, busContext->WriteMask, busContext->IntMask));

	CLR_REG_BIT(busContext->PortMask, portContext->InstancePort);
	COM422_PPORT_WRITE_CTRL_BITS(portContext, COM422_CTRL_ENCH_OFFSET, COM422_CTRL_ENCH_MASK, 0UL);

	if (busContext->TransmitOpen && !busContext->PortMask)
	{
		busContext->FnEnableTransmit(WdfInterruptGetDevice(Interrupt), FALSE);
	}

	//HrKdPrint(("PortMask: %08X\n", busContext->PortMask));

	HR_PRINT_METHOD_END();

	return TRUE;
}
