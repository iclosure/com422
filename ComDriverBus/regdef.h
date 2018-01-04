
#ifndef _COM422_REGISTER_H_
#define _COM422_REGISTER_H_


#define XXX_WRITE_INT_STAT 0x0024
#define XXX_WRITE_MASK_BIT 0x20000

/* xxx definition */

#ifdef DEBUG_WITH_REAL_DEVICE
#define COM422_MAX_PORT_COUNT			16		// 
#else
#define COM422_MAX_PORT_COUNT			8		// 
#endif  // DEBUG_WITH_REAL_DEVICE
#define COM422_BUFF_MAX_COUNT			(400*1024)	// circfifo max length
//#define COM422_RECV_FULL80_COUNT		4096
#define COM422_RECV_MAX_COUNT			1024
#define COM422_SEND_MAX_COUNT			1024
#define COM422_SEND_FIFO_SIZE           2048

/* register definition */

// global register

#define COM422_DEVID_ADDR_OFFSET		(0x00000000UL)	// 设备ID
#define COM422_CHNUM_ADDR_OFFSET		(0x00000004UL)	// 通道数
//#define COM422_INTSTAT_ADDR_OFFSET		(0x00000008UL)	// 中断状态寄存器
//#define COM422_INTMASK_ADDR_OFFSET		(0x0000000CUL)	// 中断屏蔽寄存器
//#define COM422_INTCTRL_ADDR_OFFSET		(0x00000010UL)	// 中断屏蔽设置寄存器

// channel register

#define COM422_CH_INDEX_CTRL			(0x00000000UL)	// 通道控制寄存器索引
#define COM422_CH_INDEX_STAT			(0x00000004UL)	// 通道状态寄存器索引
#define COM422_CH_INDEX_TXROOM			(0x00000008UL)	// 通道发送端FIFO空闲字节量寄存器索引
#define COM422_CH_INDEX_RXSIZE			(0x0000000CUL)	// 通道接收端FIFO有效字节量寄存器索引
#define COM422_CH_INDEX_RXMAXDEP		(0x00000010UL)	// 通道接收端最大触发深度
#define COM422_CH_INDEX_RXCURDEP		(0x00000014UL)	// 通道接收端当前触发深度
#define COM422_CH_INDEX_TXMAXDEP		(0x00000018UL)	// 通道发送端最大触发深度
#define COM422_CH_INDEX_TXCURDEP		(0x0000001CUL)	// 通道发送端当前触发深度
#define COM422_CH_INDEX_TXBUFF			(0x00001000UL)	// 通道发送端FIFO缓冲区索引
#define COM422_CH_INDEX_RXBUFF			(0x00002000UL)	// 通道接收端FIFO缓冲区索引

#define COM422_CH_ADDR_OFFSET(_channel_, _reg_)		((((_channel_) + 1UL) << 15) + (_reg_))		// 通道寄存器基址 (_channel_: 0 ~ ?)

/* bits of ConTRoL register */

#define COM422_CTRL_DATABITS_OFFSET		0			// 数据位
#define COM422_CTRL_DATABITS_MASK		0x00000003	// 数据位掩码

#define COM422_CTRL_STOPBITS_OFFSET		2			// 停止位
#define COM422_CTRL_STOPBITS_MASK		0x0000000C	// 停止位掩码

#define COM422_CTRL_PARITY_OFFSET		4			// 校验位
#define COM422_CTRL_PARITY_MASK			0x00000070	// 校验位掩码

#define COM422_CTRL_BUADRATE_OFFSET		8			// 波特率
#define COM422_CTRL_BUADRATE_MASK		0x0000FF00	// 波特率掩码

#define COM422_CTRL_ENCH_OFFSET			16			// 通道使能标志
#define COM422_CTRL_ENCH_MASK			0x00010000	// 通道使能标志掩码

#define COM422_CTRL_RXEMPTY_OFFSET		17			// 接收端FIFO空标志
#define COM422_CTRL_RXEMPTY_MASK		0x00020000	// 接收端FIFO空标志掩码

#define COM422_CTRL_RXHALF_OFFSET		18			// 接收端FIFO半满标志
#define COM422_CTRL_RXHALF_MASK			0x00040000	// 接收端FIFO半满标志掩码

#define COM422_CTRL_RXFULL_OFFSET		19			// 接收端FIFO满标志
#define COM422_CTRL_RXFULL_MASK			0x00080000	// 接收端FIFO满标志掩码

#define COM422_CTRL_TXEMPTY_OFFSET		20			// 发送端FIFO空标志
#define COM422_CTRL_TXEMPTY_MASK		0x00100000	// 发送端FIFO空标志掩码

#define COM422_CTRL_TXHALF_OFFSET		21			// 发送端FIFO半满标志
#define COM422_CTRL_TXHALF_MASK			0x00200000	// 发送端FIFO半满标志掩码

#define COM422_CTRL_TXFULL_OFFSET		22			// 发送端FIFO满标志
#define COM422_CTRL_TXFULL_MASK			0x00400000	// 发送端FIFO满标志掩码

#define COM422_CTRL_OVERFLOW_OFFSET		23			// 溢出标志
#define COM422_CTRL_OVERFLOW_MASK		0x00800000	// 溢出标志掩码

#define COM422_CTRL_PARITYERR_OFFSET	24			// 校验错误标志
#define COM422_CTRL_PARITYERR_MASK		0x01000000	// 校验错误标志掩码

/* ----------------------- for 9-wire(RS232) interface ------------------- */

