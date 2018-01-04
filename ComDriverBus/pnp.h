
#ifndef _COM422_PNP_EVENT_H_
#define _COM422_PNP_EVENT_H_

EVT_WDF_DEVICE_D0_ENTRY                 EvtDeviceD0Entry;
EVT_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED EvtDeviceD0EntryPostInterruptsEnabled;
EVT_WDF_DEVICE_D0_EXIT                  EvtDeviceD0Exit;
EVT_WDF_DEVICE_D0_EXIT_PRE_INTERRUPTS_DISABLED EvtDeviceD0ExitPreInterruptsDisabled;
EVT_WDF_DEVICE_PREPARE_HARDWARE         EvtDevicePrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE         EvtDeviceReleaseHardware;
EVT_WDF_DEVICE_SELF_MANAGED_IO_CLEANUP  EvtDeviceSelfManagedIoCleanup;
EVT_WDF_DEVICE_SELF_MANAGED_IO_FLUSH    EvtDeviceSelfManagedIoFlush;
EVT_WDF_DEVICE_SELF_MANAGED_IO_INIT     EvtDeviceSelfManagedIoInit;
EVT_WDF_DEVICE_SELF_MANAGED_IO_SUSPEND  EvtDeviceSelfManagedIoSuspend;
EVT_WDF_DEVICE_SELF_MANAGED_IO_RESTART  EvtDeviceSelfManagedIoRestart;
EVT_WDF_DEVICE_SURPRISE_REMOVAL         EvtDeviceSurpriseRemoval;
EVT_WDF_DEVICE_QUERY_REMOVE             EvtDeviceQueryRemove;
EVT_WDF_DEVICE_QUERY_STOP               EvtDeviceQueryStop;
EVT_WDF_DEVICE_USAGE_NOTIFICATION       EvtDeviceUsageNotification;
EVT_WDF_DEVICE_RELATIONS_QUERY          EvtDeviceRelationsQuery;

#endif  // _COM422_PNP_EVENT_H_
