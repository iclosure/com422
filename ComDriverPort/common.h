
#ifndef _COM422_PORT_COMMON_H_
#define _COM422_PORT_COMMON_H_

//////////////////////////////////////////////////////////////////////////

#define COM422_ALLOW_WMI_HW_GUID

//////////////////////////////////////////////////////////////////////////

//! function device object port context extension
typedef struct _FDOPORT_CONTEXT_EXTENSION
{
	PDEVICE_OBJECT		TheDevice;		// 
	PDEVICE_OBJECT		LowDevice;		// 
	UNICODE_STRING		PortName;		// 
	UNICODE_STRING		DeviceName;		// 
	UNICODE_STRING		SymLinkName;	// 
	ULONG				PortIndex;		// 
	BOOLEAN				IsOpened;		// 

	SERIAL_TIMEOUTS		Timeouts;		// 
	SERIAL_HANDFLOW		HandFlow;		// 
	ULONG				RxFifo;			// 
	ULONG				TxFifo;			// 

	SERIAL_CHARS		Chars;			// 
	SERIAL_QUEUE_SIZE	QueueSize;		// 

	PIRP				WaitMaskIrp;	//
	ULONG				WaitMask;		// 
	KTIMER				WaitMaskTimer;	// 
	KDPC				WaitMaskTimerDpc;	// 
	LARGE_INTEGER		WaitMaskDueTime;// 
	KEVENT				EvtRxChars;		// 

	CIRCFIFO_OBJECT		CircFifo;		//
	PKMUTEX             PMutexWriteMask;

//	CIRCFIFO_OBJECT     SendFifo;

	PIRP				WriteIrp;		// 
	PIRP				ReadIrp;		//

	KEVENT				EvtWriteInt;	// 
	KEVENT				EvtReadInt;		// 

	KTIMER				TimerWriteTotal;// 
	KDPC				TimerWriteTotalDpc;	// 

	LARGE_INTEGER       TimeoutInterval;	// 
	KTIMER				TimerReadTotal;		// 
	KDPC				TimerReadTotalDpc;	//

	KTIMER				TimerReadInterval;	// 
	KDPC				TimerReadIntervalDpc;	//

	KSPIN_LOCK			SpinLockInt;	// Interrupt lock
	KSPIN_LOCK			SpinLockIoctl;	// ioctl lock
	KSPIN_LOCK			SpinLockRead;	// read lock
	KSPIN_LOCK			SpinLockWrite;	// write lock

	// Read-Queue
	KDEVICE_QUEUE		QueueRead;

	// Write-Queue
	KDEVICE_QUEUE		QueueWrite;

	// WMI
	//WMILIB_CONTEXT		WmiLibContext;	// 

} FDOPORT_CONTEXT_EXTENSION, *PFDOPORT_CONTEXT_EXTENSION;

/* declaration of global functions */

#endif	// _COM422_PORT_COMMON_H_