#define COM422_CTRL_DTR_OFFSET			25			// 数据终端就绪标志
#define COM422_CTRL_DTR_MASK			0x02000000	// 数据终端就绪标志掩码

#define COM422_CTRL_RTS_OFFSET			26			// 请求发送标志
#define COM422_CTRL_RTS_MASK			0x04000000	// 请求发送标志掩码

#define COM422_CTRL_CTS_OFFSET			27			// 清除发送标志
#define COM422_CTRL_CTS_MASK			0x08000000	// 清除发送标志掩码

#define COM422_CTRL_DSR_OFFSET			28			// 数据设备就绪标志
#define COM422_CTRL_DSR_MASK			0x10000000	// 数据设备就绪标志掩码

#define COM422_CTRL_RI_OFFSET			29			// ??
#define COM422_CTRL_RI_MASK				0x20000000	// ??

#define COM422_CTRL_DCD_OFFSET			30			// 数据载波检测标志
#define COM422_CTRL_DCD_MASK			0x40000000	// 数据载波加测标志掩码

#define COM422_CTRL_DELL_OFFSET			31			// 振铃指令标志
#define COM422_CTRL_DELL_MASK			0x80000000	// 振铃指令标志掩码

// 通道状态寄存器位定义

#define COM422_CH_STAT_TXEMPTY_OFFSET	0				// 发送端FIFO空
#define COM422_CH_STAT_TXEMPTY_MASK		(0x00000001UL)	// 发送端FIFO空掩码

#define COM422_CH_STAT_TXFULL_OFFSET	1				// 发送端FIFO满
#define COM422_CH_STAT_TXFULL_MASK		(0x00000002UL)	// 发送端FIFO满掩码

#define COM422_CH_STAT_TXHALF_OFFSET	2				// 发送端FIFO半满
#define COM422_CH_STAT_TXHALF_MASK		(0x00000004UL)	// 发送端FIFO半满掩码

#define COM422_CH_STAT_RXEMPTY_OFFSET	3				// 接收端FIFO空
#define COM422_CH_STAT_RXEMPTY_MASK		(0x00000008UL)	// 接收端FIFO空掩码

#define COM422_CH_STAT_RXFULL_OFFSET	4				// 接收端FIFO满
#define COM422_CH_STAT_RXFULL_MASK		(0x00000010UL)	// 接收端FIFO满掩码

// for modem status ( rs232 )
#define COM422_CTRL_MODEMSTATUS_OFFSET	COM422_CTRL_DTR_OFFSET
#define COM422_CTRL_MODEMSTATUS_MASK (\
	COM422_CTRL_DTR_MASK | \
	COM422_CTRL_RTS_MASK | \
	COM422_CTRL_CTS_MASK | \
	COM422_CTRL_RI_MASK  | \
	COM422_CTRL_DCD_MASK | \
	COM422_CTRL_RTS_MASK)

/* ------------------------------------------------------------------ */

/* recode of bits */

#define COM422_CTRL_DATABITS_8			0
#define COM422_CTRL_DATABITS_7			1
#define COM422_CTRL_DATABITS_6			2
#define COM422_CTRL_DATABITS_5			3

#define COM422_CTRL_STOPBITS_1			0
#define COM422_CTRL_STOPBITS_2			1
#define COM422_CTRL_STOPBITS_1_5		2	// （暂未实现）

#define COM422_CTRL_PARITY_NONE			0
#define COM422_CTRL_PARITY_ODD			1
#define COM422_CTRL_PARITY_EVEN			2
#define COM422_CTRL_PARITY_MARK			3
#define COM422_CTRL_PARITY_SPACE		4

#define COM422_CTRL_BAUDRATE_50			0x00
#define COM422_CTRL_BAUDRATE_75			0x01
#define COM422_CTRL_BAUDRATE_300		0x02
#define COM422_CTRL_BAUDRATE_600		0x03
#define COM422_CTRL_BAUDRATE_1200		0x04
#define COM422_CTRL_BAUDRATE_1800		0x05
#define COM422_CTRL_BAUDRATE_2400		0x06
#define COM422_CTRL_BAUDRATE_4800		0x07
#define COM422_CTRL_BAUDRATE_7200		0x08
#define COM422_CTRL_BAUDRATE_9600		0x09
#define COM422_CTRL_BAUDRATE_14400		0x0A
#define COM422_CTRL_BAUDRATE_19200		0x0B
#define COM422_CTRL_BAUDRATE_38400		0x0C
#define COM422_CTRL_BAUDRATE_57600		0x0D
#define COM422_CTRL_BAUDRATE_115200		0x0E
#define COM422_CTRL_BAUDRATE_230400		0x0F
#define COM422_CTRL_BAUDRATE_460800		0x10
#define COM422_CTRL_BAUDRATE_921600		0x11
#define COM422_CTRL_BAUDRATE_3125000	0x12

//////////////////////////////////////////////////////////////////////////

#define COM422_PCI_MEMORY_BLOCK_NUM		4

#define COM422_PCI_BAR_REGS				0	// BAR0
#define COM422_PCI_BAR_PORT				1	// BAR1
#define COM422_PCI_BAR_SRAM				2	// BAR2
#define COM422_PCI_BAR_SRAM2			3	// BAR3

///////////////////////////////////////////////////////////////////////

#ifdef DEBUG_WITH_REAL_DEVICE

//! the type of Addr(BAR[.]) must be 'PUCHAR'

