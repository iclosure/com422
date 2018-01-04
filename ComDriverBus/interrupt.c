
#include "precomp.h"
#include "interrupt.h"

// Transmit-DPC
EVT_WDF_DPC		EvtWdfDpcIntWrite;
EVT_WDF_DPC		EvtWdfDpcIntRead;


// WORK-ITEM
EVT_WDF_WORKITEM	EvtWdfWorkItemIntTransmit;

//TODO-TIMER-COUNTER
EVT_WDF_TIMER		EvtTimerIsrCount;
EVT_WDF_TIMER		EvtRecvTimerDpc;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, InterruptCreate)
#endif

NTSTATUS InterruptCreate(
	IN WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	WDF_OBJECT_ATTRIBUTES attributes;
	WDF_INTERRUPT_CONFIG interruptConfig;
	WDF_TIMER_CONFIG timerConfig;
	WDF_DPC_CONFIG dpcConfig;

	PAGED_CODE();

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);

	// 创建读数据中断模块
	WDF_INTERRUPT_CONFIG_INIT(&interruptConfig, EvtInterruptIsr, EvtInterruptDpc);
	//interruptConfig.AutomaticSerialization = FALSE;  // Enable testing of the EvtInterruptIsr Synchronization
	status = WdfInterruptCreate(Device, &interruptConfig, WDF_NO_OBJECT_ATTRIBUTES, &busContext->Interrupt);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfInterruptCreate() ReadInterrupt failed as %08X\n", status));
		return status;
	}

	// 初始化变量
	busContext->FnEnableTransmit = EnableTransmit;

	//TODO-TIMER-COUNTER
	WDF_TIMER_CONFIG_INIT_PERIODIC(&timerConfig, EvtTimerIsrCount, 1000);	// Period: 3s
	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	attributes.ParentObject = Device;
	status = WdfTimerCreate(&timerConfig, &attributes, &busContext->TimerCounter);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfTimerCreate() TimerCounter failed as %08X\n", status));
		return status;
	}

	// Transmit-DPC-Write
	WDF_DPC_CONFIG_INIT(&dpcConfig, EvtWdfDpcIntWrite);
	//dpcConfig.AutomaticSerialization = FALSE;
	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	attributes.ParentObject = Device;
	WdfDpcCreate(&dpcConfig, &attributes, &busContext->IntWriteDpc);

	// Transmit-DPC-Read
	WDF_DPC_CONFIG_INIT(&dpcConfig, EvtWdfDpcIntRead);
	//dpcConfig.AutomaticSerialization = FALSE;
	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	attributes.ParentObject = Device;
	WdfDpcCreate(&dpcConfig, &attributes, &busContext->IntReadDpc);

	WdfTimerStart(busContext->TimerCounter, WDF_REL_TIMEOUT_IN_SEC(1));


	// 创建定时器模块
	busContext->RecvTimerPeriod = 2;  
	WDF_TIMER_CONFIG_INIT_PERIODIC(&timerConfig, EvtRecvTimerDpc, busContext->RecvTimerPeriod);
	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	attributes.ParentObject = Device;
	status = WdfTimerCreate(&timerConfig, &attributes, &busContext->RecvTimer);
	if (!NT_SUCCESS(status))
	{
		HrKdPrint(("WdfTimerCreate() failed as %d\n", status));
		return status;
	}

	HR_PRINT_METHOD_END();

	return status;
}

//TODO-TIMER-COUNTER
VOID EvtTimerIsrCount(
	__in
	WDFTIMER Timer
	)
{
	PBUS_CONTEXT_EXTENSION busContext;

	busContext = WdfBusGetContext(WdfTimerGetParentObject(Timer));

	DbgPrint(
		"ISR-> "
		"AC: %ld,"
		"WC: %ld,"
		"RC: %ld,"
		"\n",
		busContext->IsrAllCount,
		busContext->IsrWriteCount,
		busContext->IsrReadCount
		);

	busContext->IsrAllCount = 0;
	busContext->IsrWriteCount = 0;
	busContext->IsrReadCount = 0;
}

// 
NTSTATUS EnableTransmit(
	IN WDFDEVICE Device,
	IN BOOLEAN Enable)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	COM422_INTERRUPT_SYNC_CONTEXT syncContext;

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(Device);
	
	syncContext.Context = busContext;
	syncContext.Data = &Enable;

