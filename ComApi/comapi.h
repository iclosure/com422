
#ifndef COM422_API_H
#define COM422_API_H

/* --------------------------- API Version ----------------------- */

#define CCOM_API_VERSION		0x01000000

/* ---------------------- Global Definitions --------------------- */

#define CCOM_SLOTS_NUMS		    5	  /*The maximum board number*/

#define CCOM_OFF                0
#define CCOM_ON                 1

/* ------------------------ BaudRate : Start ------------------------
 * 50 Hz,      call -> input: 50
 * 75 Hz,      call -> input: 75
 * 300 Hz,     call -> input: 300
 * 600 Hz,     call -> input: 600
 * 1200 Hz,    call -> input: 1200
 * 4800 Hz,    call -> input: 4800
 * 7200 Hz,    call -> input: 7200
 * 9600 Hz,    call -> input: 9600
 * 14400 Hz,   call -> input: 14400
 * 19200 Hz,   call -> input: 19200
 * 38400 Hz,   call -> input: 38400
 * 57600 Hz,   call -> input: 57600
 * 115200 Hz,  call -> input: 115200
 * 230400 Hz,  call -> input: 230400
 * 460800 Hz,  call -> input: 460800
 * 921600 Hz,  call -> input: 921600
 * 3125000 Hz, call -> input: 3125000
 * ------------------------ BaudRate : End ------------------------*/

/* ------------------------ DataBits : Start ------------------------
 * 4 bits, call -> input: 4
 * 5 bits, call -> input: 5
 * 6 bits, call -> input: 6
 * 7 bits, call -> input: 7
 * 8 bits, call -> input: 8
 * ------------------------ DataBits : End ------------------------*/

/*Specify the stopBits*/
#define CCOM_STOPBITS_1		    0
#define CCOM_STOPBITS_1_5	    1
#define CCOM_STOPBITS_2		    2

/*Specify the parity bit*/
#define CCOM_PARITY_NONE		0
#define CCOM_PARITY_ODD			1
#define CCOM_PARITY_EVEN		2
#define CCOM_PARITY_MARK		3
#define CCOM_PARITY_SPACE		4

/*Specify the mode control*/
#define CCOM_CTRL_DTR			0x01
#define CCOM_CTRL_RTS			0x02

/*Specify the modem line status*/
#define CCOM_STAT_CTS			0x01
#define CCOM_STAT_DSR			0x02
#define CCOM_STAT_RI			0x04
#define CCOM_STAT_CD			0x08

/*Flush any data within the driver's input/output buffer with one of the following options*/
#define CCOM_FLUSH_INPUT		0	 /*Flush input buffer*/
#define CCOM_FLUSH_OUTPUT		1	 /*Flush output buffer*/
#define CCOM_FLUSH_BOTH			2	 /*Flush both the input and output buffer*/

/*Specify the control, status or error code*/
#define CCOM_SUCCESS			 0   /*Call method successfully*/
#define CCOM_BADPORT			-1   /*Port number is invalid*/
#define CCOM_OUTCONTROL			-2   /*Can't control the board*/
#define CCOM_ABORTREAD		    -3   /*Read has blocked, and user abort read*/
#define CCOM_NODATA				-4   /*No data to read or no buffer to write*/
#define CCOM_OPENFAIL		    -5   /*No such port or port is occupied by other program*/
#define CCOM_RTS_BY_HW			-6   /*RTS can't set because H/W flow control*/
#define CCOM_BADPARAM			-7   /*Bad parameter*/
#define CCOM_WINFAILURE		    -8   /*Call Win32 function fail, please call GetLastError to get the error code*/
#define CCOM_BOARDNOTSUPPORT    -9	 /*Does not support this board*/
#define CCOM_FAILURE			-10  /*comapi function run result fail*/
#define CCOM_ABORTWRITE			-11  /*Write has blocked, and user abort write*/
#define CCOM_WRITETIMEOUT		-12  /*Write timeout has happened*/
#define CCOM_PORTNOTOPEN		-13  /* port is not opened */

/* ------------------------- API -------------------------*/

#define __crd_api	__stdcall