#define COM422_WRITE_REGISTER_BARX(_context_, _bar_, _addr_, _value_)  \
	WRITE_REGISTER_ULONG((PULONG)((_context_)->BAR[_bar_].Addr + (_addr_)), (_value_))
#define COM422_READ_REGISTER_BARX(_context_, _bar_, _addr_)  \
	READ_REGISTER_ULONG((PULONG)((_context_)->BAR[_bar_].Addr + (_addr_)))

#define COM422_WRITE_REGISTER(_context_, _addr_, _value_) \
	COM422_WRITE_REGISTER_BARX((_context_), (COM422_PCI_BAR_SRAM), (_addr_), (_value_))
#define COM422_READ_REGISTER(_context_, _addr_) \
	COM422_READ_REGISTER_BARX((_context_), (COM422_PCI_BAR_SRAM), (_addr_))

/*---------------------------------------------------------------------*/

// global register

#define COM422_BUS_DEVID_ADDR(_context_) \
	((PULONG)((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + (COM422_DEVID_ADDR_OFFSET)))

#define COM422_BUS_CHNUM_ADDR(_context_) \
	((PULONG)((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + (COM422_CHNUM_ADDR_OFFSET)))

#define COM422_BUS_INTSTAT_ADDR(_context_) \
	((PULONG)((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + (COM422_INTSTAT_ADDR_OFFSET)))

#define COM422_BUS_INTMASK_ADDR(_context_) \
	((PULONG)((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + (COM422_INTMASK_ADDR_OFFSET)))

#define COM422_BUS_INTCTRL_ADDR(_context_) \
	((PULONG)((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + (COM422_INTCTRL_ADDR_OFFSET)))

// channel register

#define COM422_BUS_CTRL_ADDR(_context_, _channel_) \
	((PULONG)((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + \
	COM422_CH_ADDR_OFFSET(_channel_, COM422_CH_INDEX_CTRL)))

#define COM422_BUS_STAT_ADDR(_context_, _channel_) \
	((PULONG)((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + \
	COM422_CH_ADDR_OFFSET(_channel_, COM422_CH_INDEX_STAT)))

#define COM422_BUS_TXROOM_ADDR(_context_, _channel_) \
	((PULONG)((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + \
	COM422_CH_ADDR_OFFSET(_channel_, COM422_CH_INDEX_TXROOM)))

#define COM422_BUS_RXSIZE_ADDR(_context_, _channel_) \
	((PULONG)((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + \
	COM422_CH_ADDR_OFFSET(_channel_, COM422_CH_INDEX_RXSIZE)))

#define COM422_BUS_RXMAXDEP_ADDR(_context_, _channel_) \
	((PULONG)((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + \
	COM422_CH_ADDR_OFFSET(_channel_, COM422_CH_INDEX_RXMAXDEP)))

#define COM422_BUS_RXCURDEP_ADDR(_context_, _channel_) \
	((PULONG)((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + \
	COM422_CH_ADDR_OFFSET(_channel_, COM422_CH_INDEX_RXCURDEP)))

#define COM422_BUS_TXMAXDEP_ADDR(_context_, _channel_) \
	((PULONG)((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + \
	COM422_CH_ADDR_OFFSET(_channel_, COM422_CH_INDEX_TXMAXDEP)))

#define COM422_BUS_TXCURDEP_ADDR(_context_, _channel_) \
	((PULONG)((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + \
	COM422_CH_ADDR_OFFSET(_channel_, COM422_CH_INDEX_TXCURDEP)))

#define COM422_BUS_TXBUFF_ADDR_BASE(_context_, _channel_) \
	((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + \
	COM422_CH_ADDR_OFFSET(_channel_, COM422_CH_INDEX_TXBUFF))

#define COM422_BUS_RXBUFF_ADDR_BASE(_context_, _channel_) \
	((_context_)->BAR[COM422_PCI_BAR_SRAM].Addr + \
	COM422_CH_ADDR_OFFSET(_channel_, COM422_CH_INDEX_RXBUFF))

/*-------global--------------------------------------------------------------*/

//---DEVID

#define COM422_BUS_READ_DEVID(_context_) \
	READ_REGISTER_ULONG(COM422_BUS_DEVID_ADDR(_context_))

//---CHNUM

#define COM422_BUS_READ_CHNUM(_context_) \
	READ_REGISTER_ULONG(COM422_BUS_CHNUM_ADDR(_context_))

//---INTSTAT

#define COM422_BUS_READ_INTSTAT(_context_) \
	READ_REGISTER_ULONG((_context_)->RegIntStat)
	//READ_REGISTER_ULONG(COM422_BUS_INTSTAT_ADDR(_context_))

//---INTMASK

#define COM422_BUS_READ_INTMASK(_context_) \
	READ_REGISTER_ULONG((_context_)->RegIntMask)
	//READ_REGISTER_ULONG(COM422_BUS_INTMASK_ADDR(_context_))

#define COM422_BUS_WRITE_INTMASK(_context_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegIntMask, _value_)
//WRITE_REGISTER_ULONG(COM422_BUS_INTMASK_ADDR(_context_), _value_)

#define COM422_BUS_WRITE_INTMASK_BIT(_context_, _bit_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegIntMask, CONFIG_REG_BIT_RETURN( \
	READ_REGISTER_ULONG((_context_)->RegIntMask), (_bit_), (_value_)))

#define COM422_BUS_WRITE_INTMASK_BITS(_context_, _channel_, _offset_, _mask_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegIntMask, CONFIG_REG_BITS_RETURN( \
	READ_REGISTER_ULONG((_context_)->RegIntMask), (_offset_), (_mask_), (_value_)))

#define COM422_BUS_READ_INTMASK_BITS(_context_, _channel_, _offset_, _mask_) \
	GET_REG_BITS_VAL(READ_REGISTER_ULONG((_context_)->RegIntMask), (_offset_), (_mask_))

//---INTCTRL

#define COM422_BUS_READ_INTCTRL(_context_) \
	READ_REGISTER_ULONG((_context_)->RegIntCtrl)
	//READ_REGISTER_ULONG(COM422_BUS_INTCTRL_ADDR(_context_))

#define COM422_BUS_WRITE_INTCTRL(_context_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegIntCtrl, _value_)
	//WRITE_REGISTER_ULONG(COM422_BUS_INTCTRL_ADDR(_context_), _value_)

#define COM422_BUS_WRITE_INTCTRL_BIT(_context_, _bit_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegIntCtrl, CONFIG_REG_BIT_RETURN( \
	READ_REGISTER_ULONG((_context_)->RegIntCtrl), (_bit_), (_value_)))

#define COM422_BUS_WRITE_INTCTRL_BITS(_context_, _channel_, _offset_, _mask_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegIntCtrl, CONFIG_REG_BITS_RETURN( \
	READ_REGISTER_ULONG((_context_)->RegIntCtrl), (_offset_), (_mask_), (_value_)))

#define COM422_BUS_READ_INTCTRL_BITS(_context_, _channel_, _offset_, _mask_) \
	GET_REG_BITS_VAL(READ_REGISTER_ULONG((_context_)->RegIntCtrl), (_offset_), (_mask_))

/*--------bus----------------------------------------------------------*/

//---CTRL

#define COM422_BUS_READ_CTRL(_context_, _channel_) \
	READ_REGISTER_ULONG(COM422_BUS_CTRL_ADDR((_context_), (_channel_)))

#define COM422_BUS_WRITE_CTRL_BITS(_context_, _channel_, _offset_, _mask_, _value_) \
	WRITE_REGISTER_ULONG(COM422_CH_CTRL_ADDR((_context_), (_channel_)), \
	CONFIG_REG_BITS_RETURN(READ_REGISTER_ULONG( \
	COM422_BUS_CTRL_ADDR((_context_), (_channel_))), (_offset_), (_mask_), (_value_)))

#define COM422_BUS_READ_CTRL_BITS(_context_, _channel_, _offset_, _mask_) \
	GET_REG_BITS_VAL(READ_REGISTER_ULONG( \
	COM422_BUS_CTRL_ADDR((_context_), (_channel_))), (_offset_), (_mask_))

//---STAT

#define COM422_BUS_READ_STAT(_context_, _channel_) \
	READ_REGISTER_ULONG(COM422_BUS_STAT_ADDR((_context_), (_channel_)))

#define COM422_BUS_READ_STAT_BITS(_context_, _channel_, _offset_, _mask_) \
	GET_REG_BITS_VAL(READ_REGISTER_ULONG( \
	COM422_BUS_STAT_ADDR((_context_), (_channel_))), (_offset_), (_mask_))

//---STAT::detail

#define COM422_BUS_READ_STAT_TXEMPTY(_context_, _channel_) \
	COM422_BUS_READ_STAT_BITS(_context_, _channel_, COM422_CH_STAT_TXEMPTY_OFFSET, COM422_CH_STAT_TXEMPTY_MASK)

#define COM422_BUS_READ_STAT_TXFULL(_context_, _channel_) \
	COM422_BUS_READ_STAT_BITS(_context_, _channel_, COM422_CH_STAT_TXFULL_OFFSET, COM422_CH_STAT_TXFULL_MASK)

#define COM422_BUS_READ_STAT_TXHALF(_context_, _channel_) \
	COM422_BUS_READ_STAT_BITS(_context_, _channel_, COM422_CH_STAT_TXHALF_OFFSET, COM422_CH_STAT_TXHALF_MASK)

#define COM422_BUS_READ_STAT_RXEMPTY(_context_, _channel_) \
	COM422_BUS_READ_STAT_BITS(_context_, _channel_, COM422_CH_STAT_RXEMPTY_OFFSET, COM422_CH_STAT_RXEMPTY_MASK)

#define COM422_BUS_READ_STAT_RXFULL(_context_, _channel_) \
	COM422_BUS_READ_STAT_BITS(_context_, _channel_, COM422_CH_STAT_RXFULL_OFFSET, COM422_CH_STAT_RXFULL_MASK)

//---TXROOM

#define COM422_BUS_READ_TXROOM(_context_, _channel_) \
	READ_REGISTER_ULONG(COM422_BUS_TXROOM_ADDR((_context_), (_channel_)))

//---RXSIZE

#define COM422_BUS_READ_RXSIZE(_context_, _channel_) \
	READ_REGISTER_ULONG(COM422_BUS_RXSIZE_ADDR((_context_), (_channel_)))

//---RX-MAXDEPTH

#define COM422_BUS_READ_RXMAXDEP(_context_, _channel_) \
	READ_REGISTER_ULONG(COM422_BUS_RXMAXDEP_ADDR((_context_), (_channel_)))

#define COM422_BUS_WRITE_RXMAXDEP(_context_, _channel_, _value_) \
	WRITE_REGISTER_ULONG(COM422_BUS_RXMAXDEP_ADDR((_context_), (_channel_)), (_value_))

//---RX-CURDEPTH

#define COM422_BUS_READ_RXCURDEP(_context_, _channel_) \
	READ_REGISTER_ULONG(COM422_BUS_RXCURDEP_ADDR((_context_), (_channel_)))

#define COM422_BUS_WRITE_RXCURDEP(_context_, _channel_, _value_) \
	WRITE_REGISTER_ULONG(COM422_BUS_RXCURDEP_ADDR((_context_), (_channel_)), (_value_))

//---TX-MAXDEPTH

#define COM422_BUS_READ_TXMAXDEP(_context_, _channel_) \
	READ_REGISTER_ULONG(COM422_BUS_TXMAXDEP_ADDR((_context_), (_channel_)))

#define COM422_BUS_WRITE_TXMAXDEP(_context_, _channel_, _value_) \
	WRITE_REGISTER_ULONG(COM422_BUS_TXMAXDEP_ADDR((_context_), (_channel_)), (_value_))

//---TX-CURDEPTH

#define COM422_BUS_READ_TXCURDEP(_context_, _channel_) \
	READ_REGISTER_ULONG(COM422_BUS_TXCURDEP_ADDR((_context_), (_channel_)))

#define COM422_BUS_WRITE_TXCURDEP(_context_, _channel_, _value_) \
	WRITE_REGISTER_ULONG(COM422_BUS_TXCURDEP_ADDR((_context_), (_channel_)), (_value_))

//---WRITE-BUFF

#define COM422_BUS_WRITE_UCHARS(_context_, _channel_, _buffer_, _count_) \
	WRITE_REGISTER_BUFFER_UCHAR(COM422_BUS_TXBUFF_ADDR_BASE((_context_), (_channel_)),\
	(_buffer_), (_count_))

//---READ-BUFF

#define COM422_BUS_READ_UCHARS(_context_, _channel_, _buffer_, _count_) \
	READ_REGISTER_BUFFER_UCHAR(COM422_BUS_RXBUFF_ADDR_BASE((_context_), (_channel_)),\
	(_buffer_), (_count_))

#define COM422_BUS_READ_USHORTS(_context_, _channel_, _buffer_, _count_) \
	READ_REGISTER_BUFFER_USHORT((PUSHORT)COM422_BUS_RXBUFF_ADDR_BASE((_context_), (_channel_)),\
	(_buffer_), (_count_))

/*--------physical port-----------------------------------------------------------*/

//---GLOBAL
#if 0
#define COM422_PPORT_READ_INTMASK(_context_) \
	READ_REGISTER_ULONG((_context_)->RegIntMask)

#define COM422_PPORT_WRITE_INTMASK(_context_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegIntMask, _value_)

#define COM422_PPORT_WRITE_INTMASK_BIT(_context_, _bit_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegIntMask, CONFIG_REG_BIT_RETURN( \
	READ_REGISTER_ULONG((_context_)->RegIntMask), (_bit_), (_value_)))

#define COM422_PPORT_WRITE_INTMASK_BITS(_context_, _offset_, _mask_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegIntMask, CONFIG_REG_BITS_RETURN( \
	READ_REGISTER_ULONG((_context_)->RegIntMask), (_offset_), (_mask_), (_value_)))

#define COM422_PPORT_READ_INTMASK_BITS(_context_, _offset_, _mask_) \
	GET_REG_BITS_VAL(READ_REGISTER_ULONG((_context_)->RegIntMask), (_offset_), (_mask_))

#define COM422_PPORT_READ_INTCTRL(_context_) \
	READ_REGISTER_ULONG((_context_)->RegIntCtrl)

#define COM422_PPORT_WRITE_INTCTRL(_context_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegIntCtrl, _value_)

#define COM422_PPORT_WRITE_INTCTRL_BIT(_context_, _bit_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegIntCtrl, CONFIG_REG_BIT_RETURN( \
	READ_REGISTER_ULONG((_context_)->RegIntCtrl), (_bit_), (_value_)))

