
#ifndef _COM422_FILE_H_
#define _COM422_FILE_H_

__drv_dispatchType(IRP_MJ_CREATE)   DRIVER_DISPATCH  IrpMajorCreate;
__drv_dispatchType(IRP_MJ_CLOSE)    DRIVER_DISPATCH  IrpMajorClose;
__drv_dispatchType(IRP_MJ_CLEANUP)  DRIVER_DISPATCH  IrpMajorCleanup;

__drv_dispatchType(IRP_MJ_QUERY_INFORMATION)	DRIVER_DISPATCH  IrpMajorQueryInformation;
__drv_dispatchType(IRP_MJ_SET_INFORMATION)		DRIVER_DISPATCH  IrpMajorSetInformation;

#endif  // _COM422_FILE_H_
