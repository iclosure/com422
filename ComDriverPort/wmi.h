
#ifndef _COM422_PORT_WMI_H_
#define _COM422_PORT_WMI_H_

NTSTATUS WmiSystemControlCreate(
	IN PDEVICE_OBJECT DeviceObject);

// MajorFunction
__drv_dispatchType(IRP_MJ_SYSTEM_CONTROL)  DRIVER_DISPATCH  IrpMajorSystemControl;

// NinorFunction
DRIVER_DISPATCH		IrpSystemControlDefault;
__drv_dispatchType(IRP_MN_QUERY_ALL_DATA)			DRIVER_DISPATCH  IrpQueryAllData;
__drv_dispatchType(IRP_MN_QUERY_SINGLE_INSTANCE)	DRIVER_DISPATCH  IrpQuerySingleInstance;
__drv_dispatchType(IRP_MN_CHANGE_SINGLE_INSTANCE)	DRIVER_DISPATCH  IrpChangeSingleInstance;
__drv_dispatchType(IRP_MN_CHANGE_SINGLE_ITEM)		DRIVER_DISPATCH  IrpChangeSingleItem;
__drv_dispatchType(IRP_MN_ENABLE_EVENTS)			DRIVER_DISPATCH  IrpEnableEvents;
__drv_dispatchType(IRP_MN_DISABLE_EVENTS)			DRIVER_DISPATCH  IrpDisableEvents;
__drv_dispatchType(IRP_MN_ENABLE_COLLECTION)		DRIVER_DISPATCH  IrpEnableCollection;
__drv_dispatchType(IRP_MN_DISABLE_COLLECTION)		DRIVER_DISPATCH  IrpDisableCollection;
__drv_dispatchType(IRP_MN_REGINFO)					DRIVER_DISPATCH  IrpReginfo;
__drv_dispatchType(IRP_MN_EXECUTE_METHOD)			DRIVER_DISPATCH  IrpExecuteMethod;

//---WMI-CALLBACK

WMI_QUERY_REGINFO_CALLBACK		QueryWmiRegInfo;
WMI_QUERY_DATABLOCK_CALLBACK	QueryWmiDataBlock;
WMI_SET_DATABLOCK_CALLBACK		SetWmiDataBlock;
WMI_SET_DATAITEM_CALLBACK		SetWmiDataItem;
WMI_EXECUTE_METHOD_CALLBACK		ExecuteWmiMethod;
WMI_FUNCTION_CONTROL_CALLBACK	WmiFunctionControl;

#endif  // _COM422_PORT_WMI_H_