#define COM422_PPORT_WRITE_INTCTRL_BITS(_context_, _offset_, _mask_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegIntCtrl, CONFIG_REG_BITS_RETURN( \
	READ_REGISTER_ULONG((_context_)->RegIntCtrl), (_offset_), (_mask_), (_value_)))

#define COM422_PPORT_READ_INTCTRL_BITS(_context_, _offset_, _mask_) \
	GET_REG_BITS_VAL(READ_REGISTER_ULONG((_context_)->RegIntCtrl), (_offset_), (_mask_))
#endif

//---CTRL

#define COM422_PPORT_READ_CTRL(_context_) \
	READ_REGISTER_ULONG((_context_)->RegCtrl)

#define COM422_PPORT_WRITE_CTRL_BITS(_context_, _offset_, _mask_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegCtrl, CONFIG_REG_BITS_RETURN( \
	READ_REGISTER_ULONG((_context_)->RegCtrl), (_offset_), (_mask_), (_value_)))

#define COM422_PPORT_READ_CTRL_BITS(_context_, _offset_, _mask_) \
	GET_REG_BITS_VAL(READ_REGISTER_ULONG((_context_)->RegCtrl), (_offset_), (_mask_))

//---STAT

#define COM422_PPORT_READ_STAT(_context_) \
	READ_REGISTER_ULONG((_context_)->RegStat)

