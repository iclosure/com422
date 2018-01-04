
#ifndef _COM422_FILE_PDO_H_
#define _COM422_FILE_PDO_H_

EVT_WDF_DEVICE_FILE_CREATE            EvtDeviceFileCreatePdoPort;
EVT_WDF_FILE_CLOSE			          EvtFileClosePdoPort;
EVT_WDF_FILE_CLEANUP		          EvtFileCleanupPdoPort;

EVT_WDF_INTERRUPT_SYNCHRONIZE         EvtSerialBusCreate;
EVT_WDF_INTERRUPT_SYNCHRONIZE         EvtSerialBusClose;

#endif  // _COM422_FILE_PDO_H_
