#include "StdAfx.h"
#include "DataContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "comapi.lib")

CDataContainer::CDataContainer(void) :
	KPairThread(_T(__FUNCTION__)_T("-serialport"))
{
	this->initialize();
}

CDataContainer::~CDataContainer(void)
{
	this->close();
}

BOOL CDataContainer::initialize()
{
	m_commInfo.Port = 1;
	m_commInfo.BaudRate = 9600;
	m_commInfo.DataBits = 8;
	m_commInfo.Parity = NOPARITY;
	m_commInfo.StopBits = ONESTOPBIT;

	return TRUE;
}

bool CDataContainer::workFirst()
{
	Sleep(10);
	
	return false;
}

bool CDataContainer::workSecond()
{
	Sleep(10);

	return false;
}

int CDataContainer::open(int port)
{
	return 0;
}

int CDataContainer::close()
{
	return 0;
}