#define COM422_PPORT_READ_STAT_BITS(_context_, _offset_, _mask_) \
	GET_REG_BITS_VAL(READ_REGISTER_ULONG((_context_)->RegStat), (_offset_), (_mask_))

//---STAT::detail

#define COM422_PPORT_READ_STAT_TXEMPTY(_context_) \
	COM422_PPORT_READ_STAT_BITS(_context_, COM422_CH_STAT_TXEMPTY_OFFSET, COM422_CH_STAT_TXEMPTY_MASK)

#define COM422_PPORT_READ_STAT_TXFULL(_context_) \
	COM422_PPORT_READ_STAT_BITS(_context_, COM422_CH_STAT_TXFULL_OFFSET, COM422_CH_STAT_TXFULL_MASK)

#define COM422_PPORT_READ_STAT_TXHALF(_context_) \
	COM422_PPORT_READ_STAT_BITS(_context_, COM422_CH_STAT_TXHALF_OFFSET, COM422_CH_STAT_TXHALF_MASK)

#define COM422_PPORT_READ_STAT_RXEMPTY(_context_) \
	COM422_PPORT_READ_STAT_BITS(_context_, COM422_CH_STAT_RXEMPTY_OFFSET, COM422_CH_STAT_RXEMPTY_MASK)

#define COM422_PPORT_READ_STAT_RXFULL(_context_) \
	COM422_PPORT_READ_STAT_BITS(_context_, COM422_CH_STAT_RXFULL_OFFSET, COM422_CH_STAT_RXFULL_MASK)