#ifdef DEBUG_WITH_REAL_DEVICE
	if (Enable)
	{
		if (!busContext->TransmitOpen)
		{
			//if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtEnableInterrupt, &syncContext))
			if (!EvtEnableInterrupt(busContext->Interrupt, &syncContext))
			{
				HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() EvtEnableInterrupt failed\n"));
				return STATUS_UNSUCCESSFUL;
			}
		}
		WdfTimerStart(busContext->RecvTimer, WDF_REL_TIMEOUT_IN_MS(busContext->RecvTimerPeriod));
		DbgPrint("WdfTimerStart -------");
	}
	else
	{
		WdfTimerStop(busContext->RecvTimer, FALSE);
		if (busContext->TransmitOpen)
		{
			//if (!HR_INTERRUPT_SYNCHRONIZE(busContext->Interrupt, EvtEnableInterrupt, &syncContext))
			if (!EvtEnableInterrupt(busContext->Interrupt, &syncContext))
			{
				HrKdPrint(("HR_INTERRUPT_SYNCHRONIZE() EvtEnableInterrupt failed\n"));
				return STATUS_UNSUCCESSFUL;
			}
		}
	}
#endif  // DEBUG_WITH_REAL_DEVICE

	busContext->TransmitOpen = Enable;

	HR_PRINT_METHOD_END();

	return status;
}

// 开启、关闭设备中断
BOOLEAN EvtEnableInterrupt(
	IN WDFINTERRUPT Interrupt,
	IN WDFCONTEXT Context)
{
	BOOLEAN status = TRUE;
	PBUS_CONTEXT_EXTENSION busContext;
	BOOLEAN enable;
	INT_CSR intCsr;

	HR_PRINT_METHOD_BEGIN();

	busContext = (PBUS_CONTEXT_EXTENSION)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Context;
	enable = *(BOOLEAN*)((PCOM422_INTERRUPT_SYNC_CONTEXT)Context)->Data;

	HrKdPrint(("EnableInterrupt: %s\n", enable ? "TRUE" : "FALSE"));

	intCsr.all = READ_REGISTER_ULONG(busContext->RegIntCsr);
	intCsr.bits.PciIntEnable = enable;
	intCsr.bits.LocalIntInputEnable = enable;
	intCsr.bits.DmaChan0IntEnable = enable;
	WRITE_REGISTER_ULONG(busContext->RegIntCsr, intCsr.all);

	HR_PRINT_METHOD_END();

	return status;
}

BOOLEAN EvtInterruptIsr(
	IN WDFINTERRUPT Interrupt,
	IN ULONG MessageID)
{
	BOOLEAN status = TRUE;
	PBUS_CONTEXT_EXTENSION busContext;
	//INT_STAT intState, intMask;
	LONG    channel;
	ULONG   regWStat;
	PUCHAR  PBARADDR;
	INT_CSR intCsr;
	DMA_CSR dmaCsr;

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(WdfInterruptGetDevice(Interrupt));
	busContext->IsrAllCount++;

	PBARADDR = busContext->BAR[COM422_PCI_BAR_SRAM].Addr;

	regWStat = 0xFF & READ_REGISTER_ULONG((PULONG)(PBARADDR + XXX_WRITE_INT_STAT));
	intCsr.all = READ_REGISTER_ULONG(busContext->RegIntCsr);

	if (regWStat || intCsr.bits.DmaChan0IntActive) {
		if (regWStat) {
			busContext->IsrWriteCount++;
			// 保存中断状态掩码，并关闭中断
			busContext->RegWStat |= regWStat;
			for (channel = 0; channel < busContext->PortNum; channel++) {
				if (regWStat & (1 << channel)) {
					DbgPrint("## -> IsrIntEvent: port=%02d\n", channel);
					WRITE_REGISTER_ULONG(COM422_BUS_CTRL_ADDR(busContext, channel),
						READ_REGISTER_ULONG(COM422_BUS_CTRL_ADDR(busContext, channel)) & ~XXX_WRITE_MASK_BIT); // 中断上报屏蔽位，（bit 16）写0禁止中断上报
				}
			}
		}


		if (intCsr.bits.DmaChan0IntActive)
		{
			busContext->IsrReadCount++;
			dmaCsr.all = READ_REGISTER_UCHAR(busContext->RegDma0Csr);
			dmaCsr.bits.Clear = TRUE;
			WRITE_REGISTER_UCHAR(busContext->RegDma0Csr, dmaCsr.all);

			busContext->DmaDone |= dmaCsr.bits.Done;
		}

		if (WdfInterruptQueueDpcForIsr(busContext->Interrupt)) {
			// Disable PCI input interrupt
			INT_CSR intCsr;
			intCsr.all = READ_REGISTER_ULONG(busContext->RegIntCsr);
			intCsr.bits.PciIntEnable = FALSE;
			WRITE_REGISTER_ULONG(busContext->RegIntCsr, intCsr.all);

			return TRUE;
		}

	}

	
	HR_PRINT_METHOD_END();

	return FALSE;
}

