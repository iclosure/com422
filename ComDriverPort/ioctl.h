
#ifndef _COM422_IO_CTL_H_
#define _COM422_IO_CTL_H_

NTSTATUS QueueWriteCreate(
	IN PDEVICE_OBJECT DeviceObject);

NTSTATUS QueueReadCreate(
	IN PDEVICE_OBJECT DeviceObject);

__drv_dispatchType(IRP_MJ_READ)                    DRIVER_DISPATCH  IrpMajorRead;
__drv_dispatchType(IRP_MJ_WRITE)                   DRIVER_DISPATCH  IrpMajorWrite;
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)          DRIVER_DISPATCH  IrpMajorInternalIoControl;
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)          DRIVER_DISPATCH  IrpMajorDeviceControl;

#endif	// _COM422_IO_CTL_H_