//---TXROOM

#define COM422_PPORT_READ_TXROOM(_context_) \
	READ_REGISTER_ULONG((_context_)->RegTxRoom)

//---RXSIZE

#define COM422_PPORT_READ_RXSIZE(_context_) \
	READ_REGISTER_ULONG((_context_)->RegRxSize)

//---RX-MAXDEPTH

#define COM422_PPORT_READ_RXMAXDEP(_context_, _channel_) \
	READ_REGISTER_ULONG((_context_)->RegRxMaxDep)

#define COM422_PPORT_WRITE_RXMAXDEP(_context_, _channel_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegRxMaxDep, _value_)

//---RX-CURDEPTH

#define COM422_PPORT_READ_RXCURDEP(_context_, _channel_) \
	READ_REGISTER_ULONG((_context_)->RegRxCurDep)

#define COM422_PPORT_WRITE_RXCURDEP(_context_, _channel_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegRxCurDep, (_value_))

//---TX-MAXDEPTH

#define COM422_PPORT_READ_TXMAXDEP(_context_, _channel_) \
	READ_REGISTER_ULONG((_context_)->RegTxMaxDep)

#define COM422_PPORT_WRITE_TXMAXDEP(_context_, _channel_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegTxMaxDep, _value_)

//---TX-CURDEPTH

#define COM422_PPORT_READ_TXCURDEP(_context_, _channel_) \
	READ_REGISTER_ULONG((_context_)->RegTxCurDep)

#define COM422_PPORT_WRITE_TXCURDEP(_context_, _channel_, _value_) \
	WRITE_REGISTER_ULONG((_context_)->RegTxCurDep, (_value_))

//---WRITE-BUFF

#define COM422_PPORT_WRITE_UCHARS(_context_, _buffer_, _count_) \
	WRITE_REGISTER_BUFFER_UCHAR((_context_)->RegTxBuff, (_buffer_), (_count_))

//---READ-BUFF

#define COM422_PPORT_READ_UCHARS(_context_, _buffer_, _count_) \
	READ_REGISTER_BUFFER_UCHAR((_context_)->RegRxBuff, (_buffer_), (_count_))

#define COM422_PPORT_PRINT_CTRL(_context_) \
	HrKdPrint(("CTRL >> ADDR: %p, VAL: %08X\n", (_context_)->RegCtrl, \
	READ_REGISTER_ULONG((_context_)->RegCtrl)))

#define COM422_PPORT_PRINT_STAT(_context_) \
	HrKdPrint(("STAT >> ADDR: %p, VAL: %08X\n", (_context_)->RegStat, \
	READ_REGISTER_ULONG((_context_)->RegStat)))

#else

#define COM422_WRITE_REGISTER_BARX(_context_, _bar_, _addr_, _value_)
#define COM422_READ_REGISTER_BARX(_context_, _bar_, _addr_)  0UL

#define COM422_WRITE_REGISTER(_context_, _addr_, _value_)
#define COM422_READ_REGISTER(_context_, _addr_)  0UL

/*---------------------------------------------------------------------*/

// global register

#define COM422_BUS_DEVID_ADDR(_context_) (0)
#define COM422_BUS_CHNUM_ADDR(_context_) (0)
#define COM422_BUS_INTSTAT_ADDR(_context_) (0)
#define COM422_BUS_INTMASK_ADDR(_context_) (0)
#define COM422_BUS_INTCTRL_ADDR(_context_) (0)

// channel register

#define COM422_BUS_CTRL_ADDR(_context_, _channel_) (0)
#define COM422_BUS_STAT_ADDR(_context_, _channel_) (0)
#define COM422_BUS_TXROOM_ADDR(_context_, _channel_) (0)
#define COM422_BUS_RXSIZE_ADDR(_context_, _channel_) (0)
#define COM422_BUS_RXMAXDEP_ADDR(_context_, _channel_) (0)
#define COM422_BUS_RXCURDEP_ADDR(_context_, _channel_) (0)
#define COM422_BUS_TXMAXDEP_ADDR(_context_, _channel_) (0)
#define COM422_BUS_TXCURDEP_ADDR(_context_, _channel_) (0)
#define COM422_BUS_TXBUFF_ADDR_BASE(_context_, _channel_) (0)
#define COM422_BUS_RXBUFF_ADDR_BASE(_context_, _channel_) (0)