#pragma LOCKEDCODE
VOID EvtInterruptDpc(
	IN WDFINTERRUPT Interrupt,
	IN WDFOBJECT AssociatedObject)
{
	PBUS_CONTEXT_EXTENSION busContext;
	LONG channel;
	ULONG recvCount;
	ULONG index;
	USHORT data;
	INT_CSR intCsr;

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(WdfInterruptGetDevice(Interrupt));

	WdfInterruptAcquireLock(busContext->Interrupt);

	if (busContext->RegWStat) {

		for (channel = 0; channel < busContext->PortNum; channel++) {
			if (GET_REG_BIT(busContext->RegWStat, channel)) {
				DbgPrint("## -> DpcIntEvent: port=%02d\n", channel);
				KeSetEvent(busContext->EvtWriteInt[channel], IO_NO_INCREMENT, FALSE);
			}
		}

		busContext->RegWStat = 0;
	}

	///////////////////////////////////////////////////////////////////////////


	if (busContext->DmaDone) {
		recvCount = busContext->ReadCommonBufferCount / sizeof(USHORT);
		for (index = 0; index < recvCount; index++) {
			data = busContext->ReadCommonBuffer[index];

			if (data >= 0x0F00) {
				continue;
			} else {
				channel = (data >> 8) - 1;
				if (channel >= 0 && channel < 8) {
					CircFifoWrite(busContext->CircFifo[channel], &data);
				}
			}
		}

		// 查询DMA是否满足终端条件（接收数据量超过中断阈值）
		recvCount = READ_REGISTER_ULONG((PULONG)(busContext->BAR[COM422_PCI_BAR_SRAM].Addr + COM422_RX_DCNT)) * sizeof(USHORT);
		HrKdPrint(("recvCount: %ld\n", recvCount));

		if (recvCount <= COM422_INTERRPUT_THRESHOLD * sizeof(USHORT)) {
			WdfTimerStart(busContext->RecvTimer, WDF_REL_TIMEOUT_IN_MS(2));
		} else {
			DMA_CSR dmaCsr;

			recvCount = min(recvCount, busContext->ReadCommonBufferLength);
			busContext->ReadCommonBufferCount = recvCount;

			// 配置传输大小
			WRITE_REGISTER_ULONG(busContext->RegDma0Count, recvCount);		// byte

			// 启动DMA传输
			dmaCsr.all = READ_REGISTER_UCHAR(busContext->RegDma0Csr);
			dmaCsr.bits.Enable = TRUE;
			dmaCsr.bits.Start = TRUE;
			WRITE_REGISTER_UCHAR(busContext->RegDma0Csr, dmaCsr.all);
		}

		busContext->DmaDone = FALSE;
	}

	intCsr.all = READ_REGISTER_ULONG(busContext->RegIntCsr);
	intCsr.bits.PciIntEnable = TRUE;
	WRITE_REGISTER_ULONG(busContext->RegIntCsr, intCsr.all);

	WdfInterruptReleaseLock(busContext->Interrupt);

	HR_PRINT_METHOD_END();
}

#pragma  LOCKEDCODE
VOID EvtWdfDpcIntWrite(
	IN WDFDPC Dpc)
{
	PBUS_CONTEXT_EXTENSION busContext;
	LONG channel;

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(WdfDpcGetParentObject(Dpc));

	for (channel = 0; channel < busContext->PortNum; channel++) {
		if (GET_REG_BIT(busContext->RegWStat, channel)) {
			DbgPrint("## -> DpcIntEvent: port=%02d\n", channel);
			KeSetEvent(busContext->EvtWriteInt[channel], IO_NO_INCREMENT, FALSE);
		}
	}

	busContext->RegWStat = 0;

	HR_PRINT_METHOD_END();
}

