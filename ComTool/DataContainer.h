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
	int				Period;		// �������� (��λ��ms)
	BOOL			EnPeriod;	// ���ͷ�ʽ��0:������1:���ڣ�
	ULONG			Count;		// �ۼƷ��������ֽ���
	BOOL			Enable;		// ʹ��/��ֹ����
	MmTimer			MmTimer;	// �߾��ȶ�ʱ����ģ��
	KMutex			mutex;		// ������
	COMM_BUFFER_INFO Buffer;	// ���ݻ�����

} COMM_INFO_TX, *PCOMM_INFO_TX;

// 
typedef struct _COMM_INFO_RX
{
	int				DataFormat;	// ֡���ݸ�ʽ���˽��ƣ�ʮ���ƣ�ʮ�����ƣ�
	ULONG			Count;		// �ۼƽ��������ֽ���
	BOOL			Enable;		// ʹ��/��ֹ����
	MmTimer			MmTimer;	// �߾��ȶ�ʱ����ģ��
	KMutex			mutex;		// ������
	std::list<COMM_BUFFER_INFO> Buffer;	// ���ݻ�����
	
} COMM_INFO_RX, *PCOMM_INFO_RX;

// 
typedef struct _COMM_INFO
{
	int				Port;		// �˿ںţ�1 ~ 256��
	int				BaudRate;	// ������
	int				DataBits;	// ����λ
	int				Parity;		// У��λ
	int				StopBits;	// ֹͣλ

	SerialPort		serialPort;	// ���ڶ���

	COMM_INFO_TX	TX;			// ���Ͷ���Ϣ
	COMM_INFO_RX	RX;			// ���ն���Ϣ

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
