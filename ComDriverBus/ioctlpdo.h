
#ifndef _COM422_IO_COTL_PDO_H_
#define _COM422_IO_COTL_PDO_H_

NTSTATUS PdoPortIoQueueCreate(
	IN WDFDEVICE Device);

EVT_WDF_IO_QUEUE_IO_READ              EvtIoReadPdoPort;
EVT_WDF_IO_QUEUE_IO_WRITE             EvtIoWritePdoPort;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL    EvtIoDeviceControlPdoPort;

EVT_WDF_INTERRUPT_SYNCHRONIZE         EvtSerialSetBaudRate;
EVT_WDF_INTERRUPT_SYNCHRONIZE         EvtSerialGetBaudRate;
EVT_WDF_INTERRUPT_SYNCHRONIZE         EvtSerialSetLineControl;
EVT_WDF_INTERRUPT_SYNCHRONIZE         EvtSerialGetLineControl;
EVT_WDF_INTERRUPT_SYNCHRONIZE         EvtSerialSetDTR;
EVT_WDF_INTERRUPT_SYNCHRONIZE         EvtSerialSetRTS;
EVT_WDF_INTERRUPT_SYNCHRONIZE         EvtSerialSetModemStatus;
EVT_WDF_INTERRUPT_SYNCHRONIZE         EvtSerialGetModemStatus;
EVT_WDF_INTERRUPT_SYNCHRONIZE         EvtSerialPurge;
EVT_WDF_INTERRUPT_SYNCHRONIZE         EvtSerialGetCommStatus;

#endif	// _COM422_IO_COTL_PDO_H_