#pragma  LOCKEDCODE
VOID EvtWdfDpcIntRead(
	IN WDFDPC Dpc)
{

	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	ULONG recvCount;
	ULONG index;
	USHORT data;
	int ch;

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(WdfDpcGetParentObject(Dpc));

	WdfInterruptAcquireLock(busContext->Interrupt);

	/*if (busContext->ReadDmaDone)
	{*/

	recvCount = busContext->ReadCommonBufferCount / sizeof(USHORT);
	for (index = 0; index < recvCount; index++)
	{
		data = busContext->ReadCommonBuffer[index];

		HrKdPrint(("Index: %ld, Channel: %02X, Data: %02X ('%c')\n", index,
			data >> 8, (data & 0xFF), (data & 0xFF)));
		DbgPrint("Index: %ld, Channel: %02X, Data: %02X ('%c')\n", index,
			data >> 8, (data & 0xFF), (data & 0xFF));

		if (data >= 0x0F00)		// 
		{
			continue;
		}
		else
		{
			ch = (data >> 8) - 1;
			if (ch >= 0 && ch < 8)
			{
				CircFifoWrite(busContext->CircFifo[ch], &data);
			}
		}
	}

	// 查询DMA是否满足终端条件（接收数据量超过中断阈值）
	recvCount = READ_REGISTER_ULONG((PULONG)(busContext->BAR[COM422_PCI_BAR_SRAM].Addr + COM422_RX_DCNT)) * sizeof(USHORT);
	HrKdPrint(("recvCount: %ld\n", recvCount));

	if (recvCount <= COM422_INTERRPUT_THRESHOLD * sizeof(USHORT))
	{
		WdfTimerStart(busContext->RecvTimer, WDF_REL_TIMEOUT_IN_MS(busContext->RecvTimerPeriod));
	}
	else
	{
		DMA_CSR dmaCsr;

		recvCount = min(recvCount, busContext->ReadCommonBufferLength); 
		busContext->ReadCommonBufferCount = recvCount;

		// 配置传输大小
		WRITE_REGISTER_ULONG(busContext->RegDma0Count, recvCount);		// byte

		// 启动DMA传输
		dmaCsr.all = READ_REGISTER_UCHAR(busContext->RegDma0Csr);
		dmaCsr.bits.Enable = TRUE;
		dmaCsr.bits.Start = TRUE;
		WRITE_REGISTER_UCHAR(busContext->RegDma0Csr, dmaCsr.all);
	}

	WdfInterruptReleaseLock(busContext->Interrupt);

	HR_PRINT_METHOD_END();

}



VOID EvtRecvTimerDpc(
	IN WDFTIMER Timer)
{
	NTSTATUS status = STATUS_SUCCESS;
	PBUS_CONTEXT_EXTENSION busContext;
	ULONG recvCount;

	HR_PRINT_METHOD_BEGIN();

	busContext = WdfBusGetContext(WdfTimerGetParentObject(Timer));

	// 获取RXFIFO中数据量
	recvCount = READ_REGISTER_ULONG((PULONG)(busContext->BAR[COM422_PCI_BAR_SRAM].Addr + COM422_RX_DCNT)) * sizeof(USHORT);
	HrKdPrint(("recvCount: %ld\n", recvCount * sizeof(USHORT)));
	//DbgPrint("recvCount: %ld\n", recvCount / sizeof(USHORT));
	if (recvCount == 0)
	{
		return;
	}
	else
	{
		DMA_CSR dmaCsr;

		WdfTimerStop(Timer, FALSE);
		recvCount = min(recvCount, busContext->ReadCommonBufferLength);		// 更新recvCount
		busContext->ReadCommonBufferCount = recvCount;

		// 配置传输大小
		// busContext->RegDma0Count 传输数据量寄存器
		//告诉DMA 控制器 需要传输的数据量
		WRITE_REGISTER_ULONG(busContext->RegDma0Count, recvCount);

		// 启动DMA传输
		dmaCsr.all = READ_REGISTER_UCHAR(busContext->RegDma0Csr);
		dmaCsr.bits.Enable = TRUE;
		dmaCsr.bits.Start = TRUE;
		WRITE_REGISTER_UCHAR(busContext->RegDma0Csr, dmaCsr.all);
	}

	HR_PRINT_METHOD_END();
}