/*-------global--------------------------------------------------------------*/

#define COM422_BUS_READ_DEVID(_context_) (0)
#define COM422_BUS_READ_CHNUM(_context_) COM422_MAX_PORT_COUNT
#define COM422_BUS_READ_INTSTAT(_context_) (0)
#define COM422_BUS_READ_INTMASK(_context_) (0)
#define COM422_BUS_WRITE_INTMASK(_context_, _value_)
#define COM422_BUS_WRITE_INTMASK_BIT(_context_, _bit_, _value_)
#define COM422_BUS_WRITE_INTMASK_BITS(_context_, _channel_, _offset_, _mask_, _value_)
#define COM422_BUS_READ_INTMASK_BITS(_context_, _channel_, _offset_, _mask_) (0)
#define COM422_BUS_READ_INTCTRL(_context_) (0)
#define COM422_BUS_WRITE_INTCTRL_BIT(_context_, _bit_, _value_)
#define COM422_BUS_WRITE_INTCTRL_BITS(_context_, _channel_, _offset_, _mask_, _value_)
#define COM422_BUS_READ_INTCTRL_BITS(_context_, _channel_, _offset_, _mask_) (0)

/*--------bus----------------------------------------------------------*/

//---CTRL

#define COM422_BUS_READ_CTRL(_context_, _channel_) (0)
#define COM422_BUS_WRITE_CTRL_BITS(_context_, _channel_, _offset_, _mask_, _value_) (0)
#define COM422_BUS_READ_CTRL_BITS(_context_, _channel_, _offset_, _mask_) (0)

//---STAT

#define COM422_BUS_READ_STAT(_context_, _channel_) (0)
#define COM422_BUS_READ_STAT_BITS(_context_, _channel_, _offset_, _mask_) (0)

//---STAT::detail

#define COM422_BUS_READ_STAT_TXEMPTY(_context_, _channel_) \
	COM422_BUS_READ_STAT_BITS(_context_, _channel_, COM422_CH_STAT_TXEMPTY_OFFSET, COM422_CH_STAT_TXEMPTY_MASK)

#define COM422_BUS_READ_STAT_TXFULL(_context_, _channel_) \
	COM422_BUS_READ_STAT_BITS(_context_, _channel_, COM422_CH_STAT_TXFULL_OFFSET, COM422_CH_STAT_TXFULL_MASK)

#define COM422_BUS_READ_STAT_TXHALF(_context_, _channel_) \
	COM422_BUS_READ_STAT_BITS(_context_, _channel_, COM422_CH_STAT_TXHALF_OFFSET, COM422_CH_STAT_TXHALF_MASK)

#define COM422_BUS_READ_STAT_RXEMPTY(_context_, _channel_) \
	COM422_BUS_READ_STAT_BITS(_context_, _channel_, COM422_CH_STAT_RXEMPTY_OFFSET, COM422_CH_STAT_RXEMPTY_MASK)

#define COM422_BUS_READ_STAT_RXFULL(_context_, _channel_) \
	COM422_BUS_READ_STAT_BITS(_context_, _channel_, COM422_CH_STAT_RXFULL_OFFSET, COM422_CH_STAT_RXFULL_MASK)

//---TXROOM

#define COM422_BUS_READ_TXROOM(_context_, _channel_) (0)

//---RXSIZE

#define COM422_BUS_READ_RXSIZE(_context_, _channel_) (0)

//---RX-MAXDEPTH

#define COM422_BUS_READ_RXMAXDEP(_context_, _channel_) (0)
#define COM422_BUS_WRITE_RXMAXDEP(_context_, _channel_, _value_)

//---RX-CURDEPTH

#define COM422_BUS_READ_RXCURDEP(_context_, _channel_) (0)
#define COM422_BUS_WRITE_RXCURDEP(_context_, _channel_, _value_)

//---TX-MAXDEPTH

#define COM422_BUS_READ_TXMAXDEP(_context_, _channel_)(0)
#define COM422_BUS_WRITE_TXMAXDEP(_context_, _channel_, _value_)

//---TX-CURDEPTH

#define COM422_BUS_READ_TXCURDEP(_context_, _channel_) (0)
#define COM422_BUS_WRITE_TXCURDEP(_context_, _channel_, _value_)

//---WRITE-BUFF

#define COM422_BUS_WRITE_UCHARS(_context_, _channel_, _buffer_, _count_) (0)

//---READ-BUFF

#define COM422_BUS_READ_UCHARS(_context_, _channel_, _buffer_, _count_) (0)
#define COM422_BUS_READ_USHORTS(_context_, _channel_, _buffer_, _count_) (0)

/*--------physical port-----------------------------------------------------------*/

//---GLOBAL

#define COM422_PPORT_READ_INTMASK(_context_) (0)
#define COM422_PPORT_WRITE_INTMASK(_context_, _value_)
#define COM422_PPORT_WRITE_INTMASK_BIT(_context_, _bit_, _value_)
#define COM422_PPORT_WRITE_INTMASK_BITS(_context_, _offset_, _mask_, _value_)
#define COM422_PPORT_READ_INTMASK_BITS(_context_, _offset_, _mask_) (0)

#define COM422_PPORT_READ_INTCTRL(_context_) (0)
#define COM422_PPORT_WRITE_INTCTRL(_context_, _value_)
#define COM422_PPORT_WRITE_INTCTRL_BITS(_context_, _offset_, _mask_, _value_)
#define COM422_PPORT_READ_INTCTRL_BITS(_context_, _offset_, _mask_) (0)

