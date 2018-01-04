
#ifndef _COM422_BUS_COMMON_H_
#define _COM422_BUS_COMMON_H_

///////////////////////////////////////////////////////////////////////

#define DEBUG_WITH_REAL_DEVICE	// for debug with virtual device

////////////////////////////////////////////// /////////////////////////

#include "regdef.h"

////////////////////////////////////////////// /////////////////////////

#define COM422_MAX_INSTANCE_ID_LEN	80
#define COM422_SYMBOLIC_NAME_LEN	128

#define COM422_SERIAL_DEVICEMAP		L"SERIALCOMM"
#define COM422_PREF_WIN32_DEVNAME	L"\\DosDevices\\"

#define COM422_BUS_HWIDS			L"PCI\\VEN_BE00&DEV_1080&SUBSYS_90540422&REV_01"	// device bus hardware id
#define COM422_BUS_DEVDESCS			L"CRDTech C422 Smartio/Industio Driver (PCI BUS)"	// device bus description
#define COM422_BUS_DEVLOCAT			L"CRDTech Multi-port Bus"							// device bus location
#define COM422_BUS_COMPATIBLE_IDS	L"CrdSerial\\CrdSerBusCompatible"					//

#define CRD422_PORT_DEVIDS			L"CrdSerial\\CrdSer"			// port device id
#define CRD422_PORT_HWIDS			L"CrdSerial\\CrdSerPortOJO"		// port device hardware id
#define COM422_PORT_COMPATIBLE_IDS	L"CrdSerial\\CrdSerPortCompatible\0"
#define COM422_PORT_DEVDESCS		L"CRDTech communication port"	// port device description
#define COM422_PORT_FRIENDLYNAME	L"CRDTech Communication Port"	// port friendly name
#define COM422_PORT_DEVLOCAT		L"CRDTech Bus"					// port device location


#define COM422_REG_RFIFO			0x000fc							// DMA 数据源地址
#define COM422_RX_DCNT				0x000f8				
#define COM422_INTERRPUT_THRESHOLD		1024	// 中断门限值
///////////////////////////////////////////////////////////////////////

//! driver context extension
typedef struct _DRIVER_CONTEXT_EXTENSION
{
	LONG				Instance;	// 1 ~ ?
	//UNICODE_STRING		InstanceName;
	//UNICODE_STRING		MofResourceName;
	UNICODE_STRING		RegistryPath;

} DRIVER_CONTEXT_EXTENSION, *PDRIVER_CONTEXT_EXTENSION;

#ifdef COM422_DRIVER_BUS

// Enable/Disable Interrupt transfer
typedef NTSTATUS FN_BUS_ENABLE_TRANSMIT(
	IN WDFDEVICE Device,
	IN BOOLEAN Enable);
typedef FN_BUS_ENABLE_TRANSMIT *PFN_BUS_ENABLE_TRANSMIT;

// 
//typedef union _INT_STATE
//{
//	struct
//	{
//		ULONG PortState : 16;
//		ULONG WriteState : 16;
//	};
//	ULONG All;
//
//} INT_STAT, *PINT_STAT;

//! bus context extension
typedef struct _BUS_CONTEXT_EXTENSION
{
	LONG				Instance;		// 1 ~ ?
	LONG				PortNum;		// number of all serial ports
	USHORT              PortMask;		// channel mask that is use for interrupt status
	//union {
	//	struct {
	//		USHORT		PortMask;		// channel mask that is use for interrupt status
	//		USHORT		WriteMask;
	//	};
	//	ULONG			IntMask;		// Interrupt mask
	//};

	////////////////////////////////////////////////////////////////////

	struct {
		PUCHAR			Addr;			// 
		ULONG			Length;			// 
	} BAR[PCI_TYPE0_ADDRESSES];

	PPCI9054_REGS		RegPci;
	PULONG				RegIntCsr;
	//PULONG				RegIntStat;
	//PULONG				RegIntMask;
	//PULONG				RegIntCtrl;

	PUCHAR				RegDma0Csr;
	PULONG				RegDma0Mode;
	PULONG				RegDma0Count;
	PUSHORT				ReadCommonBuffer;
	PHYSICAL_ADDRESS	ReadCommonBufferLA;
	ULONG				ReadCommonBufferLength;
	ULONG				ReadCommonBufferCount;
	BOOLEAN				ReadDmaDone;

	LONG				RecvTimerPeriod;
	WDFTIMER			RecvTimer;

	PUCHAR				RegRxBuff;
	UCHAR				RecvBuffer[COM422_RECV_MAX_COUNT];
	
	////////////////////////////////////////////////////////////////////

	PCIRCFIFO_OBJECT	CircFifo[COM422_MAX_PORT_COUNT];	// 
	PCIRCFIFO_OBJECT    SendFifo[COM422_MAX_PORT_COUNT];
	PKEVENT				EvtWriteInt[COM422_MAX_PORT_COUNT];	// 
	PKEVENT				EvtReadInt[COM422_MAX_PORT_COUNT];	// 
	
	PFN_BUS_ENABLE_TRANSMIT	FnEnableTransmit;	// 
	WDFINTERRUPT		Interrupt;				//
	BOOLEAN				TransmitOpen;			// 

	// Transmit-DPC
	WDFDPC				IntWriteDpc;
	WDFDPC				IntReadDpc;

	// WMI
	SERIAL_WMI_COMM_DATA	WmiCommData;
	SERIAL_WMI_HW_DATA		WmiHwData;
	SERIAL_WMI_PERF_DATA	WmiPerfData;

	////////////////////////////////////////////////////////////////////

	//TODO-TIMER-COUNTER
	WDFTIMER			TimerCounter;
	ULONG				IsrAllCount;
	ULONG				IsrWriteCount;
	ULONG				IsrReadCount;

	ULONG               RegWStat;
	BOOLEAN             DmaDone;

} BUS_CONTEXT_EXTENSION, *PBUS_CONTEXT_EXTENSION;

