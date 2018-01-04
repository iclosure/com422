
#ifndef _COM422_POWER_EVENT_H_
#define _COM422_POWER_EVENT_H_

// MajorFunction
__drv_dispatchType(IRP_MJ_POWER)			DRIVER_DISPATCH  IrpMajorPower;

// NinorFunction
DRIVER_DISPATCH		IrpPowerDefault;
__drv_dispatchType(IRP_MN_WAIT_WAKE)		DRIVER_DISPATCH  IrpWaitWake;
__drv_dispatchType(IRP_MN_POWER_SEQUENCE)	DRIVER_DISPATCH  IrpPowerSequence;
__drv_dispatchType(IRP_MN_SET_POWER)		DRIVER_DISPATCH  IrpSetPower;
__drv_dispatchType(IRP_MN_QUERY_POWER)		DRIVER_DISPATCH  IrpQueryPower;

#endif  // _COM422_POWER_EVENT_H_