#ifdef __cplusplus
extern "C" {
#endif	/* _cplusplus */

/*---------------------------------------------------------
 *	                 Universal Functions
 *---------------------------------------------------------*/

/*Port control functions*/

int __crd_api com_open(int Port);  /*Open a serial port for data transmitting/receiving*/
int __crd_api com_close(int Port);  /*Disable a serial port so that it cannot receive/transmit data*/
int __crd_api com_flush(int Port, int Function);  /*Flush any data within the driver's input/output buffer*/

/*Port status inquiry*/

int __crd_api com_lstatus(int Port);  /*Get line status,such as CTS,DSR,DCD,RI*/
int __crd_api com_iqueue(int Port);  /*Get the length of data accumulated in the input buffer*/
int __crd_api com_oqueue(int Port);  /*Get the length of data remaining in the output buffer*/
int __crd_api com_data_status(int Port);  /*Check if any error encountered when receiving data*/
int __crd_api com_tx_hold(int Port);  /*Check the reason why data could not be transmitted*/

/*Functions to config the options of a serial port*/

int __crd_api com_config(int Port, int BaudRate, int DataBits, int Parity, int StopBits);  /*Set the properties of a serial port*/

int __crd_api com_set_baudrate(int Port, int BaudRate);  /*Set the data bits of a serial port*/
int __crd_api com_get_baudrate(int Port);  /*Get the data bits of a serial port*/

int __crd_api com_set_databits(int Port, int DataBits);  /*Set the data bits of a serial port*/
int __crd_api com_get_databits(int Port);  /*Get the data bits of a serial port*/

int __crd_api com_set_parity(int Port, int Parity);  /*Set the parity of a serial port*/
int __crd_api com_get_parity(int Port);  /*Get the parity of a serial port*/

int __crd_api com_set_stopbits(int Port, int StopBits);  /*Set the stop bits of a serial port*/
int __crd_api com_get_stopbits(int Port);  /*Get the stop bits of a serial port*/

int __crd_api com_flowctrl(int Port, int Mode);  /*Set hardware and/or software flow control*/
int __crd_api com_DTR(int Port, int Mode);  /*Set DTR state for a serial port*/
int __crd_api com_RTS(int Port, int Mode);  /*Set RTS state for a serial port*/
int __crd_api com_lctrl(int Port, int Mode);  /*Set both the DTR and RTS state*/

/*Input Data (This category includes functions to read data from a serial port)*/

int __crd_api com_getch(int Port);  /*Read one character from the driver's input buffer*/
int __crd_api com_read(int Port, char* Buffer, int Size);  /*Read data from the driver's input buffer*/
int __crd_api com_setReadTimeouts(int Port, unsigned long TotalTimeouts, unsigned long IntervalTimeouts);  /*Set timeout values for com_getch or com_read*/
int __crd_api com_getReadTimeouts(int Port, unsigned long* TotalTimeouts, unsigned long* IntervalTimeouts);  /*Get read timeout value for a serial port*/
int __crd_api com_abort_read(int Port);  /*Force blocked com_getch or com_read to return immediately*/

/*Output Data (This category includes functions to write data to a serial port)*/

int __crd_api com_putch(int Port, int Character);  /*Write a character into the driver's output buffer*/
int __crd_api com_write(int Port, char* Buffer, int Size);  /*Put a block of data to the driver's output buffer.The actual length of data written depends on the amount of freee space in the driver's output buffer*/
int __crd_api com_setWriteTimeouts(int Port, unsigned long TotalTimeouts);  /*Set timeout values for com_putch or com_write*/
int __crd_api com_getWriteTimeouts(int Port, unsigned long* TotalTimeouts);  /*Get write timeout value for a serial port*/
int __crd_api com_abort_write(int Port);  /*Force blocked com_putch or com_write to abort.Calling of this function will cause com_putch or com_write to return immediately with return code CRD_ABORTWRITE*/

/*Event Service: Port Status Inquiry (This category includes functions 
 *to set the communication event service routine for a serial port)*/

int __crd_api com_term_irq(int Port, void(__crd_api*CallBack)(int Port), int Code);  /*Invoke event service routine when terminator character received*/
int __crd_api com_cnt_irq(int Port, void(__crd_api*CallBack)(int Port), int Count);  /*Invoke event service routine when certain amount of data recevied*/
int __crd_api com_modem_irq(int Port, void(__crd_api*CallBack)(int Port));  /*Invoke event service routine when line status changed*/
int __crd_api com_break_irq(int Port, void(__crd_api*CallBack)(int Port));  /*Invoke event service routine when break signal received*/
int __crd_api com_tx_empty_irq(int Port, void(__crd_api*CallBack)(int Port));  /*Invoke event service routine as transmit buffer is empty*/

/*Miscellaneous (This category includes functions to do special function for a serial port.)*/

int __crd_api com_break(int Port, int Time);  /*Send out break signal*/
int __crd_api com_break_ex(int Port, int Time);  /*Send out break signal*/
int __crd_api com_act_xon(int Port);  /*Causes transmission to act as if an xon character has been received*/
int __crd_api com_act_xoff(int Port);  /*Causes transmission to act as if an xoff character has been received*/

/*The end*/

#ifdef __cplusplus
};
#endif	/* _cplusplus */

#endif  /* !COM422_API_H */