// 
typedef struct _PDO_IDENTIFICATION_DESCRIPTION
{
	WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER Header; // should contain this header 

	//
	// Unique serial number of the device on the bus
	//
	ULONG				SerialNo;

	size_t				CchHardwareIds;
	__field_bcount(CchHardwareIds) PWCHAR HardwareIds;

} PDO_IDENTIFICATION_DESCRIPTION, *PPDO_IDENTIFICATION_DESCRIPTION;
#endif  // COM422_DRIVER_BUS

//! function device object port context extension
typedef struct _SERIALPORT_IO_INFO
{
	SERIAL_BAUD_RATE	BaudRate;		// 
	SERIAL_LINE_CONTROL	LineCtrl;		// 
	ULONG				ModemStatus;	// 
	ULONG				PurgeFlags;		// 

	SERIAL_HANDFLOW     handFlow;
	SERIAL_CHARS        specialChars;

	LONG                xoffCounter;
	ULONG               errors;
	ULONG               amountInWriteQueue;
	ULONG               waitMask;
	ULONG               eventMask;
	ULONG               insertMask;
	UCHAR               escapeChar;
	SERIALPERF_STATS    perfStats;
	
	ULONG WriteCount;
	ULONG WriteOffset;

	ULONG ReadCount;
	ULONG ReadOffset;
	
} SERIALPORT_IO_INFO, *PSERIALPORT_IO_INFO;

//! physical device object port context extension
typedef struct _PDOPORT_CONTEXT_EXTENSION
{
	LONG				InstanceBus;	// 1 ~ ?
	LONG				InstancePort;	// 0 ~ ?
	ULONG				PortMask;		// 

	SERIALPORT_IO_INFO	PortIoInfo;
	PCIRCFIFO_OBJECT	CircFifo;		//  
	//PCIRCFIFO_OBJECT    SendFifo;
	PKEVENT				EvtWriteInt;	// 
	PKEVENT				EvtReadInt;		// 
	//PKSPIN_LOCK         PSL_WMask;
	PKMUTEX				PMutexWriteMask;

#ifdef DEBUG_WITH_REAL_DEVICE
	//PULONG				RegIntMask;
	//PULONG				RegIntCtrl;
	
	//PULONG              RegIntRMask;
	//PULONG              RegIntRStat;
	//PULONG              RegIntWMask;
	PULONG              RegIntWStat;

	PULONG				RegCtrl;
	PULONG				RegStat;
	PULONG				RegTxRoom;
	PULONG				RegRxSize;
	PULONG				RegRxMaxDep;
	PULONG				RegRxCurDep;
	PULONG				RegTxMaxDep;
	PULONG				RegTxCurDep;
	PUCHAR				RegTxBuff;
	PUCHAR				RegRxBuff;
#else
	ULONG				RegCtrl;
	ULONG				RegStat;
#endif  // DEBUG_WITH_REAL_DEVICE

#ifdef COM422_DRIVER_BUS
	WDFSPINLOCK			SpinLockIoctl;
#endif

} PDOPORT_CONTEXT_EXTENSION, *PPDOPORT_CONTEXT_EXTENSION;

//! Read context extension
typedef struct _COM422_INTERRUPT_SYNC_CONTEXT
{
	PVOID Context;	// 
	PVOID Data;		// 

} COM422_INTERRUPT_SYNC_CONTEXT, *PCOM422_INTERRUPT_SYNC_CONTEXT;

#if defined(COM422_DRIVER_BUS) && defined(KERNEL_FRAMEWORK_WDF)

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DRIVER_CONTEXT_EXTENSION, WdfDriverGetContext);
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(BUS_CONTEXT_EXTENSION, WdfBusGetContext);
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(PDOPORT_CONTEXT_EXTENSION, WdfPdoGetPortContext);

#endif  // KERNEL_FRAMEWORK_WDF && COM422_DRIVER_BUS

////////////////////////////////////////////////////////////////

#ifdef COM422_DRIVER_BUS

#ifdef DEBUG_WITH_REAL_DEVICE

#define HR_INTERRUPT_SYNCHRONIZE WdfInterruptSynchronize

#else

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
FORCEINLINE
HR_INTERRUPT_SYNCHRONIZE(
	IN WDFINTERRUPT Interrupt,
	IN PFN_WDF_INTERRUPT_SYNCHRONIZE Callback,
	IN WDFCONTEXT Context)
{
	return Callback(Interrupt, Context);
}

#endif // !DEBUG_WITH_REAL_DEVICE

#endif  // COM422_DRIVER_BUS

////////////////////////////////////////////////////////////////

#endif	// _COM422_BUS_COMMON_H_
