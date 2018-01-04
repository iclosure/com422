
#ifndef _COM422_WAIT_MASK_H_
#define _COM422_WAIT_MASK_H_

NTSTATUS WaitMaskCreate(
	IN PDEVICE_OBJECT DeviceObject);

NTSTATUS WaitMaskClose(
	IN PDEVICE_OBJECT DeviceObject);

#endif  // _COM422_WAIT_MASK_H_
