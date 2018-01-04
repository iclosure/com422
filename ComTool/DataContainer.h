#pragma once

#include <circfifo.h>
#include <list>
#include "comserial.h"

#if defined(_DEBUG)
#pragma comment(lib, "../Debug/comapi")
#else
#pragma comment(lib, "../Release/comapi")
#endif

// data format
#define DATA_FORMAT_BIN		0
#define DATA_FORMAT_OCT		1
#define DATA_FORAMT_DEC		2
#define DATA_FORMAT_HEX		3
#define DATA_FORMAT_TEXT	4

// 
typedef struct _COMM_BUFFER_INFO
{
	char*			Buffer;
	int				Size;

} COMM_BUFFER_INFO, *PCOMM_BUFFER_INFO;

// 
typedef struct _COMM_INFO_TX
{
	int				Period;		// 发送周期 (单位：ms)
	BOOL			EnPeriod;	// 发送方式（0:触发、1:周期）
	ULONG			Count;		// 累计发送数据字节数
	BOOL			Enable;		// 使能/禁止发送
	MmTimer			MmTimer;	// 高精度定时传输模块
	KMutex			mutex;		// 互斥锁
	COMM_BUFFER_INFO Buffer;	// 数据缓冲区

} COMM_INFO_TX, *PCOMM_INFO_TX;

// 
typedef struct _COMM_INFO_RX
{
	int				DataFormat;	// 帧数据格式（八进制；十进制；十六进制）
	ULONG			Count;		// 累计接收数据字节数
	BOOL			Enable;		// 使能/禁止接收
	MmTimer			MmTimer;	// 高精度定时传输模块
	KMutex			mutex;		// 互斥锁
	std::list<COMM_BUFFER_INFO> Buffer;	// 数据缓冲区
	
} COMM_INFO_RX, *PCOMM_INFO_RX;

// 
typedef struct _COMM_INFO
{
	int				Port;		// 端口号（1 ~ 256）
	int				BaudRate;	// 波特率
	int				DataBits;	// 数据位
	int				Parity;		// 校验位
	int				StopBits;	// 停止位

	SerialPort		serialPort;	// 串口对象

	COMM_INFO_TX	TX;			// 发送端信息
	COMM_INFO_RX	RX;			// 接收端信息

} COMM_INFO, *PCOMM_INFO;

// 
class CDataContainer : public KPairThread
{
public:
	CDataContainer();
	~CDataContainer();

public:
	int open(int port);
	int close();

	COMM_INFO & commInfo() { return m_commInfo; }

protected:
	bool workFirst();
	bool workSecond();

protected:
	BOOL initialize();

private:
	COMM_INFO			m_commInfo;
};
