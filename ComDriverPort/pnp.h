
#ifndef _COM422_PNP_EVENT_H_
#define _COM422_PNP_EVENT_H_

// MajorFunction
__drv_dispatchType(IRP_MJ_PNP)  DRIVER_DISPATCH  IrpMajorPnp;
__drv_dispatchType(IRP_MJ_SHUTDOWN)  DRIVER_DISPATCH  IrpMajorShutDown;

// NinorFunction
DRIVER_DISPATCH		IrpPnpDefault;
__drv_dispatchType(IRP_MN_START_DEVICE)           DRIVER_DISPATCH  IrpPnpStartDevice;
__drv_dispatchType(IRP_MN_QUERY_REMOVE_DEVICE)    DRIVER_DISPATCH  IrpPnpQueryRemoveDrivice;
__drv_dispatchType(IRP_MN_REMOVE_DEVICE)          DRIVER_DISPATCH  IrpPnpRemoveDevice;
__drv_dispatchType(IRP_MN_CANCEL_REMOVE_DEVICE)   DRIVER_DISPATCH  IrpPnpCancelRemoveDevice;
__drv_dispatchType(IRP_MN_STOP_DEVICE)            DRIVER_DISPATCH  IrpPnpStopDevice;
__drv_dispatchType(IRP_MN_QUERY_STOP_DEVICE)      DRIVER_DISPATCH  IrpPnpQueryStopDevice;
__drv_dispatchType(IRP_MN_CANCEL_STOP_DEVICE)     DRIVER_DISPATCH  IrpPnpCancelStopDevice;
__drv_dispatchType(IRP_MN_QUERY_DEVICE_RELATIONS) DRIVER_DISPATCH  IrpPnpQueryDeviceRelations;
__drv_dispatchType(IRP_MN_QUERY_INTERFACE)        DRIVER_DISPATCH  IrpPnpQueryInterface;
__drv_dispatchType(IRP_MN_QUERY_CAPABILITIES)     DRIVER_DISPATCH  IrpPnpQueryCapabilities;
__drv_dispatchType(IRP_MN_QUERY_RESOURCES)        DRIVER_DISPATCH  IrpPnpQueryResources;
__drv_dispatchType(IRP_MN_QUERY_RESOURCE_REQUIREMENTS) DRIVER_DISPATCH  IrpPnpQueryResourceRequirements;
__drv_dispatchType(IRP_MN_QUERY_DEVICE_TEXT)      DRIVER_DISPATCH  IrpPnpQueryDeviceText;
__drv_dispatchType(IRP_MN_FILTER_RESOURCE_REQUIREMENTS) DRIVER_DISPATCH  IrpPnpFilterResourceRequirements;
__drv_dispatchType(IRP_MN_READ_CONFIG)            DRIVER_DISPATCH  IrpPnpReadConfig;
__drv_dispatchType(IRP_MN_WRITE_CONFIG)           DRIVER_DISPATCH  IrpPnpWriteConfig;
__drv_dispatchType(IRP_MN_EJECT)                  DRIVER_DISPATCH  IrpPnpEject;
__drv_dispatchType(IRP_MN_SET_LOCK)               DRIVER_DISPATCH  IrpPnpSetLock;
__drv_dispatchType(IRP_MN_QUERY_ID)               DRIVER_DISPATCH  IrpPnpQueryId;
__drv_dispatchType(IRP_MN_QUERY_PNP_DEVICE_STATE) DRIVER_DISPATCH  IrpPnpQueryPnpDeviceState;
__drv_dispatchType(IRP_MN_QUERY_BUS_INFORMATION)  DRIVER_DISPATCH  IrpPnpQueryBusInformation;
__drv_dispatchType(IRP_MN_DEVICE_USAGE_NOTIFICATION) DRIVER_DISPATCH  IrpPnpDeviceUsageNotification;
__drv_dispatchType(IRP_MN_SURPRISE_REMOVAL)       DRIVER_DISPATCH  IrpPnpSupriseRemoval;

IO_COMPLETION_ROUTINE  IoRequestCompleteRoutine;
NTSTATUS ForwardAndWait(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp);

#endif  // _COM422_PNP_EVENT_H_
