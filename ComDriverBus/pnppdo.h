
#ifndef _COM422_PNP_PDO_H_
#define _COM422_PNP_PDO_H_

EVT_WDF_DEVICE_D0_ENTRY                 EvtDeviceD0EntryPdoPort;
EVT_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED EvtDeviceD0EntryPostInterruptsEnabledPdoPort;
EVT_WDF_DEVICE_D0_EXIT                  EvtDeviceD0ExitPdoPort;
EVT_WDF_DEVICE_D0_EXIT_PRE_INTERRUPTS_DISABLED EvtDeviceD0ExitPreInterruptsDisabledPdoPort;
EVT_WDF_DEVICE_PREPARE_HARDWARE         EvtDevicePrepareHardwarePdoPort;
EVT_WDF_DEVICE_RELEASE_HARDWARE         EvtDeviceReleaseHardwarePdoPort;
EVT_WDF_DEVICE_SELF_MANAGED_IO_CLEANUP  EvtDeviceSelfManagedIoCleanupPdoPort;
EVT_WDF_DEVICE_SELF_MANAGED_IO_FLUSH    EvtDeviceSelfManagedIoFlushPdoPort;
EVT_WDF_DEVICE_SELF_MANAGED_IO_INIT     EvtDeviceSelfManagedIoInitPdoPort;
EVT_WDF_DEVICE_SELF_MANAGED_IO_SUSPEND  EvtDeviceSelfManagedIoSuspendPdoPort;
EVT_WDF_DEVICE_SELF_MANAGED_IO_RESTART  EvtDeviceSelfManagedIoRestartPdoPort;
EVT_WDF_DEVICE_SURPRISE_REMOVAL         EvtDeviceSurpriseRemovalPdoPort;
EVT_WDF_DEVICE_QUERY_REMOVE             EvtDeviceQueryRemovePdoPort;
EVT_WDF_DEVICE_QUERY_STOP               EvtDeviceQueryStopPdoPort;
EVT_WDF_DEVICE_USAGE_NOTIFICATION       EvtDeviceUsageNotificationPdoPort;
EVT_WDF_DEVICE_RELATIONS_QUERY          EvtDeviceRelationsQueryPdoPort;

#endif  // _COM422_PNP_PDO_H_