//---CTRL

#define COM422_PPORT_READ_CTRL(_context_) \
	((_context_)->RegCtrl)

#define COM422_PPORT_WRITE_CTRL_BIT(_context_, _bit_, _value_) \
	CONFIG_REG_BIT_ULONG((_context_)->RegCtrl, (_bit_), (_value_))

#define COM422_PPORT_WRITE_CTRL_BITS(_context_, _offset_, _mask_, _value_) \
	CONFIG_REG_BITS_ULONG((_context_)->RegCtrl, (_offset_), (_mask_), (_value_))

#define COM422_PPORT_READ_CTRL_BITS(_context_, _offset_, _mask_) \
	GET_REG_BITS_VAL((_context_)->RegCtrl, (_offset_), (_mask_))

//---STAT

#define COM422_PPORT_READ_STAT(_context_) \
	((_context_)->RegStat)

#define COM422_PPORT_READ_STAT_BITS(_context_, _offset_, _mask_) \
	GET_REG_BITS_VAL((_context_)->RegStat, (_offset_), (_mask_))

//---STAT::detail

#define COM422_PPORT_READ_STAT_TXEMPTY(_context_) \
	COM422_PPORT_READ_STAT_BITS(_context_, COM422_CH_STAT_TXEMPTY_OFFSET, COM422_CH_STAT_TXEMPTY_MASK)

#define COM422_PPORT_READ_STAT_TXFULL(_context_) \
	COM422_PPORT_READ_STAT_BITS(_context_, COM422_CH_STAT_TXFULL_OFFSET, COM422_CH_STAT_TXFULL_MASK)

#define COM422_PPORT_READ_STAT_TXHALF(_context_) \
	COM422_PPORT_READ_STAT_BITS(_context_, COM422_CH_STAT_TXHALF_OFFSET, COM422_CH_STAT_TXHALF_MASK)

#define COM422_PPORT_READ_STAT_RXEMPTY(_context_) \
	COM422_PPORT_READ_STAT_BITS(_context_, COM422_CH_STAT_RXEMPTY_OFFSET, COM422_CH_STAT_RXEMPTY_MASK)

#define COM422_PPORT_READ_STAT_RXFULL(_context_) \
	COM422_PPORT_READ_STAT_BITS(_context_, COM422_CH_STAT_RXFULL_OFFSET, COM422_CH_STAT_RXFULL_MASK)

//---TXROOM

#define COM422_PPORT_READ_TXROOM(_context_) \
	CircFifoSpace((_context_)->CircFifo)

//---RXSIZE

#define COM422_PPORT_READ_RXSIZE(_context_) \
	CircFifoSize((_context_)->CircFifo)

//---RX-MAXDEPTH

#define COM422_PPORT_READ_RXMAXDEP(_context_, _channel_) (0)
#define COM422_PPORT_WRITE_RXMAXDEP(_context_, _channel_, _value_)

//---RX-CURDEPTH

#define COM422_PPORT_READ_RXCURDEP(_context_, _channel_) (0)
#define COM422_PPORT_WRITE_RXCURDEP(_context_, _channel_, _value_)

//---TX-MAXDEPTH

#define COM422_PPORT_READ_TXMAXDEP(_context_, _channel_) (0)
#define COM422_PPORT_WRITE_TXMAXDEP(_context_, _channel_, _value_)

//---TX-CURDEPTH

#define COM422_PPORT_READ_TXCURDEP(_context_, _channel_) (0)
#define COM422_PPORT_WRITE_TXCURDEP(_context_, _channel_, _value_)

//---WRITE-BUFF

#define COM422_PPORT_WRITE_UCHARS(_context_, _buffer_, _count_) \
	{\
		ULONG _i_;\
		for (_i_ = 0; _i_ < _count_; _i_++) \
		{ \
			CircFifoWrite((_context_)->CircFifo, &_buffer_[_i_]); \
		} \
	}

//---READ-BUFF
/*
#define COM422_PPORT_READ_UCHARS(_context_, _buffer_, _count_) \
	{\
		ULONG _i_;\
		for (_i_ = 0; _i_ < _count_; _i_++) \
		{ \
			CircFifoRead((_context_)->CircFifo, &_buffer_[_i_]); \
		} \
	}
*/
#define COM422_PPORT_PRINT_CTRL(_context_) \
	HrKdPrint(("CTRL >> ADDR: %p, VAL: %08X\n", &(_context_)->RegCtrl, (_context_)->RegCtrl))

#define COM422_PPORT_PRINT_STAT(_context_) \
	HrKdPrint(("STAT >> ADDR: %p, VAL: %08X\n", &(_context_)->RegStat, (_context_)->RegStat))

#endif  // !DEBUG_WITH_REAL_DEVICE

/* --------------------------------------------------------------------
*
* 9-wire  serialport definition:
*
* ---------------------------------------------------------------------
* |针号|  1  |  2 |  3 |  4  |  5 |   6 |  7 |  8 |  9 |
* ---------------------------------------------------------------------
* |缩写| DCD | RXD| TXD| DTR | GND|  DSR| RTS| CTS|DELL|
* ---------------------------------------------------------------------
* |功能|数据载|接收|发送|数据终|信号|数据设|请求|清除|振铃|
* |说明|波检测|数据|数据|端就绪|地  |备就绪|发送|发送|指示|
*
* 其中，RTS/CTS为流控制
* --------------------------------------------------------------------*/

#endif	// _COM422_REGISTER_H_
